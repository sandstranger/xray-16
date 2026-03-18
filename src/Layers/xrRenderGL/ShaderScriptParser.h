
#pragma once

#include "xrCommon/xr_string.h"
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "glad/gl.h"

using namespace std;
namespace fs = filesystem;

namespace xray::render::RENDER_NAMESPACE
{

struct SamplerInfo
{
    string name;
    u32 index;
    string texture;

    SamplerInfo() : index(u32(-1)) {}
    SamplerInfo(const string& n, u32 i, const string& t)
        : name(n), index(i), texture(t) {}
};

struct ShaderPassInfo
{
    string vs;
    string gs;
    string ps;
    bool fog;
    bool emissive;
    bool z_test;
    bool z_write;
    vector<SamplerInfo> samplers;
};

struct ShaderFunctionInfo
{
    string name;
    vector<ShaderPassInfo> passes;
};

vector<string> GetUniforms(GLuint program);

class ShaderScriptParser
{
public:
    ShaderScriptParser();
    ~ShaderScriptParser();
    void ParseShaderSamplers(GLuint program, const string &shaderName);

private:
    vector<ShaderFunctionInfo> m_functions;
    bool ParseFunction(const char*& ptr, ShaderFunctionInfo& function);
    bool ParsePass(const char*& ptr, ShaderPassInfo& pass);
    bool ParseSampler(const char*& ptr, SamplerInfo& sampler);
    bool ParseBool(const char*& ptr, bool& value);
    bool ParseString(const char*& ptr, string& str);
    void SkipWhitespace(const char*& ptr);
    bool SkipComment(const char*& ptr);
    bool MatchKeyword(const char*& ptr, const char* keyword);
    bool ParseFile(const string filename);
    string ReadFileToString(const filesystem::path& filepath);
    bool ParseString(const char* script);
    [[nodiscard]] const vector<ShaderFunctionInfo>& GetShaderFunctions() const { return m_functions; }
    vector<string> GetSFilesAsStrings(const fs::path& directory);
    const ShaderFunctionInfo* GetShaderFunction(const char* name) const;
    void Clear();
};

}
