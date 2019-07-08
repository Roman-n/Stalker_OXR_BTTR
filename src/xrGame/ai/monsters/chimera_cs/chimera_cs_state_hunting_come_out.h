#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class	CStateChimera_csHuntingComeOut : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;

public:
						CStateChimera_csHuntingComeOut	(_Object *obj);

	virtual	void		reselect_state				();
	virtual bool 		check_start_conditions		();	
	virtual bool 		check_completion			();	

};

#include "Chimera_cs_state_hunting_come_out_inline.h"
#endif