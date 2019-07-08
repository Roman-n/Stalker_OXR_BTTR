#pragma once
#ifdef CHIMERA_CS
#include "Chimera_cs_state_hunting_move_to_cover.h"
#include "Chimera_cs_state_hunting_come_out.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimera_csHuntingAbstract CStateChimera_csHunting<_Object>

TEMPLATE_SPECIALIZATION
CStateChimera_csHuntingAbstract::CStateChimera_csHunting(_Object *obj) : inherited(obj)
{
	add_state(eStateMoveToCover,	xr_new<CStateChimera_csHuntingMoveToCover<_Object> >	(obj));
	add_state(eStateComeOut,		xr_new<CStateChimera_csHuntingComeOut<_Object> >		(obj));
}


TEMPLATE_SPECIALIZATION
bool CStateChimera_csHuntingAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimera_csHuntingAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csHuntingAbstract::reselect_state()
{
	if (prev_substate == u32(-1))					select_state(eStateMoveToCover);
	else if (prev_substate == eStateMoveToCover)	select_state(eStateComeOut);
	else											select_state(eStateMoveToCover);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csHuntingAbstract
#endif