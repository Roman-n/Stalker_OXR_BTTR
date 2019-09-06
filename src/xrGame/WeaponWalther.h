#pragma once

//#include "weaponrevolver.h"
#include "WeaponPistol.h"


class CWeaponWalther :
	public CWeaponPistol
{
	typedef CWeaponPistol inherited;
public:
	CWeaponWalther(void);
	virtual ~CWeaponWalther(void);
};
