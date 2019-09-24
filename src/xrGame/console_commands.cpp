#include "pch_script.h"
#include "xrEngine/xr_ioconsole.h"
#include "xrEngine/xr_ioc_cmd.h"
#include "xrEngine/customhud.h"
#include "xrEngine/fdemorecord.h"
#include "xrEngine/fdemoplay.h"
#include "xrMessages.h"
#include "xrserver.h"
#include "Level.h"
#include "xrScriptEngine/script_debugger.hpp"
#include "ai_debug.h"
#include "alife_simulator.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "actor.h"
#include "Actor_Flags.h"
#include "customzone.h"
#include "xrScriptEngine/script_engine.hpp"
#include "xrScriptEngine/script_process.hpp"
#include "xrServer_Objects.h"
#include "ui/UIMainIngameWnd.h"
#include "xrPhysics/iphworld.h"
#include "string_table.h"
#include "autosave_manager.h"
#include "ai_space.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "date_time.h"
#include "mt_config.h"
#include "UIGameSP.h"
#include "ui/UIActorMenu.h"
#include "ui/UIStatic.h"
#include "zone_effector.h"
#include "GameTask.h"
#include "MainMenu.h"
#include "saved_game_wrapper.h"
#include "xrAICore/Navigation/level_graph.h"
#include "xrNetServer/NET_Messages.h"
#include "uizonemap.h"
#include "cameralook.h"
#include "character_hit_animations_params.h"
#include "inventory_upgrade_manager.h"

#include "ai_debug_variables.h"
#include "xrPhysics/console_vars.h"

string_path g_last_saved_game;

extern u64 g_qwStartGameTime;
extern u64 g_qwEStartGameTime;

ENGINE_API
extern float psHUD_FOV_def;
extern float psSqueezeVelocity;
extern int psLUA_GCSTEP;
extern Fvector m_hud_offset_pos;
extern Fvector m_hand_offset_pos;

float minimap_zoom_factor = 1.0f;

extern int x_m_x;
extern int x_m_z;
extern BOOL net_cl_inputguaranteed;
extern BOOL net_sv_control_hit;
extern int g_dwInputUpdateDelta;
extern BOOL g_bShowHitSectors;
extern ESingleGameDifficulty g_SingleGameDifficulty;
extern BOOL g_show_wnd_rect2;
//-----------------------------------------------------------
extern float g_fTimeFactor;
extern BOOL b_toggle_weapon_aim;

extern u32 UIStyleID;
extern xr_vector<xr_token> UIStyleToken;

extern float g_smart_cover_factor;
extern int g_upgrades_log;
extern float g_smart_cover_animation_speed_factor;

extern BOOL g_ai_use_old_vision;
float g_aim_predict_time = 0.40f;
int g_keypress_on_start = 1;
int g_sprint_reload_wpn = 0;
int g_hand_hide_inventory = 0;

ENGINE_API extern float g_console_sensitive;

// Alundaio
extern BOOL g_ai_die_in_anomaly;
int g_inv_highlight_equipped = 0;
//-Alundaio

int __type_hud_lost_alpha = 0;
int __type_hud_veter_vremeni = 0;
int __type_hud_soc = 0;
int __type_hud_coc = 1;
int __type_hud_cop = 0;

#ifdef Call_of_Chernobyl_OXR
extern u32 gLanguage;
extern xr_vector<xr_token> gLanguagesToken;
#endif

//-----------------------------------------------------------

BOOL g_bCheckTime = FALSE;
int net_cl_inputupdaterate = 50;
Flags32 g_mt_config = {mtLevelPath | mtDetailPath | mtObjectHandler | mtSoundPlayer | mtAiVision | mtBullets |
    mtLUA_GC | mtLevelSounds | mtALife | mtMap};

int g_AI_inactive_time = 0;
Flags32 g_uCommonFlags;
enum E_COMMON_FLAGS
{
    flAiUseTorchDynamicLights = 1
};

static void full_memory_stats()
{
    Memory.mem_compact();
    u32 m_base = 0, c_base = 0, m_lmaps = 0, c_lmaps = 0;
    GEnv.Render->ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
    log_vminfo();
    size_t _process_heap = ::Memory.mem_usage();
    int _eco_strings = (int)g_pStringContainer->stat_economy();
    int _eco_smem = (int)g_pSharedMemoryContainer->stat_economy();
    Msg("~ [ D3D ]: textures[%d K]", (m_base + m_lmaps) / 1024);
    Msg("~ [OpenXRay]: process heap[%u K]", _process_heap / 1024);
    Msg("~ [OpenXRay]: economy: strings[%d K], smem[%d K]", _eco_strings / 1024, _eco_smem);

    Log("--------------------------------------------------------------------------------");

    xrMemory::SProcessMemInfo memCounters;
    ::Memory.GetProcessMemInfo(memCounters);
    Msg("~ %I64dMB physical memory installed, %I64dMB available, %ld percent of memory in use",
        memCounters.TotalPhysicalMemory / (1024 * 1024), memCounters.FreePhysicalMemory / (1024 * 1024),
        memCounters.MemoryLoad);

    Msg("~ PageFile usage: %I64dMB, Peak PageFile usage: %I64dMB,", memCounters.PagefileUsage / (1024 * 1024),
        memCounters.PeakPagefileUsage / (1024 * 1024));

    Log("--------------------------------------------------------------------------------");
    Log("# Build engine for <OpenXRay Call of Chernobyl DEV 885>");
    Log("--------------------------------------------------------------------------------");
}

class CCC_MemStats : public IConsole_Command
{
public:
    CCC_MemStats(LPCSTR N) : IConsole_Command(N)
    {
        bEmptyArgsHandled = TRUE;
        xrDebug::SetOutOfMemoryCallback(full_memory_stats);
    };
    virtual void Execute(LPCSTR args) { full_memory_stats(); }
};

// console commands
class CCC_GameDifficulty : public CCC_Token
{
public:
    CCC_GameDifficulty(LPCSTR N) : CCC_Token(N, (u32*)&g_SingleGameDifficulty, difficulty_type_token){};
    virtual void Execute(LPCSTR args)
    {
        CCC_Token::Execute(args);
        if (g_pGameLevel && Level().game)
        {
            game_cl_Single* game = smart_cast<game_cl_Single*>(Level().game);
            VERIFY(game);
            game->OnDifficultyChanged();
        }
    }
    virtual void Info(TInfo& I) { xr_strcpy(I, "game difficulty"); }
};

#ifdef Call_of_Chernobyl_OXR
class CCC_GameLanguage : public CCC_Token
{
public:
    CCC_GameLanguage(LPCSTR N) : CCC_Token(N, (u32*)&gLanguage, NULL)
    {
        CStringTable();
        tokens = gLanguagesToken.data();
    };

    virtual void Execute(LPCSTR args)
    {
        tokens = gLanguagesToken.data();

        CCC_Token::Execute(args);
        CStringTable().ReloadLanguage();
    }
};
#endif

class CCC_ALifeTimeFactor : public IConsole_Command
{
public:
    CCC_ALifeTimeFactor(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        float id1 = 0.0f;
        sscanf(args, "%f", &id1);
        if (id1 < EPS_L)
            Msg("Invalid time factor! (%.4f)", id1);
        else
        {
            if (!OnServer())
                return;

            Level().SetGameTimeFactor(id1);
        }
    }

    virtual void Save(IWriter* F){};
    virtual void Status(TStatus& S)
    {
        if (!g_pGameLevel)
            return;

        float v = Level().GetGameTimeFactor();
        xr_sprintf(S, sizeof(S), "%3.5f", v);
        while (xr_strlen(S) && ('0' == S[xr_strlen(S) - 1]))
            S[xr_strlen(S) - 1] = 0;
    }
    virtual void Info(TInfo& I)
    {
        if (!OnServer())
            return;
        float v = Level().GetGameTimeFactor();
        xr_sprintf(I, sizeof(I), " value = %3.5f", v);
    }
    virtual void fill_tips(vecTips& tips, u32 mode)
    {
        if (!OnServer())
            return;
        float v = Level().GetGameTimeFactor();

        TStatus str;
        xr_sprintf(str, sizeof(str), "%3.5f  (current)  [0.0,1000.0]", v);
        tips.push_back(str);
        IConsole_Command::fill_tips(tips, mode);
    }
};

class CCC_ALifeSwitchDistance : public IConsole_Command
{
public:
    CCC_ALifeSwitchDistance(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if ((GameID() == eGameIDSingle) && ai().get_alife())
        {
            float id1 = 0.0f;
            sscanf(args, "%f", &id1);
            if (id1 < 2.0f)
                Msg("Invalid online distance! (%.4f)", id1);
            else
            {
                NET_Packet P;
                P.w_begin(M_SWITCH_DISTANCE);
                P.w_float(id1);
                Level().Send(P, net_flags(TRUE, TRUE));
            }
        }
        else
            Log("!Not a single player game!");
    }
};

class CCC_ALifeProcessTime : public IConsole_Command
{
public:
    CCC_ALifeProcessTime(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if ((GameID() == eGameIDSingle) && ai().get_alife())
        {
            game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level().Server->GetGameState());
            VERIFY(tpGame);
            int id1 = 0;
            sscanf(args, "%d", &id1);
            if (id1 < 1)
                Msg("Invalid process time! (%d)", id1);
            else
                tpGame->alife().set_process_time(id1);
        }
        else
            Log("!Not a single player game!");
    }
};

class CCC_ALifeObjectsPerUpdate : public IConsole_Command
{
public:
    CCC_ALifeObjectsPerUpdate(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if ((GameID() == eGameIDSingle) && ai().get_alife())
        {
            game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level().Server->GetGameState());
            VERIFY(tpGame);
            int id1 = 0;
            sscanf(args, "%d", &id1);
            tpGame->alife().objects_per_update(id1);
        }
        else
            Log("!Not a single player game!");
    }
};

class CCC_ALifeSwitchFactor : public IConsole_Command
{
public:
    CCC_ALifeSwitchFactor(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if ((GameID() == eGameIDSingle) && ai().get_alife())
        {
            game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level().Server->GetGameState());
            VERIFY(tpGame);
            float id1 = 0;
            sscanf(args, "%f", &id1);
            clamp(id1, .1f, 1.f);
            tpGame->alife().set_switch_factor(id1);
        }
        else
            Log("!Not a single player game!");
    }
};

//-----------------------------------------------------------------------
class CCC_DemoRecord : public IConsole_Command
{
public:
    CCC_DemoRecord(LPCSTR N) : IConsole_Command(N) {}
    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel) // level not loaded
        {
            Log("Demo Record is disabled when level is not loaded.");
            return;
        }

        Console->Hide();

        // close main menu if it is open
        if (MainMenu()->IsActive())
            MainMenu()->Activate(false);

        LPSTR fn_;
        STRCONCAT(fn_, args, ".xrdemo");
        string_path fn;
        FS.update_path(fn, "$game_saves$", fn_);

        g_pGameLevel->Cameras().AddCamEffector(new CDemoRecord(fn));
    }
};

class CCC_DemoRecordSetPos : public CCC_Vector3
{
    static Fvector p;

public:
    CCC_DemoRecordSetPos(LPCSTR N)
        : CCC_Vector3(N, &p, Fvector().set(-FLT_MAX, -FLT_MAX, -FLT_MAX), Fvector().set(FLT_MAX, FLT_MAX, FLT_MAX)){};
    virtual void Execute(LPCSTR args)
    {
        CDemoRecord::GetGlobalPosition(p);
        CCC_Vector3::Execute(args);
        CDemoRecord::SetGlobalPosition(p);
    }
    virtual void Save(IWriter* F) { ; }
};
Fvector CCC_DemoRecordSetPos::p = {0, 0, 0};

class CCC_DemoPlay : public IConsole_Command
{
public:
    CCC_DemoPlay(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        if (0 == g_pGameLevel)
        {
            Msg("! There are no level(s) started");
        }
        else
        {
            Console->Hide();
            string_path fn;
            u32 loops = 0;
            LPSTR comma = strchr(const_cast<LPSTR>(args), ',');
            if (comma)
            {
                loops = atoi(comma + 1);
                *comma = 0; //. :)
            }
            strconcat(sizeof(fn), fn, args, ".xrdemo");
            FS.update_path(fn, "$game_saves$", fn);
            g_pGameLevel->Cameras().AddCamEffector(new CDemoPlay(fn, 1.0f, loops));
        }
    }
};

// helper functions --------------------------------------------

bool valid_saved_game_name(LPCSTR file_name)
{
    LPCSTR I = file_name;
    LPCSTR E = file_name + xr_strlen(file_name);
    for (; I != E; ++I)
    {
        if (!strchr("/\\:*?\"<>|^()[]%", *I))
            continue;

        return (false);
    };

    return (true);
}

void get_files_list(xr_vector<shared_str>& files, LPCSTR dir, LPCSTR file_ext)
{
    VERIFY(dir && file_ext);
    files.clear();

    FS_Path* P = FS.get_path(dir);
    P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
    FS.m_Flags.set(CLocatorAPI::flNeedCheck, TRUE);
    FS.rescan_pathes();

    LPCSTR fext;
    STRCONCAT(fext, "*", file_ext);

    FS_FileSet files_set;
    FS.file_list(files_set, dir, FS_ListFiles, fext);
    u32 len_str_ext = xr_strlen(file_ext);

    auto itb = files_set.begin();
    auto ite = files_set.end();

    for (; itb != ite; ++itb)
    {
        LPCSTR fn_ext = (*itb).name.c_str();
        VERIFY(xr_strlen(fn_ext) > len_str_ext);
        string_path fn;
        strncpy_s(fn, sizeof(fn), fn_ext, xr_strlen(fn_ext) - len_str_ext);
        files.push_back(fn);
    }
    FS.m_Flags.set(CLocatorAPI::flNeedCheck, FALSE);
}

#include "UIGameCustom.h"

class CCC_ALifeSave : public IConsole_Command
{
public:
    CCC_ALifeSave(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR args)
    {
        if (!g_actor || !Actor()->g_Alive())
        {
            Msg("cannot make saved game because actor is dead :(");
            return;
        }

        Console->Execute("stat_memory");

        string_path S, S1;
        S[0] = 0;
        strncpy_s(S, sizeof(S), args, _MAX_PATH - 1);

        if (!xr_strlen(S))
        {
            strconcat(sizeof(S), S, Core.UserName, " - ", "quicksave");
            NET_Packet net_packet;
            net_packet.w_begin(M_SAVE_GAME);
            net_packet.w_stringZ(S);
            net_packet.w_u8(0);
            Level().Send(net_packet, net_flags(TRUE));
        }
        else
        {
            if (!valid_saved_game_name(S))
            {
                Msg("! Save failed: invalid file name - %s", S);
                return;
            }

            NET_Packet net_packet;
            net_packet.w_begin(M_SAVE_GAME);
            net_packet.w_stringZ(S);
            net_packet.w_u8(1);
            Level().Send(net_packet, net_flags(TRUE));
        }

        StaticDrawableWrapper* _s = CurrentGameUI()->AddCustomStatic("game_saved", true);
        LPSTR save_name;
        STRCONCAT(save_name, CStringTable().translate("st_game_saved").c_str(), ": ", S);
        _s->wnd()->TextItemControl()->SetText(save_name);

        xr_strcat(S, ".dds");
        FS.update_path(S1, "$game_saves$", S);

        MainMenu()->Screenshot(IRender::SM_FOR_GAMESAVE, S1);

    } // virtual void Execute

    virtual void fill_tips(vecTips& tips, u32 mode) { get_files_list(tips, "$game_saves$", SAVE_EXTENSION); }
}; // CCC_ALifeSave

class CCC_ALifeLoadFrom : public IConsole_Command
{
public:
    CCC_ALifeLoadFrom(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR args)
    {
        string_path saved_game;
        strncpy_s(saved_game, sizeof(saved_game), args, _MAX_PATH - 1);

        if (!ai().get_alife())
        {
            Log("! ALife simulator has not been started yet");
            return;
        }

        if (!xr_strlen(saved_game))
        {
            Log("! Specify file name!");
            return;
        }

        if (!CSavedGameWrapper::saved_game_exist(saved_game))
        {
            Msg("! Cannot find saved game %s", saved_game);
            return;
        }

        if (!CSavedGameWrapper::valid_saved_game(saved_game))
        {
            Msg("! Cannot load saved game %s, version mismatch or saved game is corrupted", saved_game);
            return;
        }

        if (!valid_saved_game_name(saved_game))
        {
            Msg("! Cannot load saved game %s, invalid file name", saved_game);
            return;
        }

        if (MainMenu()->IsActive())
            MainMenu()->Activate(false);

        Console->Execute("stat_memory");

        if (Device.Paused())
            Device.Pause(FALSE, TRUE, TRUE, "CCC_ALifeLoadFrom");

        NET_Packet net_packet;
        net_packet.w_begin(M_LOAD_GAME);
        net_packet.w_stringZ(saved_game);
        Level().Send(net_packet, net_flags(TRUE));
    }

    virtual void fill_tips(vecTips& tips, u32 mode) { get_files_list(tips, "$game_saves$", SAVE_EXTENSION); }
}; // CCC_ALifeLoadFrom

class CCC_LoadLastSave : public IConsole_Command
{
public:
    CCC_LoadLastSave(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; }
    virtual void Execute(LPCSTR args)
    {
        string_path saved_game = "";
        if (args)
        {
            strncpy_s(saved_game, sizeof(saved_game), args, _MAX_PATH - 1);
        }

        if (saved_game && *saved_game)
        {
            xr_strcpy(g_last_saved_game, saved_game);
            return;
        }

        if (!*g_last_saved_game)
        {
            Msg("! cannot load last saved game since it hasn't been specified");
            return;
        }

        if (!CSavedGameWrapper::saved_game_exist(g_last_saved_game))
        {
            Msg("! Cannot find saved game %s", g_last_saved_game);
            return;
        }

        if (!CSavedGameWrapper::valid_saved_game(g_last_saved_game))
        {
            Msg("! Cannot load saved game %s, version mismatch or saved game is corrupted", g_last_saved_game);
            return;
        }

        if (!valid_saved_game_name(g_last_saved_game))
        {
            Msg("! Cannot load saved game %s, invalid file name", g_last_saved_game);
            return;
        }

        LPSTR command;
        if (ai().get_alife())
        {
            STRCONCAT(command, "load ", g_last_saved_game);
            Console->Execute(command);
            return;
        }

        STRCONCAT(command, "start server(", g_last_saved_game, "/single/alife/load)");
        Console->Execute(command);
    }

    virtual void Save(IWriter* F)
    {
        if (!*g_last_saved_game)
            return;

        F->w_printf("%s %s\r\n", cName, g_last_saved_game);
    }
};

class CCC_FlushLog : public IConsole_Command
{
public:
    CCC_FlushLog(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR /**args/**/)
    {
        FlushLog();
        Msg("* Log file has been saved successfully!");
    }
};

class CCC_ClearLog : public IConsole_Command
{
public:
    CCC_ClearLog(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR)
    {
        LogFile->clear();
        FlushLog();
        Msg("* Log file has been cleaned successfully!");
    }
};

class CCC_Spawn_to_inventory : public IConsole_Command
{
public:
    CCC_Spawn_to_inventory(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel)
        {
            Log("Error: No game level!");
            return;
        }

        if (!pSettings->section_exist(args))
        {
            Msg("! Section [%s] isn`t exist...", args);
            return;
        }

        char Name[128];
        Name[0] = 0;
        sscanf(args, "%s", Name);

        Level().spawn_item(Name, Actor()->Position(), false, Actor()->ID());
    }
    virtual void Info(TInfo& I) { strcpy(I, "name,team,squad,group"); }
};

class CCC_FloatBlock : public CCC_Float
{
public:
    CCC_FloatBlock(LPCSTR N, float* V, float _min = 0, float _max = 1) : CCC_Float(N, V, _min, _max){};

    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel || GameID() == eGameIDSingle)
            CCC_Float::Execute(args);
        else
        {
            Msg("! Command disabled for this type of game");
        }
    }
};

class CCC_Net_CL_InputUpdateRate : public CCC_Integer
{
protected:
    int* value_blin;

public:
    CCC_Net_CL_InputUpdateRate(LPCSTR N, int* V, int _min = 0, int _max = 999)
        : CCC_Integer(N, V, _min, _max), value_blin(V){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        if ((*value_blin > 0) && g_pGameLevel)
        {
            g_dwInputUpdateDelta = 1000 / (*value_blin);
        };
    }
};

class CCC_PHIterations : public CCC_Integer
{
public:
    CCC_PHIterations(LPCSTR N) : CCC_Integer(N, &phIterations, 15, 50){};
    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        // dWorldSetQuickStepNumIterations(NULL,phIterations);
        if (physics_world())
            physics_world()->StepNumIterations(phIterations);
    }
};

class CCC_PHFps : public IConsole_Command
{
public:
    CCC_PHFps(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        float step_count = (float)atof(args);

        // IPHWorld::SetStep(1.f/step_count);
        ph_console::ph_step_time = 1.f / step_count;
        // physics_world()->SetStep(1.f/step_count);
        if (physics_world())
            physics_world()->SetStep(ph_console::ph_step_time);
    }
    virtual void Status(TStatus& S) { xr_sprintf(S, "%3.5f", 1.f / ph_console::ph_step_time); }
};

#include "xrAICore/Navigation/game_graph.h"
struct CCC_JumpToLevel : public IConsole_Command
{
    CCC_JumpToLevel(LPCSTR N) : IConsole_Command(N){};

    virtual void Execute(LPCSTR level)
    {
        if (!ai().get_alife())
        {
            Msg("! ALife simulator is needed to perform specified command!");
            return;
        }

        GameGraph::LEVEL_MAP::const_iterator I = ai().game_graph().header().levels().begin();
        GameGraph::LEVEL_MAP::const_iterator E = ai().game_graph().header().levels().end();
        for (; I != E; ++I)
            if (!xr_strcmp((*I).second.name(), level))
            {
                ai().alife().jump_to_level(level);
                return;
            }
        Msg("! There is no level \"%s\" in the game graph!", level);
    }

    virtual void Save(IWriter* F){};
    virtual void fill_tips(vecTips& tips, u32 mode)
    {
        if (!ai().get_alife())
        {
            Msg("! ALife simulator is needed to perform specified command!");
            return;
        }

        GameGraph::LEVEL_MAP::const_iterator itb = ai().game_graph().header().levels().begin();
        GameGraph::LEVEL_MAP::const_iterator ite = ai().game_graph().header().levels().end();
        for (; itb != ite; ++itb)
        {
            tips.push_back((*itb).second.name());
        }
    }
};

class CCC_Script : public IConsole_Command
{
public:
    CCC_Script(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = false; };
    virtual void Execute(LPCSTR args)
    {
        if (!xr_strlen(args))
        {
            Log("* Specify script name!");
        }
        else
        {
            // rescan pathes
            FS_Path* P = FS.get_path("$game_scripts$");
            P->m_Flags.set(FS_Path::flNeedRescan, TRUE);
            FS.rescan_pathes();
            // run script
            if (GEnv.ScriptEngine->script_process(ScriptProcessor::Level))
                GEnv.ScriptEngine->script_process(ScriptProcessor::Level)->add_script(args, false, true);
        }
    }

    virtual void Status(TStatus& S) { xr_strcpy(S, "<script_name> (Specify script name!)"); }
    virtual void Save(IWriter* F) {}
    virtual void fill_tips(vecTips& tips, u32 mode) { get_files_list(tips, "$game_scripts$", ".script"); }
};

class CCC_ScriptCommand : public IConsole_Command
{
public:
    CCC_ScriptCommand(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = false; };
    virtual void Execute(LPCSTR args)
    {
        if (!xr_strlen(args))
            Log("* Specify string to run!");
        else
        {
            if (GEnv.ScriptEngine->script_process(ScriptProcessor::Level))
            {
                GEnv.ScriptEngine->script_process(ScriptProcessor::Level)->add_script(args, true, true);
                return;
            }

            string4096 S;
            shared_str m_script_name = "console command";
            xr_sprintf(S, "%s\n", args);
            int l_iErrorCode = luaL_loadbuffer(GEnv.ScriptEngine->lua(), S, xr_strlen(S), "@console_command");
            if (!l_iErrorCode)
            {
                l_iErrorCode = lua_pcall(GEnv.ScriptEngine->lua(), 0, 0, 0);
                if (l_iErrorCode)
                {
                    GEnv.ScriptEngine->print_output(GEnv.ScriptEngine->lua(), *m_script_name, l_iErrorCode);
                    GEnv.ScriptEngine->on_error(GEnv.ScriptEngine->lua());
                    return;
                }
            }

            GEnv.ScriptEngine->print_output(GEnv.ScriptEngine->lua(), *m_script_name, l_iErrorCode);
        }
    } // void	Execute

    virtual void Status(TStatus& S) { xr_strcpy(S, "<script_name.function()> (Specify script and function name!)"); }
    virtual void Save(IWriter* F) {}
    virtual void fill_tips(vecTips& tips, u32 mode)
    {
        if (mode == 1)
        {
            get_files_list(tips, "$game_scripts$", ".script");
            return;
        }

        IConsole_Command::fill_tips(tips, mode);
    }
};

class CCC_Spawn : public IConsole_Command
{
public:
    CCC_Spawn(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel)
            return;

        //#ifndef	DEBUG
        if (GameID() != eGameIDSingle)
        {
            Msg("For this game type entity-spawning is disabled.");
            return;
        };
        //#endif

        if (!pSettings->section_exist(args))
        {
            Msg("! Section [%s] isn`t exist...", args);
            return;
        }

        char Name[128];
        Name[0] = 0;
        sscanf(args, "%s", Name);
        Fvector pos = Actor()->Position();
        pos.y += 3.0f;
        Level().g_cl_Spawn(Name, 0xff, M_SPAWN_OBJECT_LOCAL, pos);
    }
    virtual void Info(TInfo& I) { strcpy(I, "name,team,squad,group"); }
};

class CCC_TimeFactor : public IConsole_Command
{
public:
    CCC_TimeFactor(LPCSTR N) : IConsole_Command(N) {}
    virtual void Execute(LPCSTR args)
    {
        float time_factor = (float)atof(args);
        clamp(time_factor, EPS, 1000.f);
        Device.time_factor(time_factor);
        psSpeedOfSound = time_factor;
    }
    virtual void Status(TStatus& S) { xr_sprintf(S, sizeof(S), "%f", Device.time_factor()); }
    virtual void Info(TInfo& I) { xr_strcpy(I, "[0.001 - 1000.0]"); }
    virtual void fill_tips(vecTips& tips, u32 mode)
    {
        TStatus str;
        xr_sprintf(str, sizeof(str), "%3.3f  (current)  [0.001 - 1000.0]", Device.time_factor());
        tips.push_back(str);
        IConsole_Command::fill_tips(tips, mode);
    }
};

#include "GamePersistent.h"

class CCC_MainMenu : public IConsole_Command
{
public:
    CCC_MainMenu(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR args)
    {
        bool bWhatToDo = TRUE;
        if (0 == xr_strlen(args))
        {
            bWhatToDo = !MainMenu()->IsActive();
        };

        if (EQ(args, "on") || EQ(args, "1"))
            bWhatToDo = TRUE;

        if (EQ(args, "off") || EQ(args, "0"))
            bWhatToDo = FALSE;

        MainMenu()->Activate(bWhatToDo);
    }
};

struct CCC_StartTimeSingle : public IConsole_Command
{
    CCC_StartTimeSingle(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
        sscanf(args, "%d.%d.%d %d:%d:%d.%d", &year, &month, &day, &hours, &mins, &secs, &milisecs);
        year = _max(year, 1);
        month = _max(month, 1);
        day = _max(day, 1);
        g_qwStartGameTime = generate_time(year, month, day, hours, mins, secs, milisecs);

        if (!g_pGameLevel)
            return;

        if (!Level().Server)
            return;

        if (!Level().Server->GetGameState())
            return;

        Level().SetGameTimeFactor(g_qwStartGameTime, g_fTimeFactor);
    }

    virtual void Status(TStatus& S)
    {
        u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
        split_time(g_qwStartGameTime, year, month, day, hours, mins, secs, milisecs);
        xr_sprintf(S, "%d.%d.%d %d:%d:%d.%d", year, month, day, hours, mins, secs, milisecs);
    }
};

struct CCC_TimeFactorSingle : public CCC_Float
{
    CCC_TimeFactorSingle(LPCSTR N, float* V, float _min = 0.f, float _max = 1.f) : CCC_Float(N, V, _min, _max){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);

        if (!g_pGameLevel)
            return;

        if (!Level().Server)
            return;

        if (!Level().Server->GetGameState())
            return;

        Level().SetGameTimeFactor(g_fTimeFactor);
    }
};

class CCC_DumpObjects : public IConsole_Command
{
public:
    CCC_DumpObjects(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR) { Level().Objects.dump_all_objects(); }
};

class CCC_Net_SV_GuaranteedPacketMode : public CCC_Integer
{
protected:
    int* value_blin;

public:
    CCC_Net_SV_GuaranteedPacketMode(LPCSTR N, int* V, int _min = 0, int _max = 2)
        : CCC_Integer(N, V, _min, _max), value_blin(V){};

    virtual void Execute(LPCSTR args) { CCC_Integer::Execute(args); }
};

class CCC_InvDropAllItems : public IConsole_Command
{
public:
    CCC_InvDropAllItems(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel)
        {
            return;
        }
        CUIGameSP* ui_game_sp = smart_cast<CUIGameSP*>(CurrentGameUI());
        if (!ui_game_sp)
        {
            return;
        }
        int d = 0;
        sscanf(args, "%d", &d);
        if (ui_game_sp->GetActorMenu().DropAllItemsFromRuck(d == 1))
        {
            Msg("- All items from ruck of Actor is dropping now.");
        }
        else
        {
            Msg("! ActorMenu is not in state `Inventory`");
        }
    }
};

class CCC_DbgVar : public IConsole_Command
{
public:
    CCC_DbgVar(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = false; };
    virtual void Execute(LPCSTR arguments)
    {
        if (!arguments || !*arguments)
        {
            return;
        }

        if (_GetItemCount(arguments, ' ') == 1)
        {
            ai_dbg::show_var(arguments);
        }
        else
        {
            char name[1024];
            float f;
            sscanf(arguments, "%s %f", name, &f);
            ai_dbg::set_var(name, f);
        }
    }
};

// Change weather immediately
class CCC_SetWeather : public IConsole_Command
{
public:
    CCC_SetWeather(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if (!xr_strlen(args))
            return;

        if (!Device.editor())
            g_pGamePersistent->Environment().SetWeather(args, true);
    }
};

// Give money to actor
class CCC_GiveMoney : public IConsole_Command
{
public:
    CCC_GiveMoney(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel)
            return;

        if (!xr_strlen(args))
            return;

        int money = atoi(args);

        if (Actor() && money > 0)
            Actor()->set_money(Actor()->get_money() + money, true);
        else
            Msg("Arg must be greater than 0!");
    }
};

void CCC_RegisterCommands()
{
    // options

    CMD1(CCC_MemStats, "stat_memory");
    // game
    CMD3(CCC_Mask, "g_crouch_toggle", &psActorFlags, AF_CROUCH_TOGGLE);
    CMD3(CCC_Mask, "hud_crosshair_collide", &psActorFlags, AF_CROSSHAIR_COLLIDE);
    CMD3(CCC_Mask, "hud_crosshair_inert", &psActorFlags, AF_CROSSHAIR_INERT);
    CMD3(CCC_Mask, "hud_crosshair_standart", &psActorFlags, AF_CROSSHAIR_STANDART);
    CMD1(CCC_GameDifficulty, "g_game_difficulty");
#ifdef Call_of_Chernobyl_OXR
    CMD1(CCC_GameLanguage, "g_language");
#endif

    CMD3(CCC_Mask, "g_backrun", &psActorFlags, AF_RUN_BACKWARD);

    // alife

    CMD1(CCC_ALifeSave, "save"); // save game
    CMD1(CCC_ALifeLoadFrom, "load"); // load game from ...
    CMD1(CCC_LoadLastSave, "load_last_save"); // load last saved game from ...

    CMD1(CCC_FlushLog, "flush"); // flush log
    CMD1(CCC_ClearLog, "clear_log");

    CMD1(CCC_ALifeTimeFactor, "al_time_factor"); // set time factor
    CMD1(CCC_ALifeSwitchDistance, "al_switch_distance"); // set switch distance
    CMD1(CCC_ALifeProcessTime, "al_process_time"); // set process time
    CMD1(CCC_ALifeObjectsPerUpdate, "al_objects_per_update"); // set process time
    CMD1(CCC_ALifeSwitchFactor, "al_switch_factor"); // set switch factor

    CMD3(CCC_Mask, "hud_weapon", &psHUD_Flags, HUD_WEAPON);
    CMD3(CCC_Mask, "hud_info", &psHUD_Flags, HUD_INFO);
    CMD3(CCC_Mask, "hud_draw", &psHUD_Flags, HUD_DRAW);

    // hud
    psHUD_Flags.set(HUD_CROSSHAIR, true);
    psHUD_Flags.set(HUD_WEAPON, true);
    psHUD_Flags.set(HUD_DRAW, true);
    psHUD_Flags.set(HUD_INFO, true);

    CMD3(CCC_Mask, "hud_crosshair", &psHUD_Flags, HUD_CROSSHAIR);
    CMD3(CCC_Mask, "hud_crosshair_dist", &psHUD_Flags, HUD_CROSSHAIR_DIST);

    CMD4(CCC_Float, "hud_fov", &psHUD_FOV_def, 0.35f, 0.65f);
    CMD4(CCC_Float, "fov", &g_fov, 35.0f, 70.0f);
    CMD4(CCC_Float, "scope_fov", &g_scope_fov, 35.0f, 70.0f);

    // Demo
    CMD1(CCC_DemoPlay, "demo_play");
    CMD1(CCC_DemoRecord, "demo_record");
    CMD1(CCC_DemoRecordSetPos, "demo_set_cam_position");

    // ai
    CMD3(CCC_Mask, "mt_ai_vision", &g_mt_config, mtAiVision);
    CMD3(CCC_Mask, "mt_level_path", &g_mt_config, mtLevelPath);
    CMD3(CCC_Mask, "mt_detail_path", &g_mt_config, mtDetailPath);
    CMD3(CCC_Mask, "mt_object_handler", &g_mt_config, mtObjectHandler);
    CMD3(CCC_Mask, "mt_sound_player", &g_mt_config, mtSoundPlayer);
    CMD3(CCC_Mask, "mt_bullets", &g_mt_config, mtBullets);
    CMD3(CCC_Mask, "mt_script_gc", &g_mt_config, mtLUA_GC);
    CMD3(CCC_Mask, "mt_level_sounds", &g_mt_config, mtLevelSounds);
    CMD3(CCC_Mask, "mt_alife", &g_mt_config, mtALife);
    CMD3(CCC_Mask, "mt_map", &g_mt_config, mtMap);

    CMD4(CCC_Float, "ai_smart_factor", &g_smart_cover_factor, 0.f, 1000000.f);
    CMD3(CCC_Mask, "lua_debug", &g_LuaDebug, 1);

    // Physics
    CMD1(CCC_PHFps, "ph_frequency");
    CMD1(CCC_PHIterations, "ph_iterations");

    CMD1(CCC_JumpToLevel, "jump_to_level");

    CMD3(CCC_Mask, "g_god", &psActorFlags, AF_GODMODE);
    CMD3(CCC_Mask, "g_unlimitedammo", &psActorFlags, AF_UNLIMITEDAMMO);

    if (!psDeviceFlags.test(rsR1))
    {
        CMD3(CCC_Mask, "g_3d_scopes", &psActorFlags, AF_3DSCOPE_ENABLE);
        CMD3(CCC_Mask, "g_pnv_in_scope", &psActorFlags, AF_PNV_W_SCOPE_DIS);
    }

    CMD1(CCC_TimeFactor, "time_factor");
    CMD1(CCC_Spawn, "g_spawn");
    CMD1(CCC_Spawn_to_inventory, "g_spawn_to_inventory");
    CMD1(CCC_Script, "run_script");
    CMD1(CCC_ScriptCommand, "run_string");
    CMD3(CCC_Mask, "g_no_clip", &psActorFlags, AF_NO_CLIP);
    CMD1(CCC_SetWeather, "set_weather");
    CMD1(CCC_GiveMoney, "give_money");

    CMD4(CCC_Integer, "__type_hud_lost_alpha", &__type_hud_lost_alpha, 0, 1); // Лост Альфа
    CMD4(CCC_Integer, "__type_hud_veter_vremeni", &__type_hud_veter_vremeni, 0, 1); // Ветер Времени 1.3
    CMD4(CCC_Integer, "__type_hud_shadow_of_chernobyl", &__type_hud_soc, 0, 1); // Тень Чернобыля
    CMD4(CCC_Integer, "__type_hud_call_of_chernobyl", &__type_hud_coc, 0, 1); // Зов Чернобыля 1.4.22
    CMD4(CCC_Integer, "__type_hud_call_of_pripyat", &__type_hud_cop, 0, 1); // Зов Припяти

    CMD3(CCC_Mask, "g_autopickup", &psActorFlags, AF_AUTOPICKUP);
    CMD3(CCC_Mask, "g_dynamic_music", &psActorFlags, AF_DYNAMIC_MUSIC);
    CMD3(CCC_Mask, "g_important_save", &psActorFlags, AF_IMPORTANT_SAVE);
    CMD4(CCC_Integer, "g_inv_highlight_equipped", &g_inv_highlight_equipped, 0, 1);

    CMD4(CCC_Float, "rs_minimap_zoom_factor", &minimap_zoom_factor, 0.5, 3.5);

    CMD3(CCC_Mask, "cl_dynamiccrosshair", &psHUD_Flags, HUD_CROSSHAIR_DYNAMIC);
    CMD1(CCC_MainMenu, "main_menu");

    CMD1(CCC_StartTimeSingle, "start_time_single");
    CMD4(CCC_TimeFactorSingle, "time_factor_single", &g_fTimeFactor, 0.f, 1000.0f);

    g_uCommonFlags.zero();
    g_uCommonFlags.set(flAiUseTorchDynamicLights, TRUE);

    CMD3(CCC_Mask, "ai_use_torch_dynamic_lights", &g_uCommonFlags, flAiUseTorchDynamicLights);

    CMD1(CCC_InvDropAllItems, "inv_drop_all_items"); // Выкинуть все вещи из инвентаря

    CMD4(CCC_Float, "con_sensitive", &g_console_sensitive, 0.01f, 1.0f);
    CMD4(CCC_Integer, "wpn_aim_toggle", &b_toggle_weapon_aim, 0, 1);
    CMD4(CCC_Integer, "inv_upgrades_log", &g_upgrades_log, 0, 1);

    CMD3(CCC_Token, "ui_style", &UIStyleID, UIStyleToken.data());

    CMD4(CCC_Integer, "g_sleep_time", &psActorSleepTime, 1, 24);

    CMD4(CCC_Integer, "ai_use_old_vision", &g_ai_use_old_vision, 0, 1);

    CMD4(CCC_Integer, "ai_die_in_anomaly", &g_ai_die_in_anomaly, 0, 1); // Alundaio

    CMD4(CCC_Float, "ai_aim_predict_time", &g_aim_predict_time, 0.f, 10.f);

    // Alundaio: Scoped outside DEBUG
    extern BOOL g_ai_aim_use_smooth_aim;
    CMD4(CCC_Integer, "ai_aim_use_smooth_aim", &g_ai_aim_use_smooth_aim, 0, 1);

    extern float g_ai_aim_min_speed;
    CMD4(CCC_Float, "ai_aim_min_speed", &g_ai_aim_min_speed, 0.f, 10.f * PI);

    extern float g_ai_aim_min_angle;
    CMD4(CCC_Float, "ai_aim_min_angle", &g_ai_aim_min_angle, 0.f, 10.f * PI);

    extern float g_ai_aim_max_angle;
    CMD4(CCC_Float, "ai_aim_max_angle", &g_ai_aim_max_angle, 0.f, 10.f * PI);

    *g_last_saved_game = 0;

    CMD3(CCC_String, "slot_0", g_quick_use_slots[0], 32);
    CMD3(CCC_String, "slot_1", g_quick_use_slots[1], 32);
    CMD3(CCC_String, "slot_2", g_quick_use_slots[2], 32);
    CMD3(CCC_String, "slot_3", g_quick_use_slots[3], 32);

    CMD4(CCC_Integer, "keypress_on_start", &g_keypress_on_start, 0, 1);

    CMD4(CCC_Integer, "sprint_reload_wpn", &g_sprint_reload_wpn, 0, 1);

    CMD4(CCC_Integer, "hand_hide_inventory", &g_hand_hide_inventory, 0, 1);
}
