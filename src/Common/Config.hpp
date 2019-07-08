#pragma once


//                                                  // - S.T.A.L.K.E.R. - Call of Chernobyl Back to the roots
#define UPDATEINVHANDS 						        // Убрать руки при открытом инвентаре
#define SHOWMAPINVCOP 						        // Убрать мини карту при открытом инвентаре
#define AMBIENTSFILELOAD 					        // Отключил загрузку отдельного файла амбиента для каждой локации -> ...level_name.c_str(), ".ltx");
#define PSYHEALTH 							        // Пси-здоровье, убить ГГ при низком уровне psy_health
#define NEWIND 								        // Новые индикаторы, восстановление старых
#define SCREENSPACE_OFF 					        // Лучи проходят через холм, если на них смотреть в сторону солнца
#define RAINDROPS 							        // Капли на экран из XRay Oxygen, общие функции + R2
#define RAINDROPS_R3 						        // Капли на экран R3, зависимость от дефайна RAINDROPS
#define RAINDROPS_R4 						        // Капли на экран R4, зависимость от дефайна RAINDROPS
#define FIX_FLASHING_POINTS_LAMPS			        // Исправление мигания света ламп, XRay Oxygen
#define _STD_EXTENSIONS_SATURATE__LERP 		        // XRay Oxygen, Адаптация новых функций для _std_extensions.h OpenXRay
#define AMMO_FROM_BELT					            // Патроны берем только с пояса
#define POLTER_DEATH_SOC					        // Смерть полтера как в ТЧ
#define POLTER_FIX_DEATH					        // Полтер не кидается предметами после смерти
#define POLTER_POLZUN						        // Потеря энергии у полтергейста
#define FLASHLIGHT						            // Сломанный фонарик 
#define SET_MONSTER_RELATION				        // Сет отношений мутантов со сталкерами
#define ARTEFACT_SPAWN_ANOMALUS_ZONE		        // Спавним артефакт при срабатывании аномалии( ТЧ )
#define ANOMALIUS_ZONE_SOC					        // Перенес код CustomZone и Mincer с ТЧ, при срабатывании зоны вызов функции: self::SpawnArtefact()
#define ENGINE_THIRST						        // Движковая жажда
#define ENGINE_SLEEP						        // Движковый сон
#define FIX_ACTORCONDITION					        // Исправление проверки состояния гг в скриптах, Oxygen
#define OLD_TYPEDEF							        // Старые типы u16\32, s16\32 для адаптации х32 кода
#define COLLISION_WPN						        // Коллизия оружия, спасибо Daniel Surs
#define EXTENDED_ITEM_CALLBACKS                     // Новые каллбеки - eItemToBelt, eItemToSlot, eItemToRuck. Т.е. те функции, которые были в bind_stalker ТЧ
#define EXTENDED_WEAPON_CALLBACKS                   // Новые функции для скриптов - eOnWeaponZoomIn, eOnWeaponZoomOut, eOnWeaponJammed
#define INPUT_CALLBACKS                             // Новые функции для биндов клавиш - eKeyPress, eKeyRelease, eKeyHold, eMouseMove, eMouseWheel)
#define ENGINE_LUA_ALIFE_STORAGE_MANAGER_CALLBACKS  // Новые функции для LUA - alife_storage_manager.script  (alife_storage_manager.CALifeStorageManager_save) and (alife_storage_manager.CALifeStorageManager_load)
#define ENGINE_LUA_ALIFE_UPDAGE_MANAGER_CALLBACKS   // Новые функции для LUA - on_before_change_level and on_after_new_game in _G.script when enabled
#define MORE_INVENTORY_SLOTS                        // Больше 5 слотов, система как в тч - слот 1 - .xml slot 1
#define GAME_OBJECT_EXTENDED_EXPORTS                // Смотри: script_game_object*.cpp/h
#define GAME_OBJECT_TESTING_EXPORTS                 // Смотри: script_game_object4.cpp  functions for object testing (ie. is_stalker(), is_heli())
#define NAMESPACE_LEVEL_EXPORTS                     // Смотри: level_script.cpp, новые функции
#define DETAIL_RADIUS                               // Правильный радиус травы (by K.D.)
#define GRASS_SHADOWS                               // Тени от травы, без зависаний (by K.D.)
#define ECO_RENDER                                  // Ограничение кадров в секунду (by alpet)
//#define FP_DEATH                                  // Смерть от первого лица
#define DEAD_BODY_COLLISION                         // Коллизия мертвых тел спасибо malandrinus
#define NEW_ANIMS                                   // Использование новых анимаций (?)
#define CONFIG_SUN_MOVEMENT                         // Движение солнца по конфигам погоды в .ltx
#define NEW_SOUNDS                                  // Новые звуки для оружие Р1
#define LAYERED_SND_SHOOT                           // Новые звуки для оружие Р2
#define DIST_TO_POINT_CS							// Показать дистанцию до цели на карте 
#define NEW_ZOMBIE						        	// НПС не видят зомби пока активна фейк смерть
#define HIT_SLOWMO									// Опциональный запрет на прыжок при замедлении после укуса
#define DSAJ										// Снижение скорости после прыжка
#define LOST_ALPHA_HUD_IND				        	// Движковые индикаторы на худ как в Лост Альфе
#define RESTORE_EAX									// Восст. объемного звука в помещениях, требует расставление snd_env в sdk на локациях
#define CROW_FIX_POSITION							// Вороны не разлетаются с одной точки при старте игры
//#define __DISCORD_API								// Адаптация движка к дискорду
#define FIX_AURA_CONTROLLER							// Прервать звук ауры контролера при перезагрузке сейва, если гг умирает при ней
#define LOST_ALPHA_INV_IND							// Движковые колбы в инвентаре из Лост Альфы
#define FIX_COUNT_ART								// Исправлена ошибка с нулевым m_artefact_spawn_count
#define EXPORT_OLD_MUTANTS							// Обязательное условие для подключения старых классов мутантов
#define CHIMERA_CS									// Химера из ЧН/ТЧ
	
//                                                  // - S.T.A.L.K.E.R. - Call of Chernobyl OpenXRay
#ifdef Call_of_Chernobyl_OXR                        // Общий дефайн ЗЧ, для OpenXRay
#define CALLOFCHERNOBYL_RANKING 			        // Система рангов
#define COC_USER_SPOT 						        // Метки на карту через ПДА,взяло из Лост Альфы Р1
#define COC_MAP_SPOT_PROPS_BOX                      // Метки на карту через ПДА,взяло из Лост Альфы Р2
#define COC_ENABLE_AUTOSAVE 				        // Дистпетчер автосейвов
#define COC_KICK                                    // Пинок вещей
#define COC_LOADSCREEN                              // Что-то с загрузочным экраном (???)
#define COC_BACKPACK                                // Рюкзаки
#define COC_SLOTS                                   // Опять дин. слоты
#define COC_DISABLE_ANOMALY_AND_ITEMS_PLANNER       // Чтобы вещи не летали возле аномалий (???)
#define COC_SPRINT_FIX                              // Фикс бега
#define COC_DEBUG_BEHAVIOUR 				        // Для откладки
#define ACTOR_BEFORE_DEATH_CALLBACK 		        // Интересная функция: Для продления жизни актера - фальшивая смерть или выполнить другие функции, которые должны произойти, прежде чем актер умер
#define R2_SHADERS_BACKWARDS_COMPATIBILITY          // Что-то по графике
#endif


#ifdef Call_of_Chernobyl
//#define NON_FATAL_VERIFY                          // Для откладки
#endif


//Планы;
//#define BLOODSUCKER_CS							// Сосыч из Чистого неба
//#define BLOODSUCKER_SOC							// Сосыч из Теней Чернобыля
//#define BOAR_CS									// Кабан из Чистого неба
//#define BURER_CS									// Бюрер из ЧН/ТЧ
//#define DOG_SOC									// Собака из ЧН/ТЧ
//#define POLTERGEIST_CS							// Полтергейсты из ЧН/ТЧ
//#define SNORK_CS									// Снорк из ЧН/ТЧ
//#define PSEUDO_GIANT_JUMPER						// Прыгающий псевдогигант из ЧН/ТЧ
