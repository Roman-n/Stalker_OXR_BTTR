#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class	CStateChimera_csHunting : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;
	typedef CState<_Object>* state_ptr;

	enum {
		eStateMoveToCover,
		eStateComeOut
	};

public:
						CStateChimera_csHunting	(_Object *obj);

	virtual	void		reselect_state			();
	virtual bool 		check_start_conditions	();	
	virtual bool 		check_completion		();	

};

#include "Chimera_cs_state_hunting_inline.h"
#endif