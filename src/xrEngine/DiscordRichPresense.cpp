#include "stdafx.h"
#ifdef __DISCORD_API
#include "DiscordRichPresense.h"
#include "DiscordRichPresense/discord_register.h"
#include "DiscordRichPresense/discord_rpc.h"
#include "../xrCore/StringsUtils.h"
#include "iGame_Level.h"
#include "x_ray.h"

ENGINE_API xrDiscordPresense g_discord;

void xrDiscordPresense::Initialize()
{
	// We don't have multiplayer mode, so no need to invite system to support
	DiscordEventHandlers nullHandlers;
	ZeroMemory(&nullHandlers, sizeof(nullHandlers));
	Discord_Initialize("602204664448155649", &nullHandlers, TRUE, nullptr);
	bInitialize = true;
}

void xrDiscordPresense::Shutdown()
{
	if (bInitialize)
	{
		Discord_Shutdown();
		bInitialize = false;
	}
}

void xrDiscordPresense::SetStatus(StatusId status)
{
	if (!bInitialize) return;

	DiscordRichPresence presenseInfo;
	memset(&presenseInfo, 0, sizeof(presenseInfo));

	StatusId realStatus = status;
	if (status == StatusId::In_Game)
	{
		// get level name, and set status to different value, when we found vanilla levels
		if (pApp->Level_Current < pApp->Levels.size())
		{
			CApplication::sLevelInfo& LevelInfo = pApp->Levels[pApp->Level_Current];
			if (xr_strcmp(LevelInfo.name, "zaton") == 0)
			{
				realStatus = StatusId::Zaton;
			}
			if (xr_strcmp(LevelInfo.name, "jupiter") == 0)
			{
				realStatus = StatusId::Upiter;
			}
			if (xr_strcmp(LevelInfo.name, "pripyat") == 0)
			{
				realStatus = StatusId::Pripyat;
			}
		}
	}

	presenseInfo.largeImageText = "Zone Awaits...";
	presenseInfo.details = "Level: ";

	bool bOnLevel = true;

	switch (realStatus)
	{
	case StatusId::In_Game:	presenseInfo.largeImageKey	= "zaton"; break;
	case StatusId::Zaton:	presenseInfo.largeImageKey  = "zaton"; break;
	case StatusId::Upiter:	presenseInfo.largeImageKey  = "ypiter"; break;
	case StatusId::Pripyat:	presenseInfo.largeImageKey  = "pripyat"; break;
	case StatusId::Menu:
	default:
		presenseInfo.details		= "Main Menu";
		presenseInfo.largeImageKey	= "menu_final";
		bOnLevel = false;
		break;
	}
	// Add levelname to details
	if (bOnLevel && g_pGameLevel && g_pGameLevel->name().c_str())
	{
		string64 LevelName;
		strconcat(sizeof(LevelName), LevelName, "Level: ", ConvertToUTF8(g_pGameLevel->name().c_str()));
		presenseInfo.details = std::move(LevelName);
	}
	else if (!g_pGameLevel)
	{
		presenseInfo.details = "Loading...";
	}

	Discord_UpdatePresence(&presenseInfo);
}

xrDiscordPresense::~xrDiscordPresense()
{
	Shutdown();
}
#endif
