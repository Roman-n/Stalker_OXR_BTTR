#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class	CStateChimera_csThreaten : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;
	typedef CState<_Object>* state_ptr;

	enum {
		eStateWalk			= u32(0),
		eStateFaceEnemy,
		eStateThreaten,
		eStateSteal
	};

	u32					m_last_time_threaten;

public:
						CStateChimera_csThreaten	(_Object *obj);
	virtual				~CStateChimera_csThreaten	();

	virtual void		reinit					();

	virtual	void		initialize				();

	virtual	void		reselect_state			();
	virtual void 		finalize				();
	virtual void 		critical_finalize		();
	virtual bool 		check_start_conditions	();	
	virtual bool 		check_completion		();	
	virtual void        remove_links(IGameObject* object) { inherited::remove_links(object); }
};

#include "Chimera_cs_state_threaten_inline.h"
#endif
