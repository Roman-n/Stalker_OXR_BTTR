#pragma once
#include "../monster_state_manager.h"

class CAI_dogsoc;

class CStateManagerdogsoc : public CMonsterStateManager<CAI_dogsoc> {
	typedef CMonsterStateManager<CAI_dogsoc> inherited;

public:

					CStateManagerdogsoc	(CAI_dogsoc *monster); 
	virtual void	execute				();
	virtual void	remove_links		(CObject* object) { inherited::remove_links(object);}
};
