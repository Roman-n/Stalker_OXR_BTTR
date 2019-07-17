#pragma once
#include "UIProgressBar.h"
#include "UIProgressShape.h"

class CUIMotionIcon_soc : public CUIWindow
{
	typedef CUIWindow inherited;
public:
	enum	EState		{
						stNormal,
						stCrouch,
						stCreep,
						stClimb,
						stRun,
						stSprint,
						stLast
					};
private:
							CUIStatic			background_shoc;
							EState				m_curren_state;
							CUIStatic			m_states[stLast];
							CUIProgressBar		m_luminosity_progress_;
							CUIProgressBar		m_noise_progress;

		struct _npc_visibility{
			u16				id;
			float				value;
			bool operator == (const u16& _id){
				return id == _id;
			}
			bool operator < (const _npc_visibility& m) const
			{
				return (value < m.value);
			}
		};
		xr_vector<_npc_visibility>	m_npc_visibility;
		bool						m_bchanged;
		float						m_luminosity;
		float						cur_pos;

public:
	virtual					~CUIMotionIcon_soc		();
							CUIMotionIcon_soc		();
	virtual	void			Update				();
	virtual void			Draw				();
			void			Init();
			void			ShowState			(EState state);			
			void			SetNoise			(float Pos);
			void			SetLuminosity_		(float Pos);
			void			SetActorVisibility_	(u16 who_id, float value);
			void			ResetVisibility		();
};
