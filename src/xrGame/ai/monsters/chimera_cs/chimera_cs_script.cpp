#include "pch_script.h"
#ifdef CHIMERA_CS
#include "chimera_cs.h"
#include "xrScriptEngine/ScriptExporter.hpp"

    using namespace luabind;

SCRIPT_EXPORT(
    CChimecs, (CGameObject), { module(luaState)[class_<CChimecs, CGameObject>("CChimecs").def(constructor<>())]; });
#endif
