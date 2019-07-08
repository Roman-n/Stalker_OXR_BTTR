#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class	CStateChimera_csHuntingMoveToCover : public CState<_Object> {
protected:
	typedef CState<_Object> inherited;

public:
						CStateChimera_csHuntingMoveToCover	(_Object *obj);

	virtual void		initialize						();
	virtual	void		execute							();
	virtual bool 		check_start_conditions			();
	virtual bool 		check_completion				();
};

#include "Chimera_cs_state_hunting_move_to_cover_inline.h"
#endif