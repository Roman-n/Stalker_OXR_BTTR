#pragma once

#include "xruicore/static/UIStatic.h"

class CActor;
class CUIMiniMap;

class CUIZoneMap
{
public:
    bool visible;
    bool disabled;

private:
    CUIMiniMap* m_activeMap;

    CUIStatic m_background;
    CUIStatic* m_clock_wnd;
    CUIStatic m_center;
    CUIStatic m_compass;
    CUIWindow m_clipFrame;
#ifdef DIST_TO_POINT_CS
    CUIStatic* m_pointerDistanceText;
    CUIStatic* m_pointerDistanceText_16_9;
#endif	
    CUIStatic m_Counter;
    CUITextWnd m_Counter_text;
    u8 m_current_map_idx;

public:
    CUIZoneMap();
    virtual ~CUIZoneMap();

    void Init();

    void Render();
    void Update();

    bool ZoomIn();
    bool ZoomOut();

    CUIStatic& Background() { return m_background; };
    CUIWindow& MapFrame() { return m_clipFrame; };
    void SetupCurrentMap();
    void OnSectorChanged(int sector);
    void Counter_ResetClrAnimation();

private:
    void SetHeading(float angle);
    void UpdateRadar(Fvector pos);
};
