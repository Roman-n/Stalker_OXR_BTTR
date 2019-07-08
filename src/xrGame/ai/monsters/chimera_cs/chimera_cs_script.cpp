#include "pch_script.h"
#ifdef CHIMERA_CS
#include "Chimera_cs.h"
#include "xrScriptEngine/ScriptExporter.hpp"

    using namespace luabind;

SCRIPT_EXPORT(
    CChimera_cs, (CGameObject), { module(luaState)[class_<CChimera_cs, CGameObject>("CChimera_cs").def(constructor<>())]; });
#endif
