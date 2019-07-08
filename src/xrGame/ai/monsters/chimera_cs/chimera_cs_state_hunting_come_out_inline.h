#pragma once
#ifdef CHIMERA_CS
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimera_csHuntingMoveToCoverAbstract CStateChimera_csHuntingMoveToCover<_Object>

TEMPLATE_SPECIALIZATION
CStateChimera_csHuntingMoveToCoverAbstract::CStateChimera_csHuntingMoveToCover(_Object *obj) : inherited(obj)
{
}


TEMPLATE_SPECIALIZATION
bool CStateChimera_csHuntingMoveToCoverAbstract::check_start_conditions()
{
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateChimera_csHuntingMoveToCoverAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csHuntingMoveToCoverAbstract::reselect_state()
{
	if (prev_substate == u32(-1))					select_state(eStateMoveToCover);
	else if (prev_substate == eStateMoveToCover)	select_state(eStateComeOut);
	else											select_state(eStateMoveToCover);
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csHuntingMoveToCoverAbstract
#endif