#pragma once
#ifdef CHIMERA_CS
#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateChimera_csAttackRunAbstract CStateChimera_csAttackRun<_Object>

TEMPLATE_SPECIALIZATION
CStateChimera_csAttackRunAbstract::CStateChimera_csAttackRun(_Object *obj) : inherited(obj)
{
//	add_state(eStateAttack_MoveToHomePoint,	xr_new<CStateMonsterAttackMoveToHomePoint<CChimera_cs> >(obj));	
 	add_state(eStateAttack_Run_chimera,				xr_new<CStateMonsterAttackRun<CChimera_cs> >			(obj));
 	add_state(eStateAttack_Melee_chimera,			xr_new<CStateMonsterAttackMelee<CChimera_cs> >			(obj));	
}

TEMPLATE_SPECIALIZATION
CStateChimera_csAttackRunAbstract::~CStateChimera_csAttackRun()
{
}

TEMPLATE_SPECIALIZATION
bool CStateChimera_csAttackRunAbstract::check_home_point_cs()
{
//	if (prev_substate != eStateAttack_MoveToHomePoint) {
//		if (get_state(eStateAttack_MoveToHomePoint)->check_start_conditions())	return true;
//	} else {
//		if (!get_state(eStateAttack_MoveToHomePoint)->check_completion())		return true;
//	}
//
//	return false;
}


TEMPLATE_SPECIALIZATION
void CStateChimera_csAttackRunAbstract::initialize()
{
	inherited::initialize			();
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csAttackRunAbstract::execute()
{
	object->anim().clear_override_animation	();
	
//	if	( check_home_point_cs() )
//	{
//		select_state					(eStateAttack_MoveToHomePoint);
//		get_state_current()->execute	();
//		prev_substate				=	current_substate;
//		return;
//	}	
	
	EMonsterState		state_id	=	eStateUnknown;
	const CEntityAlive* enemy		=	object->EnemyMan.get_enemy();

	if (current_substate == eStateAttack_Melee_chimera)
	{
		if (get_state(eStateAttack_Melee_chimera)->check_completion())
			state_id = eStateAttack_Run_chimera;
		else
			state_id = eStateAttack_Melee_chimera;
	}
	else
	{
		if ( get_state(eStateAttack_Melee_chimera)->check_start_conditions() )
			state_id = eStateAttack_Melee_chimera;
		else
			state_id = eStateAttack_Run_chimera;
	}
	
	select_state						(state_id);
	get_state_current()->execute		();
	prev_substate					=	current_substate;	
	
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csAttackRunAbstract::choose_action()
{
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csAttackRunAbstract::finalize_cs()
{
	inherited::finalize();
}

TEMPLATE_SPECIALIZATION
void CStateChimera_csAttackRunAbstract::critical_finalize_cs()
{
	inherited::critical_finalize();
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateChimera_csAttackRunAbstract

#endif