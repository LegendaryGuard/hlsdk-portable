#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#define BAR_HEIGHT (ScreenHeight / 25)
#define BAR_WIDTH (ScreenWidth / 4)
#define BAR_YPOS (ScreenHeight - BAR_HEIGHT - 10)
#define BAR_XPOS (ScreenWidth / 2 - BAR_WIDTH / 2)

DECLARE_MESSAGE(m_Charge, Charge);
extern int DrawBar(int x, int y, int width, int height, float f);

int CHudCharge::Init()
{
	HOOK_MESSAGE(Charge);
	gHUD.AddHudElem(this);
	m_flFill = 0;

	m_iFlags |= HUD_ACTIVE;
	return 1;
}

void CHudCharge::DrawChargeBar()
{
	DrawBar(BAR_XPOS, BAR_YPOS, BAR_WIDTH, BAR_HEIGHT, m_flFill);
	int r, g, b;
	UnpackRGB(r, g, b, RGB_YELLOWISH);
	FillRGBA(BAR_XPOS + BAR_WIDTH / 3, BAR_YPOS - 2, 2, BAR_HEIGHT + 4, r, g, b, 128);	
}

int CHudCharge::Draw(float fltime)
{
	if (m_flFill == 0)
		return 1;

	DrawChargeBar();
	return 1;
}

int CHudCharge::MsgFunc_Charge(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int n = READ_BYTE();
	m_flFill = (float)n / 100;
	return 1;
}