#pragma once
#ifdef POLTERGEIST_CS
#include "../monster_state_manager.h"

class CPoltergeist_cs;

class CStateManagerPoltergeist_cs : public CMonsterStateManager<CPoltergeist_cs> {
	typedef CMonsterStateManager<CPoltergeist_cs> inherited;

    CPolterSpecialAbility_cs* m_tele_cs; // Тип полтергейста m_tele_cs
    CPolterSpecialAbility_cs* m_flame;

public:
						CStateManagerPoltergeist_cs		(CPoltergeist_cs *obj);
	virtual				~CStateManagerPoltergeist_cs	();

	virtual void		reinit						();
	virtual	void		execute						();
	virtual void		remove_links				(IGameObject* object) { inherited::remove_links(object);}

private:

			u32			time_next_flame_attack;
			u32			time_next_tele_attack;
			u32			time_next_scare_attack;

};
#endif
