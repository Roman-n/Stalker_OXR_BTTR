#pragma once
#ifdef CHIMERA_CS
#include "../state.h"

template<typename _Object>
class CStateChimera_csThreatenRoar : public CState<_Object> {
	typedef CState<_Object>		inherited;

public:
	IC					CStateChimera_csThreatenRoar	(_Object *obj) : inherited(obj){}
	
	virtual	void		initialize					();	
	virtual	void		execute						();
	virtual bool		check_completion			();
	virtual void		remove_links				(IGameObject* object) { inherited::remove_links(object);}
};

#include "Chimera_cs_state_threaten_roar_inline.h"
#endif
