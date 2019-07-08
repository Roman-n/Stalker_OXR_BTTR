#pragma once
#ifdef CHIMERA_CS
#include "../monster_state_manager.h"

class CChimera_cs;

class CStateManagerChimera_cs : public CMonsterStateManager<CChimera_cs> {
	
	typedef CMonsterStateManager<CChimera_cs> inherited;

public:
						CStateManagerChimera_cs	(CChimera_cs *obj);
	virtual				~CStateManagerChimera_cs	();

	virtual	void		execute					();
    virtual void remove_links(IGameObject* object) { inherited::remove_links(object); }
};
#endif
