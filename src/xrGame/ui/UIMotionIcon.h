#pragma once
#include "UIProgressBar.h"
#include "UIProgressShape.h"

class CUIMotionIcon : public CUIWindow
{
    typedef CUIWindow inherited;

public:

    enum EState
    {
        stNormal,
        stCrouch,
        stCreep,
        stClimb,
        stRun,
        stSprint,
        stLast
    };

private:
    CUIProgressShape m_luminosity_progress_shape;
    CUIProgressShape m_noise_progress_shape;

    CUIStatic background_shoc;
    EState m_curren_state;
    CUIStatic m_states[stLast];
    CUIProgressBar m_luminosity_progress;
    CUIProgressBar m_noise_progress;

    struct _npc_visibility
    {
        u16 id;
        float value;
        bool operator==(const u16& _id) { return id == _id; }
        bool operator<(const _npc_visibility& m) const { return (value < m.value); }
    };
    xr_vector<_npc_visibility> m_npc_visibility;
    bool m_bchanged;
    float m_luminosity;
    float cur_pos;

public:
    virtual ~CUIMotionIcon();
    CUIMotionIcon();
    virtual void Update();
    virtual void Draw();

    void Init_soc();
    void ShowState(EState state);

    void Init(Frect const& rect);
    void SetNoise(float Pos);
    void SetLuminosity(float Pos);
    void SetActorVisibility(u16 who_id, float value);
    void ResetVisibility();
};
