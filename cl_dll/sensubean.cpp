#include "hud.h"
#include "parsemsg.h"
#include "const.h"
#include "cl_util.h"

DECLARE_MESSAGE(m_SensuBean, SensuBean);

int CHudSensuBean::Init()
{
	HOOK_MESSAGE(SensuBean);
	m_iSensuBeans = 0;	

	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudSensuBean::VidInit()
{
	m_HUD_bean = gHUD.GetSpriteIndex("bean");
	return 1;
}

int CHudSensuBean::Draw(float flTime)
{
	//no beans, so don't draw a thing
	if (!m_iSensuBeans) 
		return 1;

	//draw beans
	int iFlags = DHN_DRAWZERO; // draw 0 values
	int iIconWidth = gHUD.GetSpriteRect(m_HUD_bean).right - gHUD.GetSpriteRect(m_HUD_bean).left;
	int iBeanWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
	int x = ScreenWidth - 2 * iBeanWidth - iIconWidth;
	int y = 0;
	int r, g, b;
	UnpackRGB(r, g, b, RGB_WHITEISH);
	x = gHUD.DrawHudNumber(x, y, iFlags | DHN_2DIGITS, m_iSensuBeans, r, g, b);

	UnpackRGB(r, g, b, RGB_YELLOWISH);
	SPR_Set(gHUD.GetSprite(m_HUD_bean), r, g, b);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_bean));

	return 1;
}

int CHudSensuBean::MsgFunc_SensuBean(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iSensuBeans = READ_BYTE();
	return 1;
}