#pragma once

// CONFIG_PROFILE_LOCKS
// XXX: convert to command line params
// CONFIG_SCRIPT_ENGINE_LOG_EXPORTS
// CONFIG_SCRIPT_ENGINE_LOG_SKIPPED_EXPORTS

/* Callbacks */
#define EXTENDED_ITEM_CALLBACKS // (eItemToBelt, eItemToSlot, eItemToRuck)
#define EXTENDED_WEAPON_CALLBACKS // (eOnWeaponZoomIn, eOnWeaponZoomOut, eOnWeaponJammed)
#define INPUT_CALLBACKS // (eKeyPress, eKeyRelease, eKeyHold, eMouseMove, eMouseWheel)
#define ENGINE_LUA_ALIFE_STORAGE_MANAGER_CALLBACKS // calls lua functions from engine in a script named alife_storage_manager.script  (alife_storage_manager.CALifeStorageManager_save) and (alife_storage_manager.CALifeStorageManager_load)
#define ENGINE_LUA_ALIFE_UPDAGE_MANAGER_CALLBACKS // calls lua function named on_before_change_level and on_after_new_game in _G.script when enabled

/* Scripts */
#define MORE_INVENTORY_SLOTS // Adds 5 more slots CUSTOM_SLOT_1..5
#define GAME_OBJECT_EXTENDED_EXPORTS // see: script_game_object*.cpp/h
#define GAME_OBJECT_TESTING_EXPORTS // see: script_game_object4.cpp  functions for object testing (ie. is_stalker(), is_heli())
#define NAMESPACE_LEVEL_EXPORTS // see: level_script.cpp

/* Visual */
#define DETAIL_RADIUS // detail draw radius (by K.D.)
#define GRASS_SHADOWS // enable grass shadows (by K.D.)
#define ECO_RENDER // limit FPS in menu to prevent video card overheat (by alpet)

/* Tweaks: */
//#define FP_DEATH // first person death view
#define DEAD_BODY_COLLISION // restore collision with dead bodies (thanks malandrinus)
#define NEW_ANIMS // use new animations. Please enclose any new animation additions with this define
#define CONFIG_SUN_MOVEMENT // With this defined sun will move as configured in weather ltx files

/* Sound: */
#define NEW_SOUNDS // use new sounds. Please enclose any new sound additions with this define
#define LAYERED_SND_SHOOT// see comment down below

// Новые define 							- S.T.A.L.K.E.R. - Call of Chernobyl Back to the roots
#define UPDATEINVHANDS 						// Убрать руки при открытом инвентаре
#define SHOWMAPINVCOP 						// Убрать мини карту при открытом инвентаре
#define AMBIENTSFILELOAD 					// Отключил загрузку отдельного файла амбиента для каждой локации -> ...level_name.c_str(), ".ltx");
#define PSYHEALTH 							// Пси-здоровье, убить ГГ при низком уровне psy_health
#define NEWIND 								// Новые индикаторы, восстановление старых
#define SCREENSPACE_OFF 					// Лучи проходят через холм, если на них смотреть в сторону солнца
#define RAINDROPS 							// Капли на экран из XRay Oxygen, общие функции + R2
#define RAINDROPS_R3 						// Капли на экран R3
#define RAINDROPS_R4 						// Капли на экран R4
#define FIX_FLASHING_POINTS_LAMPS			// Исправление мигания света ламп, XRay Oxygen
#define _STD_EXTENSIONS_SATURATE__LERP 		// XRay Oxygen, Адаптация новых функций для _std_extensions.h OpenXRay
//#define AMMO_FROM_BELT					// Патроны берем только с пояса
#define POLTER_DEATH_SOC					// Смерть полтера как в ТЧ
#define POLTER_FIX_DEATH					// Полтер не кидается предметами после смерти
#define POLTER_POLZUN						// Потеря энергии у полтергейста
#define FLASHLIGHT						    // Сломанный фонарик 
#define SET_MONSTER_RELATION				// Сет отношений мутантов со сталкерами
#define ARTEFACT_SPAWN_ANOMALUS_ZONE		// Спавним артефакт при срабатывании аномалии( ТЧ )
#define ANOMALIUS_ZONE_SOC					// Перенес код CustomZone и Mincer с ТЧ, при срабатывании зоны вызов функции: self::SpawnArtefact()
#define ENGINE_THIRST						// Движковая жажда
#define ENGINE_SLEEP						// Движковый сон
#define FIX_ACTORCONDITION					// Исправление проверки состояния гг в скриптах, Oxygen
#define OLD_TYPEDEF							// Старые типы u16\32, s16\32 для адаптации х32 кода


// Планы: 
//#define LOST_ALPHA_HUD_IND				// Движковые индикаторы на худ как в Лост Альфе
//#define AURA_CONTROLLER_FIX				// Фикс звука ауры контролера при смерти гг, при перезагрузки звук продолжал играть
//#define VOLUMETRIK_TORCH					// Объемный свет фонаря
//#define COLLISION_ACTIVE_ITEM				// Включить коллизию оружия
//#define NEW_ZOMBIE						// НПС не видят зомби пока активна фейк смерть

//Call of Chernobyl OpenXRay
#ifdef COC_EDITION
#define CALLOFCHERNOBYL_RANKING 			// Enables Call of Chernobyl ranking system instead of original Call of Pripyat system
#define COC_USER_SPOT 						// Enable user spot on the map from CoC
#define COC_MAP_SPOT_PROPS_BOX
#define COC_ENABLE_AUTOSAVE 				// Enable autosave manager
#define COC_KICK
#define COC_LOADSCREEN
#define COC_BACKPACK
#define COC_SLOTS
#define COC_DISABLE_ANOMALY_AND_ITEMS_PLANNER// need editing xr_* scripts!
#define COC_SPRINT_FIX
#define COC_DEBUG_BEHAVIOUR 				// Enables CoC debug behaviour when the game crashes
#define ACTOR_BEFORE_DEATH_CALLBACK 		// For extending the life of the actor to fake death or do other tasks that need to happen before actor is dead
#define R2_SHADERS_BACKWARDS_COMPATIBILITY
#endif

#ifdef COC_DEBUG
//#define NON_FATAL_VERIFY
#endif

/* LAYERED_SND_SHOOT by Alundaio
When defined, it will allow you to play a group of sounds from a specified section for snd_shoot.
You can have as many layers as you want, but you must follow naming convention,
snd_1_layer
snd_2_layer
snd_3_layer
...
You can also have different variants for each layer defined,
snd_1_layer
snd_1_layer1
snd_1_layer2
...
The correct line settings are standard, (ie. snd_1_layer = sound_path, volume, delay)
ex.
Here is an example usage:
snd_shoot = new_snd_section
[new_snd_section]
snd_1_layer = weapons\new_sound_shoot1
snd_1_layer1 = weapons\new_sound_shoot2
snd_1_layer2 = weapons\new_sound_shoot3
snd_1_layer3 = weapons\new_sound_shoot4
snd_2_layer = weapons\mechanical_noise, 1.0, 0.1
snd_3_layer = weapons\gunshot_echo, 1.0, 0.8
*/
//-TWEAKS
