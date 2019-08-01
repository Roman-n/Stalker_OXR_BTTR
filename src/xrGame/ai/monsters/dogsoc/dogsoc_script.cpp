#include "pch_script.h"
#include "dogsoc.h"

using namespace luabind;

#pragma optimize("s",on)
void CAI_dogsoc::script_register(lua_State *L)
{
	module(L)
	[
		class_<CAI_dogsoc,CGameObject>("CAI_dogsoc")
			.def(constructor<>())
	];
}
