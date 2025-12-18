#include "hud.h"
#include "parsemsg.h"
#include "cl_util.h"
#include "const.h"
#include "pm_shared.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "event_api.h"

DECLARE_MESSAGE(m_ChargeSpiritBomb, ChargeSB);
DECLARE_MESSAGE(m_ChargeSpiritBomb, StopChSB);
HSPRITE m_hsprSBCharge;

charge_sprite chargesb[200];
int charge_sb;

int CHudChargeSpiritBomb::Init()
{
	HOOK_MESSAGE(ChargeSB);
	HOOK_MESSAGE(StopChSB);
	charge_sb = false;
	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudChargeSpiritBomb::VidInit()
{
	m_hsprSBCharge = LoadSprite("sprites/spiritbombcharge.spr");
	return 1;
}

void CHudChargeSpiritBomb::Think()
{
	if (!charge_sb)
		return;

	//check in we can restart this sprite
	for (int n = 0; n < 200; n++)
	{
		if (Length(m_vecBomb - chargesb[n].origin) < 25)
			CreateCharger(chargesb[n]);
	}
}

void CHudChargeSpiritBomb::CreateCharger(charge_sprite &charger)
{
	vec3_t vecDir;
	vecDir.x = gEngfuncs.pfnRandomFloat(-1, 1);
	vecDir.y = gEngfuncs.pfnRandomFloat(-1, 1);
	vecDir.z = -1;
	VectorNormalize(vecDir);
	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_PlayerTrace(m_vecBomb, m_vecBomb + vecDir * 8192, PM_NORMAL, -1, &tr);
	charger.origin = tr.endpos;
	charger.direction = m_vecBomb - tr.endpos;
	VectorNormalize(charger.direction);
	charger.direction = charger.direction * gEngfuncs.pfnRandomFloat(100, 400);
}

int CHudChargeSpiritBomb::MsgFunc_ChargeSB(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_vecBomb.x = READ_COORD();
	m_vecBomb.y = READ_COORD();
	m_vecBomb.z = READ_COORD();

	//calculate start positions
	for (int n = 0; n < 200; n++)
	{
		CreateCharger(chargesb[n]);
	}

	charge_sb = true;
	return 1;
}

int CHudChargeSpiritBomb::MsgFunc_StopChSB(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	charge_sb = false;
	return 1;
}