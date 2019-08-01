#pragma once
#ifdef DOG_SOC
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"

class CAI_dogsoc : public CBaseMonster, 
				public CControlledEntity<CAI_dogsoc> {
	
	typedef		CBaseMonster				inherited;
	typedef		CControlledEntity<CAI_dogsoc>	CControlled;

public:
					CAI_dogsoc				();
	virtual			~CAI_dogsoc			();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();

	virtual void	CheckSpecParams		(u32 spec_params);

	virtual bool	ability_can_drag		() {return true;}
    pcstr           get_monster_class_name  () override { return "dogsoc";}
	
};

#endif
