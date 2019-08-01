#pragma once

#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../../xrServerEntities/script_export_space.h"

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
	virtual	char*	get_monster_class_name () { return "dogsoc"; }
	

private:
#ifdef _DEBUG	
	virtual void	debug_on_key		(int key);
#endif



	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_dogsoc)
#undef script_type_list
#define script_type_list save_type_list(CAI_dogsoc)
