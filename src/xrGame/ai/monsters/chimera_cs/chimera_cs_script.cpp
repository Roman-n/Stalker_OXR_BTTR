#include "pch_script.h"
#ifdef CHIMERA_CS
#include "Chimera_cs.h"
//
using namespace luabind;

#pragma optimize("s",on)
void CChimera_cs::script_register(lua_State *L)
{
	module(L)
	[
		class_<CChimera_cs,CGameObject>("CChimera_cs")
			.def(constructor<>())
	];
}
#endif