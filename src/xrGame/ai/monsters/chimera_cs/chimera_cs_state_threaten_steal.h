#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class CStateChimera_csThreatenSteal : public CStateMonsterMoveToPointEx<_Object> {
	typedef CStateMonsterMoveToPointEx<_Object>		inherited;

public:
	IC					CStateChimera_csThreatenSteal	(_Object *obj) : inherited(obj){}
	virtual	void		initialize					();	
	virtual void		finalize					();
	virtual	void		execute						();
	virtual bool		check_completion			();
	virtual bool		check_start_conditions		();
};

#include "Chimera_cs_state_threaten_steal_inline.h"
#endif