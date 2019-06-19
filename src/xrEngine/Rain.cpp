#include "stdafx.h"
#pragma once

#include "Rain.h"
#include "IGame_Persistent.h"
#include "Environment.h"
#include "GameMtlLib.h"
#ifdef _EDITOR
#include "ui_toolscustom.h"
#else
#include "Render.h"
#include "IGame_Level.h"
#include "xrCDB/xr_area.h"
#include "xr_object.h"
#endif

// Warning: duplicated in dxRainRender
static const int max_desired_items = 2500;
static const float source_radius = 12.5f;
static const float source_offset = 40.f;
static const float max_distance = source_offset * 1.25f;
static const float sink_offset = -(max_distance - source_offset);
static const float drop_length = 5.f;
static const float drop_width = 0.30f;
static const float drop_angle = 3.0f;
static const float drop_max_angle = deg2rad(89.f);
static const float drop_max_wind_vel = 100.0f;
static const float drop_speed_min = 40.f;
static const float drop_speed_max = 85.f;

const int max_particles = 1000;
const int particles_cache = 400;
const float particles_time = .3f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef RAINDROPS
CEffect_Rain::CEffect_Rain()
{
    state = stIdle;

    snd_Ambient.create("ambient\\rain", st_Effect, sg_Undefined);

    p_create();

    timerWorld = new RainTimer();
    timerCurrViewEntity = new RainTimer();
}
#else
CEffect_Rain::CEffect_Rain()

{
    state = stIdle;

    snd_Ambient.create("ambient\\rain", st_Effect, sg_Undefined);

    p_create();
}
#endif
CEffect_Rain::~CEffect_Rain()
{
    snd_Ambient.destroy();

    p_destroy();

#ifdef RAINDROPS
    xr_delete(timerWorld);
    xr_delete(timerCurrViewEntity);
#endif
}
#ifdef RAINDROPS
void CEffect_Rain::Load()
{
    //#TODO: RZ to self: load all rain parameters from config, for each rain descriptor separately
}

float CEffect_Rain::GetWorldWetness() const
{
    float rainTimer = timerWorld->rainTimer;
    float lastRainDuration = timerWorld->lastRainDuration;
    float rainDropTime = timerWorld->rainDropTime;

    float res = ((rainTimer - lastRainDuration) / 20.f + _lerp(0.0f, saturate(lastRainDuration / 20.f), saturate(lastRainDuration)));
	return saturate(res);	
}

float CEffect_Rain::GetCurrViewEntityWetness() const
{
    float rainTimer = timerCurrViewEntity->rainTimer;
    float lastRainDuration = timerCurrViewEntity->lastRainDuration;
    float rainDropTime = timerCurrViewEntity->rainDropTime;

    float res = ((rainTimer - lastRainDuration) / rainDropTime + _lerp(0.0f, saturate(lastRainDuration / rainDropTime), saturate(lastRainDuration)));
	return res > 0 ? saturate(res) : 0.f;	
}
#endif

// Born
void CEffect_Rain::Born(Item& dest, float radius)
{
    Fvector axis;
    axis.set(0, -1, 0);
    float gust = g_pGamePersistent->Environment().wind_strength_factor;
    float k = g_pGamePersistent->Environment().CurrentEnv->wind_velocity * gust / drop_max_wind_vel;
    clamp(k, 0.f, 1.f);
    float pitch = drop_max_angle * k - PI_DIV_2;
    axis.setHP(g_pGamePersistent->Environment().CurrentEnv->wind_direction, pitch);

    Fvector& view = Device.vCameraPosition;
    float angle = ::Random.randF(0, PI_MUL_2);
    float dist = ::Random.randF();
    dist = _sqrt(dist) * radius;
    float x = dist * _cos(angle);
    float z = dist * _sin(angle);
    dest.D.random_dir(axis, deg2rad(drop_angle));
    dest.P.set(x + view.x - dest.D.x * source_offset, source_offset + view.y, z + view.z - dest.D.z * source_offset);
    // dest.P.set (x+view.x,height+view.y,z+view.z);
    dest.fSpeed = ::Random.randF(drop_speed_min, drop_speed_max);

    float height = max_distance;
#ifdef RAINDROPS
    u16 targetMaterial = GAMEMTL_NONE_IDX;

    // Check if droplet will hit something when fall down
    bool bHit = RayPick(dest.P, dest.D, height, targetMaterial, collide::rqtBoth);
    RenewItem(dest, height, targetMaterial, bHit);
#else
    RenewItem(dest, height, RayPick(dest.P, dest.D, height, collide::rqtBoth));
#endif
}
#ifdef RAINDROPS
bool CEffect_Rain::RayPick(const Fvector& s, const Fvector& d, float& range, u16& targetMaterial, collide::rq_target tgt)
{
    collide::rq_result RQ;
    IGameObject* E = g_pGameLevel->CurrentViewEntity();
    const bool bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, range, tgt, RQ, E);
    if (bRes)
    {
        range = RQ.range;
        CDB::TRI const& triangle = *(g_pGameLevel->ObjectSpace.GetStaticTris() + RQ.element);
        targetMaterial = triangle.material;
    }

    return bRes;
}
#else
BOOL CEffect_Rain::RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
    BOOL bRes = TRUE;
#ifdef _EDITOR
    Tools->RayPick(s, d, range);
#else
    collide::rq_result RQ;
    IGameObject* E = g_pGameLevel->CurrentViewEntity();
    bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, range, tgt, RQ, E);
    if (bRes)
        range = RQ.range;
#endif
    return bRes;
}
#endif

#ifdef RAINDROPS
void CEffect_Rain::RenewItem(Item& dest, float height, u16 targetMaterial, bool bHit)
{
    dest.uv_set = Random.randI(2);
    if (bHit)
    {
        dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.Phit.mad(dest.P, dest.D, height);
        dest.targetMaterial = targetMaterial;
    }
    else
    {
        dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(2 * 1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.Phit.set(dest.P);
    }
}
#else
void CEffect_Rain::RenewItem(Item& dest, float height, BOOL bHit)
{
    dest.uv_set = Random.randI(2);
    if (bHit)
    {
        dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.Phit.mad(dest.P, dest.D, height);
    }
    else
    {
        dest.dwTime_Life = Device.dwTimeGlobal + iFloor(1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.dwTime_Hit = Device.dwTimeGlobal + iFloor(2 * 1000.f * height / dest.fSpeed) - Device.dwTimeDelta;
        dest.Phit.set(dest.P);
    }
}
#endif

void CEffect_Rain::SetInvalidateState() { state = stIdle; }
void CEffect_Rain::StopAmbient() { snd_Ambient.stop(); }

#ifdef RAINDROPS
void CEffect_Rain::OnFrame()
{
    if (!g_pGameLevel)
        return;

    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    static float hemi_factor = 0.0f;
    IGameObject* pCurrViewEntity = g_pGameLevel->CurrentViewEntity();
    if (pCurrViewEntity && pCurrViewEntity->renderable_ROS())
    {
        // Determine if we indoor/outdoor by hemi values
        float* hemi_cube = pCurrViewEntity->renderable_ROS()->get_luminocity_hemi_cube();
        float hemi_val = std::max(hemi_cube[0], hemi_cube[1]);
        hemi_val = std::max(hemi_val, hemi_cube[2]);
        hemi_val = std::max(hemi_val, hemi_cube[3]);
        hemi_val = std::max(hemi_val, hemi_cube[5]);

        float f = hemi_val;
        float t = Device.fTimeDelta;
        clamp(t, 0.001f, 1.0f);
        hemi_factor = _lerp(hemi_factor, f, t);
    }
    // Update world wetness timer
    UpdateTimer(*timerWorld, state, false);

    switch (state)
    {
    case stIdle:

    {
        // Update current view entity wetness timer
        // Just drying up, no need for using raypick
        UpdateTimer(*timerCurrViewEntity, state, false);

        if (factor >= EPS_L)
        {
            state = stWorking;
            snd_Ambient.play(nullptr, sm_Looped);
            snd_Ambient.set_position(Fvector().set(0, 0, 0));
            snd_Ambient.set_range(source_offset, source_offset * 2.0f);
        }
    }
    break;
    case stWorking:
    {
        if (factor < EPS_L)
        {
            state = stIdle;
            snd_Ambient.stop();
            return;
        }
        // Update current view entity wetness timer
        // Use raypick to determine whether actor is under cover or not
        UpdateTimer(*timerCurrViewEntity, state, true);
    }
    break;
    }
    if (snd_Ambient._feedback())
    {
        snd_Ambient.set_volume(_max(0.1f, factor) * hemi_factor);
    }
}
#else
void CEffect_Rain::OnFrame()
{
#ifndef _EDITOR
    if (!g_pGameLevel)
        return;
#endif

    if (GEnv.isDedicatedServer)
        return;

    // Parse states
    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    static float hemi_factor = 0.f;
#ifndef _EDITOR
    IGameObject* E = g_pGameLevel->CurrentViewEntity();
    if (E && E->renderable_ROS())
    {
        // hemi_factor = 1.f-2.0f*(0.3f-_min(_min(1.f,E->renderable_ROS()->get_luminocity_hemi()),0.3f));
        float* hemi_cube = E->renderable_ROS()->get_luminocity_hemi_cube();
        float hemi_val = _max(hemi_cube[0], hemi_cube[1]);
        hemi_val = _max(hemi_val, hemi_cube[2]);
        hemi_val = _max(hemi_val, hemi_cube[3]);
        hemi_val = _max(hemi_val, hemi_cube[5]);

        // float f = 0.9f*hemi_factor + 0.1f*hemi_val;
        float f = hemi_val;
        float t = Device.fTimeDelta;
        clamp(t, 0.001f, 1.0f);
        hemi_factor = hemi_factor * (1.0f - t) + f * t;
    }
#endif

    switch (state)
    {
    case stIdle:
        if (factor < EPS_L)
            return;
        state = stWorking;
        snd_Ambient.play(0, sm_Looped);
        snd_Ambient.set_position(Fvector().set(0, 0, 0));
        snd_Ambient.set_range(source_offset, source_offset * 2.f);
        break;
    case stWorking:
        if (factor < EPS_L)
        {
            state = stIdle;
            snd_Ambient.stop();
            return;
        }
        break;
    }

    if (snd_Ambient._feedback())
    {
        snd_Ambient.set_volume(_max(0.1f, factor) * hemi_factor);
    }
}
#endif
#ifdef RAINDROPS

void CEffect_Rain::UpdateTimer(RainTimer& timer, States state, bool bNeedRayPick)
{
    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor > EPS_L)
    {
        // Is raining
        if (state == States::stWorking)
        {
            // Effect is enabled
            Fvector P, D;
            P.set(Device.vCameraPosition); // Camera position
            D.set(0, 1, 0); // Direction to sky

            float max_dist = max_distance;
            u16 material = GAMEMTL_NONE_IDX;
            if (!bNeedRayPick || !RayPick(P, D, max_dist, material, collide::rqtBoth))
            {
                // Under the sky
                if (timer.bFirstRainingFrame)
                {
                    // First frame
                    timer.bFirstRainingFrame = false;
                    timer.rainDropTime = timer.rainDropTimeBasic / factor; // Speed of getting wet
                    timer.rainTimestamp = Device.fTimeGlobal;
                    if (timer.rainTimer > EPS)
                        timer.rainTimestamp += timer.lastRainDuration - timer.rainTimer -
                            std::min(timer.rainDropTime, timer.lastRainDuration);

                    timer.lastRainDuration = 0.0f;
                }

                if (timer.rainTimer < 0.0f)
                    timer.rainTimer = 0.0f;

                timer.rainTimer = Device.fTimeGlobal - timer.rainTimestamp;
            }
            else
            {
                // Under the cover. But may be it just appeared
                if (timer.rainTimer > EPS)
                {
                    // Actor was under the sky recently
                    float delta = timer.rainTimer - (Device.fTimeGlobal - timer.previousFrameTime);
                    timer.rainTimer = (delta > 0.0f) ? delta : 0.0f;
                    if (!timer.bFirstRainingFrame)
                    {
                        // First update since rain was stopped
                        timer.bFirstRainingFrame = true;
                        timer.lastRainDuration = Device.fTimeGlobal - timer.rainTimestamp;
                    }
                }
            }
        }
        else
        {
            // Effect is disabled, reset all
            timer.bFirstRainingFrame = true;
            timer.lastRainDuration = 0.0f;
            timer.rainTimer = 0.0f;
            timer.rainTimestamp = Device.fTimeGlobal;
        }
        timer.previousFrameTime = Device.fTimeGlobal;
    }
    else
    {
        // No rain. But may be it just stopped
        if (timer.rainTimer > EPS)
        {
            // Yes, it has been raining recently
            // so decrease timer
            float delta = timer.rainTimer - (Device.fTimeGlobal - timer.previousFrameTime);
            timer.rainTimer = (delta > 0.0f) ? delta : 0.0f;
            if (!timer.bFirstRainingFrame)
            {
                // First update since rain was stopped
                timer.bFirstRainingFrame = true;
                timer.lastRainDuration = Device.fTimeGlobal - timer.rainTimestamp;
            }
            timer.previousFrameTime = Device.fTimeGlobal;
        }
    }
}
#endif
void CEffect_Rain::Render()
{
#ifndef _EDITOR
    if (!g_pGameLevel)
        return;
#endif
    m_pRender->Render(*this);
}

void CEffect_Rain::Hit(Fvector& pos)
{
    if (0 != ::Random.randI(2))
        return;
    Particle* P = p_allocate();
    if (0 == P)
        return;

    const Fsphere& bv_sphere = m_pRender->GetDropBounds();

    P->time = particles_time;
    P->mXForm.rotateY(::Random.randF(PI_MUL_2));
    P->mXForm.translate_over(pos);
    P->mXForm.transform_tiny(P->bounds.P, bv_sphere.P);
    P->bounds.R = bv_sphere.R;
}
// initialize particles pool
void CEffect_Rain::p_create()
{
    // pool
    particle_pool.resize(max_particles);
    for (u32 it = 0; it < particle_pool.size(); it++)
    {
        Particle& P = particle_pool[it];
        P.prev = it ? (&particle_pool[it - 1]) : 0;
        P.next = (it < (particle_pool.size() - 1)) ? (&particle_pool[it + 1]) : 0;
    }

    // active and idle lists
    particle_active = 0;
    particle_idle = &particle_pool.front();
}

// destroy particles pool
void CEffect_Rain::p_destroy()
{
    // active and idle lists
    particle_active = 0;
    particle_idle = 0;

    // pool
    particle_pool.clear();
}

// _delete_ node from _list_
void CEffect_Rain::p_remove(Particle* P, Particle*& LST)
{
    VERIFY(P);
    Particle* prev = P->prev;
    P->prev = NULL;
    Particle* next = P->next;
    P->next = NULL;
    if (prev)
        prev->next = next;
    if (next)
        next->prev = prev;
    if (LST == P)
        LST = next;
}

// insert node at the top of the head
void CEffect_Rain::p_insert(Particle* P, Particle*& LST)
{
    VERIFY(P);
    P->prev = 0;
    P->next = LST;
    if (LST)
        LST->prev = P;
    LST = P;
}

// determine size of _list_
int CEffect_Rain::p_size(Particle* P)
{
    if (0 == P)
        return 0;
    int cnt = 0;
    while (P)
    {
        P = P->next;
        cnt += 1;
    }
    return cnt;
}

// alloc node
CEffect_Rain::Particle* CEffect_Rain::p_allocate()
{
    Particle* P = particle_idle;
    if (0 == P)
        return NULL;
    p_remove(P, particle_idle);
    p_insert(P, particle_active);
    return P;
}

// xr_free node
void CEffect_Rain::p_free(Particle* P)
{
    p_remove(P, particle_active);
    p_insert(P, particle_idle);
}
