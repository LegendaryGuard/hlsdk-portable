#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#define BAR_HEIGHT (ScreenHeight / 25)
#define BAR_WIDTH (ScreenWidth / 8)
#define BAR_YPOS (ScreenHeight - BAR_HEIGHT - 10)
#define BAR_XPOS ((ScreenWidth * 3) / 4 - BAR_WIDTH)

DECLARE_MESSAGE(m_PowerStruggle, PowerStrug);

int CHudPowerStruggle::Init()
{
	m_fPowerStruggle = false;
	HOOK_MESSAGE(PowerStrug);
	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudPowerStruggle::Draw(float fltime)
{
	if (!m_fPowerStruggle)
		return 1;

	int r, g, b, r_left, g_left, b_left, r_right, g_right, b_right;
	UnpackRGB(r_right, g_right, b_right, RGB_GREENISH);
	UnpackRGB(r_left, g_left, b_left, RGB_REDISH);
	float a = 128;
	float a_dec = (float)BAR_WIDTH / 128.0;
	for (int x = 0; x < BAR_WIDTH; x++)
	{
		FillRGBA(BAR_XPOS + x, BAR_YPOS, 1, BAR_HEIGHT, r_right, g_right, b_right, a);
		FillRGBA(BAR_XPOS - x, BAR_YPOS, 1, BAR_HEIGHT, r_left, g_left, b_left, a);
		a -= a_dec;
	}
	UnpackRGB(r, g, b, RGB_YELLOWISH);
	FillRGBA(BAR_XPOS + m_flPowerRatio * BAR_WIDTH, BAR_YPOS, 2, BAR_HEIGHT, r, g, b, 128);
	return 1;
}

int CHudPowerStruggle:: MsgFunc_PowerStrug(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int n = READ_CHAR();
	if (n == 127)
		m_fPowerStruggle = false;
	else
	{
		m_fPowerStruggle = true;
		m_flPowerRatio = (float)n / 100.0;
	}
	return 1;
}