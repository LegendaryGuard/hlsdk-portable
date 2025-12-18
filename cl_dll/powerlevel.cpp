#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_PowerLevel, PowerLevel);
DECLARE_MESSAGE(m_PowerLevel, MaxPL);

extern int DrawBar(int x, int y, int width, int height, float f);

int CHudPowerLevel::Init()
{
	HOOK_MESSAGE(PowerLevel);
	HOOK_MESSAGE(MaxPL);
	gHUD.AddHudElem(this);
	m_iPowerLevel = 750000;
	m_iMaxPowerLevel = 1000000;

	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudPowerLevel::VidInit()
{
	m_HUD_powerlevel = gHUD.GetSpriteIndex("powerlevel");
	return 1;
}

int CHudPowerLevel::Draw(float fltime)
{
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 1;

	if ((gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return 1;

	int r, g, b, x, y;
	UnpackRGB(r, g, b, RGB_YELLOWISH);

	ScaleColors(r, g, b, 200 );
	int PowerLevelWidth = gHUD.GetSpriteRect(m_HUD_powerlevel).right - gHUD.GetSpriteRect(m_HUD_powerlevel).left;
	int PowerLevelHeigth = gHUD.GetSpriteRect(m_HUD_powerlevel).bottom - gHUD.GetSpriteRect(m_HUD_powerlevel).top;
	int width = ScreenWidth * 0.25 - PowerLevelWidth;

	y = ScreenHeight - PowerLevelHeigth * 4.5; //assume KiHeigt==HealthHeight==PowerLevelHeight
	x = PowerLevelWidth / 2;

	SPR_Set(gHUD.GetSprite(m_HUD_powerlevel), r, g, b);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_powerlevel));

	//added by harSens: draw health bar
	x += PowerLevelWidth + PowerLevelWidth / 2;

	//draw outside lines
	int heigth = PowerLevelHeigth;
	UnpackRGB(r, g, b, RGB_GREENISH);
	FillRGBA(x, y, width, 1, r, g, b, 255);
	FillRGBA(x, y + heigth - 1, width, 1, r, g, b, 255);
	FillRGBA(x, y + 1, 1, heigth - 1, r, g, b, 255);
	FillRGBA(x + width - 1, y + 1, 1, heigth - 1, r, g, b, 255);
	width -= 2;
	heigth -= 2;
	x++;
	y++;

	int w = (m_iPowerLevel * width) / m_iMaxPowerLevel;

	// Always show at least one pixel if we have a powerlevel
	if (w <= 0)
		w = 1;

	// draw numbers
	UnpackRGB(r, g, b, RGB_WHITEISH);
	int number_heigth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).bottom - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).top;
	int digit_width = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left ;
	int number_width = digit_width * gHUD.GetNumWidth(m_iPowerLevel, DHN_KI);
	//check if number fits in bar
	if (number_heigth < heigth && number_width < width)
	{
		int number_y = y + (heigth - number_heigth) / 2;
		int number_x = x + (width - number_width) / 2;
		gHUD.DrawHudNumber(number_x, number_y, DHN_KI, m_iPowerLevel, r, g, b);
	}

	// draw yellow bar
	UnpackRGB(r, g, b, RGB_YELLOWISH);
	FillRGBA(x, y, w, heigth, r, g, b, 255);
	x += w;
	width -= w;

	// draw empty part
	UnpackRGB(r, g, b, RGB_YELLOWISH);
	FillRGBA(x, y, width, heigth, r, g, b, 128);

	return 1;
}

int CHudPowerLevel::MsgFunc_PowerLevel(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iPowerLevel = READ_LONG();
	return 1;
}

int CHudPowerLevel::MsgFunc_MaxPL(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iMaxPowerLevel = READ_LONG();
	return 1;
}
