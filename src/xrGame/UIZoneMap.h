#pragma once
//���������� ���� �����

#include "ui/UIStatic.h"

class CActor;
class CUIMiniMap;


class CUIZoneMap
{
public:
	bool						visible;

private:
	CUIMiniMap*					m_activeMap;

	CUIStatic					m_background;
	CUIStatic*					m_clock_wnd;
	
//	CUIStatic*					m_time_frame;
	
	CUIStatic					m_center;
	CUIStatic					m_times;
	CUIStatic					m_compass;
#ifdef DIST_TO_POINT_CS
    CUIStatic*					m_pointerDistanceText;
#endif
	CUIWindow					m_clipFrame;
	CUIStatic					m_Counter;
	CUITextWnd					m_Counter_text;
//	CUITextWnd					m_pointerDistanceText;	
	u8							m_current_map_idx;

public:
								CUIZoneMap		();
	virtual						~CUIZoneMap		();

	void						Init			();

	void						Render			();
	void						Update			();

	bool						ZoomIn			();
	bool						ZoomOut			();

	CUIStatic&					Background		()									{return m_background;};
	CUIWindow&					MapFrame		()									{return m_clipFrame;};
	void						SetupCurrentMap	();
	void						OnSectorChanged	(int sector);
	void						Counter_ResetClrAnimation();

private:
	void						SetHeading		(float angle);
	void						UpdateRadar		(Fvector pos);

};
