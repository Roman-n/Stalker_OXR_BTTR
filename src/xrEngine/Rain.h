// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RainH
#define RainH
#pragma once

#include "xrCDB/xr_collide_defs.h"

// refs

class ENGINE_API IRender_DetailModel;

#include "Include/xrRender/FactoryPtr.h"
#include "Include/xrRender/RainRender.h"
//
class ENGINE_API CEffect_Rain
{
    friend class dxRainRender;

private:
#ifdef RAINDROPS
// Timers for simulating smooth wetting/drying effect
	struct RainTimer
	{
		bool			bFirstRainingFrame;
		float			rainTimestamp;
		float			rainDropTimeBasic;
		float			previousFrameTime;
		float			lastRainDuration;
		float			rainDropTime;
		float			rainTimer;

		RainTimer() : bFirstRainingFrame(true), rainTimestamp(0.0f), rainDropTimeBasic(20.0f), previousFrameTime(0.0f),
			lastRainDuration(0.0f), rainDropTime(0.0f), rainTimer(0.0f)
		{
		};
	};
#endif
    struct Item
    {
        Fvector P;
        Fvector Phit;
        Fvector D;
        float fSpeed;
        u32 dwTime_Life;
        u32 dwTime_Hit;
        u32 uv_set;
#ifdef RAINDROPS		
		u16	targetMaterial;
#endif		
        void invalidate() { dwTime_Life = 0; }
    };
    struct Particle
    {
        Particle *next, *prev;
        Fmatrix mXForm;
        Fsphere bounds;
        float time;
    };
    enum States
    {
        stIdle = 0,
        stWorking
    };
	#ifdef RAINDROPS
public:
	struct RainParams
	{
		float			rainTimer;
	};
#endif	
private:

    FactoryPtr<IRainRender> m_pRender;

    xr_vector<Item> items;
    States state;

    xr_vector<Particle> particle_pool;
    Particle* particle_active;
    Particle* particle_idle;

    ref_sound snd_Ambient;


	RainTimer*						timerWorld;
	RainTimer*						timerCurrViewEntity;

	void							ParticleFree	(Particle* P);
	void Hit				(Item& item);


    void p_create();
    void p_destroy();
	void p_remove(Particle* P, Particle*& LST);
    void p_insert(Particle* P, Particle*& LST);
    int p_size(Particle* LST);
    Particle* p_allocate();
    void p_free(Particle* P);
    void Hit(Fvector& pos);
    bool RayPick(const Fvector& s, const Fvector& d, float& range, u16& targetMaterial, collide::rq_target tgt);
    void RenewItem(Item& dest, float height, u16 targetMaterial, bool bHit);
	
    

    void Born(Item& dest, float radius);


public:
    CEffect_Rain();
    ~CEffect_Rain();
#ifdef RAINDROPS
	float							GetWorldWetness			() const;
	float							GetCurrViewEntityWetness() const;
	void							UpdateTimer		(RainTimer& timer, States state, bool bNeedRayPick);
    void Load();
#endif	
    void Render();
    void OnFrame();


    void StopAmbient();
    void SetInvalidateState();
};

#endif // RainH
