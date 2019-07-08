#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class CStateChimera_csThreatenWalk : public CStateMonsterMoveToPointEx<_Object> {
	typedef CStateMonsterMoveToPointEx<_Object>		inherited;

public:
	IC					CStateChimera_csThreatenWalk	(_Object *obj) : inherited(obj){}
	virtual	void		initialize					();	
	virtual	void		execute						();
	virtual bool		check_completion			();
	virtual bool		check_start_conditions		();
	
};

#include "Chimera_cs_state_threaten_walk_inline.h"
#endif