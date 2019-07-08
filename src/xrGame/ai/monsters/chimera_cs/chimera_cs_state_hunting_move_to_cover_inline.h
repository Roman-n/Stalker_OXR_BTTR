#pragma once
##ifdef CHIMERA_CS
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimera_csHuntingMoveToCoverAbstract CStateChimera_csHuntingMoveToCover<_Object>

TEMPLATE_SPECIALIZATION
CStateChimera_csHuntingMoveToCoverAbstract::CStateChimera_csHuntingMoveToCover(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csHuntingMoveToCoverAbstract::initialize()
{
	inherited::initialize();
	
	
}

TEMPLATE_SPECIALIZATION
bool CStateChimera_csHuntingMoveToCoverAbstract::check_completion()
{
	return false;
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csHuntingMoveToCoverAbstract::execute()
{
	
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csHuntingMoveToCoverAbstract
#endif