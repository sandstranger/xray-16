#include "pch_script.h"
#include "Scope.h"
#include "Silencer.h"
#include "GrenadeLauncher.h"

void CScope::script_register(lua_State* luaState)
{
    using namespace luabind;

    module(luaState)
    [
        class_<CScope, CGameObject>("CScope")
            .def(constructor<>()),

        class_<CSilencer, CGameObject>("CSilencer")
            .def(constructor<>()),

        class_<CGrenadeLauncher, CGameObject>("CGrenadeLauncher")
            .def(constructor<>())
    ];
}
