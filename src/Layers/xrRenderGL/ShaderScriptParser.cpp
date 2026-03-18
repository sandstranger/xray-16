
#include "stdafx.h"
#include "ShaderScriptParser.h"
#include <fstream>
#include "unordered_map"
#include "shader_uniforms_parser.h"

#pragma hdrstop

namespace xray::render::RENDER_NAMESPACE
{

static vector<ShaderFunctionInfo> parsedShaders;
static unordered_map<GLuint, vector<string>> parsedUniformsCache;

static void fillUniformsCacheWithDefaultValues (GLuint program)
{
    if (!parsedUniformsCache.contains(program)){
        parsedUniformsCache[program] = getUniforms(program);
    }
}

vector<string> GetUniforms(GLuint program)
{
    return parsedUniformsCache.contains(program) ? parsedUniformsCache.at(program) : vector<string>();
}

static vector<string> split(const string& s, char delimiter)
{
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        if (!token.empty()){
            tokens.push_back(token);
        }
    }
    return tokens;
}

ShaderScriptParser::ShaderScriptParser()
{
}

ShaderScriptParser::~ShaderScriptParser()
{
    Clear();
}

void ShaderScriptParser::Clear()
{
    m_functions.clear();
}

bool ShaderScriptParser::ParseFile(const string filename)
{
    string fileContent = ReadFileToString(filename);
    return !fileContent.empty() && ParseString(fileContent.c_str());
}

bool ShaderScriptParser::ParseString(const char* script)
{
    Clear();

    const char* ptr = script;

    while (*ptr)
    {
        SkipWhitespace(ptr);
        if (*ptr == '\0')
            break;

        if (*ptr == '-' && *(ptr + 1) == '-')
        {
            SkipComment(ptr);
            continue;
        }

        if (MatchKeyword(ptr, "function"))
        {
            ShaderFunctionInfo function;
            if (ParseFunction(ptr, function))
            {
                m_functions.push_back(function);
            }
        }
        else
        {
            while (*ptr && *ptr != '\n' && *ptr != '\r')
                ptr++;
        }
    }

    return !m_functions.empty();
}

const ShaderFunctionInfo* ShaderScriptParser::GetShaderFunction(const char* name) const
{
    for (const auto& func : m_functions)
    {
        if (func.name == name)
            return &func;
    }
    return nullptr;
}

vector<string> ShaderScriptParser::GetSFilesAsStrings(const fs::path& directory)
{
    vector<string> files;
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return files;
    }
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".s") {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

string ShaderScriptParser::ReadFileToString(const filesystem::path& filepath)
{
    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        return "";
    }
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    string buffer(size, '\0');
    if (file.read(buffer.data(), size)) {
        return buffer;
    }
    return "";
}

void ShaderScriptParser::ParseShaderSamplers(GLuint program, const string &shaderName)
{
    if (parsedShaders.empty()) {
        const auto files = GetSFilesAsStrings("gamedata/shaders/gl");
        if (files.empty()) {
            fillUniformsCacheWithDefaultValues(program);
            return;
        }

        for (const auto &filename : files) {
            Clear();
            if (ParseFile(filename)) {
                for (const auto &item: m_functions){
                    parsedShaders.push_back(item);
                }
            }
        }
    }

    auto shaderParts = split(shaderName, '|');

    if (shaderParts.size() <2){
        fillUniformsCacheWithDefaultValues(program);
        return;
    }

    const string &psName = shaderParts[1];
    const string &vsName = shaderParts[0];
    vector<string> result;

    for (const auto &func: parsedShaders) {
        for (const auto &pass: func.passes) {
            if ((!pass.ps.empty() && psName.find(pass.ps) != string::npos) &&
                (!pass.vs.empty() && vsName.find(pass.vs) != string::npos)) {
                for (const auto &sampler: pass.samplers) {
                    result.push_back(sampler.name);
                }

                if (result.empty()){
                    fillUniformsCacheWithDefaultValues(program);
                } else if (!parsedUniformsCache.contains(program)){
                    parsedUniformsCache[program] = result;
                }
                return;
            }
        }
    }

    fillUniformsCacheWithDefaultValues(program);
}

void ShaderScriptParser::SkipWhitespace(const char*& ptr)
{
    while (*ptr && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r'))
        ptr++;
}

bool ShaderScriptParser::SkipComment(const char*& ptr)
{
    if (*ptr == '-' && *(ptr + 1) == '-')
    {
        while (*ptr && *ptr != '\n' && *ptr != '\r')
            ptr++;
        return true;
    }
    return false;
}

bool ShaderScriptParser::MatchKeyword(const char*& ptr, const char* keyword)
{
    size_t len = strlen(keyword);
    if (strncmp(ptr, keyword, len) == 0)
    {
        ptr += len;
        return true;
    }
    return false;
}

bool ShaderScriptParser::ParseFunction(const char*& ptr, ShaderFunctionInfo& function)
{
    SkipWhitespace(ptr);

    if (!ParseString(ptr, function.name))
        return false;

    SkipWhitespace(ptr);

    if (*ptr == '(')
    {
        ptr++;
        int depth = 1;
        while (*ptr && depth > 0)
        {
            if (*ptr == '(') depth++;
            if (*ptr == ')') depth--;
            ptr++;
        }
    }

    while (*ptr)
    {
        SkipWhitespace(ptr);

        if (MatchKeyword(ptr, "end"))
        {
            SkipWhitespace(ptr);
            return true;
        }

        if (MatchKeyword(ptr, "shader"))
        {
            SkipWhitespace(ptr);
            if (*ptr == ':')
            {
                ptr++;
                SkipWhitespace(ptr);

                if (MatchKeyword(ptr, "begin"))
                {
                    ShaderPassInfo pass;
                    if (ParsePass(ptr, pass))
                    {
                        function.passes.push_back(pass);
                    }
                }
                else if (MatchKeyword(ptr, "sampler"))
                {
                    if (!function.passes.empty())
                    {
                        SamplerInfo sampler;
                        if (ParseSampler(ptr, sampler))
                        {
                            function.passes.back().samplers.push_back(sampler);
                        }
                    }
                }
            }
        }
        else
        {
            while (*ptr && *ptr != '\n' && *ptr != '\r')
                ptr++;
        }
    }

    return !function.passes.empty();
}

bool ShaderScriptParser::ParsePass(const char*& ptr, ShaderPassInfo& pass)
{
    SkipWhitespace(ptr);
    if (*ptr == '(')
    {
        ptr++;
        SkipWhitespace(ptr);

        if (!ParseString(ptr, pass.vs))
            return false;

        SkipWhitespace(ptr);

        if (*ptr == ',')
        {
            ptr++;
            SkipWhitespace(ptr);
            if (!ParseString(ptr, pass.ps))
                return false;

            SkipWhitespace(ptr);
            if (*ptr == ',')
            {
                ptr++;
                SkipWhitespace(ptr);
                if (!ParseString(ptr, pass.gs))
                    return false;
            }
        }

        SkipWhitespace(ptr);
        if (*ptr != ')')
            return false;
        ptr++;
    }

    while (*ptr)
    {
        SkipWhitespace(ptr);

        if (*ptr == '\n' || *ptr == '\r' || MatchKeyword(ptr, "end"))
            break;

        if (MatchKeyword(ptr, "shader"))
        {
            SkipWhitespace(ptr);
            if (*ptr == ':')
            {
                ptr++;
                SkipWhitespace(ptr);

                if (MatchKeyword(ptr, "sampler"))
                {
                    SamplerInfo sampler;
                    if (ParseSampler(ptr, sampler))
                    {
                        pass.samplers.push_back(sampler);
                    }
                }
            }
            continue;
        }

        if (*ptr == ':')
        {
            ptr++;
            SkipWhitespace(ptr);

            if (MatchKeyword(ptr, "fog"))
            {
                SkipWhitespace(ptr);
                ParseBool(ptr, pass.fog);
            }
            else if (MatchKeyword(ptr, "emissive"))
            {
                SkipWhitespace(ptr);
                ParseBool(ptr, pass.emissive);
            }
            else if (MatchKeyword(ptr, "zb"))
            {
                SkipWhitespace(ptr);
                if (*ptr == '(')
                {
                    ptr++;
                    SkipWhitespace(ptr);
                    ParseBool(ptr, pass.z_test);
                    SkipWhitespace(ptr);
                    if (*ptr == ',')
                    {
                        ptr++;
                        SkipWhitespace(ptr);
                        ParseBool(ptr, pass.z_write);
                    }
                    SkipWhitespace(ptr);
                    if (*ptr == ')')
                        ptr++;
                }
            }
        }
        else
        {
            while (*ptr && *ptr != '\n' && *ptr != '\r' && *ptr != ':')
                ptr++;
        }
    }

    return true;
}

    bool ShaderScriptParser::ParseSampler(const char*& ptr, SamplerInfo& sampler)
    {
        SkipWhitespace(ptr);

        if (*ptr == '(')
        {
            ptr++;
            SkipWhitespace(ptr);
            if (!ParseString(ptr, sampler.name))
                return false;
            SkipWhitespace(ptr);
            if (*ptr != ')')
                return false;
            ptr++;
        }

        static u32 nextIndex = 0;
        sampler.index = nextIndex++;

        while (*ptr)
        {
            SkipWhitespace(ptr);

            if (strncmp(ptr, "shader", 6) == 0 || strncmp(ptr, "end", 3) == 0)
                break;

            if (*ptr == '\n' || *ptr == '\r')
                break;

            if (*ptr == ':')
            {
                ptr++;
                SkipWhitespace(ptr);

                if (MatchKeyword(ptr, "texture"))
                {
                    SkipWhitespace(ptr);
                    if (*ptr == '(')
                    {
                        ptr++;
                        SkipWhitespace(ptr);
                        ParseString(ptr, sampler.texture);
                        SkipWhitespace(ptr);
                        if (*ptr == ')')
                            ptr++;
                    }
                }
            }
            else
            {
                while (*ptr && *ptr != '\n' && *ptr != '\r' && *ptr != ':')
                    ptr++;
            }
        }

        return true;
    }

bool ShaderScriptParser::ParseBool(const char*& ptr, bool& value)
{
    SkipWhitespace(ptr);

    if (MatchKeyword(ptr, "true"))
    {
        value = true;
        return true;
    }
    else if (MatchKeyword(ptr, "false"))
    {
        value = false;
        return true;
    }

    return false;
}

bool ShaderScriptParser::ParseString(const char*& ptr, string& str)
{
    SkipWhitespace(ptr);

    if (*ptr == '\"')
    {
        ptr++;
        const char* start = ptr;
        while (*ptr && *ptr != '\"')
            ptr++;
        if (*ptr == '\"')
        {
            str.assign(start, ptr - start);
            ptr++;
            return true;
        }
    }
    else
    {
        const char* start = ptr;
        while (*ptr && isalnum(*ptr) && *ptr != ' ' && *ptr != '\t' && 
               *ptr != '\n' && *ptr != '\r' && *ptr != '(' && *ptr != ')' && 
               *ptr != ',' && *ptr != ':')
            ptr++;
        if (start != ptr)
        {
            str.assign(start, ptr - start);
            return true;
        }
    }

    return false;
}
}
