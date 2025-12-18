#include "hud.h"
#include "parsemsg.h"
#include "const.h"
#include "cl_util.h"
#include "r_efx.h"
#include "event_api.h"
#define PI 3.1415926535897932384626433832795

DECLARE_MESSAGE(m_Explosion, Explosion);
explosion *tri_exp;
HSPRITE m_hsprExplosion;
HSPRITE m_hsprExplosionBeam;
float cos_angle[36];
float sin_angle[36];

int CHudExplosion::Init()
{
	HOOK_MESSAGE(Explosion);
	tri_exp = NULL;
	m_flLastResize = 0;

	float fRadAngle;
	for (int n = 0; n < 36; n++)
	{
		fRadAngle = (PI * n) / 18.0;
		cos_angle[n] = cos (fRadAngle);
		sin_angle[n] = sin (fRadAngle);
	}

	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudExplosion::VidInit()
{
	m_hsprExplosion = LoadSprite("sprites/explosion.spr");
	m_hsprExplosionBeam = LoadSprite("sprites/explosionbeam.spr");
	m_iBeam = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/explosion.spr");
	return 1;
}

int CHudExplosion::Draw(float flTime)
{
	//clock reset, or time for another shrink
	if (flTime < m_flLastResize || flTime - m_flLastResize > 0.2)
	{
		explosion *current = tri_exp;
		while (current)
		{
			//current->light->radius = current->radius * 2;
			if (current->radius > 1)
			{
				current->radius *= 0.7;
				current = current->next;
			}
			else
				current = RemoveExplosion(current);
		}
		m_flLastResize = flTime;
	}
	return 1;
}

explosion* CHudExplosion::RemoveExplosion(explosion *exp)
{
	explosion *prev;
	prev = tri_exp;

	//should never happen...
	if (!prev)
		return NULL;

	//remove 1st explosion
	if (prev == exp)
	{
		tri_exp = prev->next;
		delete exp;
		return tri_exp;
	}

	while (prev->next != exp)
	{
		prev = prev->next;
		//should not happen...
		if (!prev)
			return NULL;
	}
	prev->next = exp->next;
	delete exp;
	return prev->next;
}

void CHudExplosion::AddExplosion(Vector org, int radius, char r, char g, char b, char a)
{
	//draw light flash
	dlight_t *light = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
	light->origin = org;
	light->radius = radius * 1.5;
	light->color.r = r;
	light->color.g = g;
	light->color.b = b;
	light->die = gEngfuncs.GetClientTime() + 0.5;

	//draw beams
	explosion *new_exp = new explosion;
	new_exp->pos.x = org.x;
	new_exp->pos.y = org.y;
	new_exp->pos.z = org.z;
	new_exp->r = r;
	new_exp->g = g;
	new_exp->b = b;
	new_exp->a = a;
	new_exp->radius = radius;
	new_exp->next = NULL;

	//first explosion
	if (!tri_exp)
		tri_exp = new_exp;
	else
	{
		//add it as last explosion
		explosion *current = tri_exp;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new_exp;
	}
}

int CHudExplosion::MsgFunc_Explosion(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	Vector vecMiddle;
	vecMiddle.x = READ_COORD();
	vecMiddle.y = READ_COORD();
	vecMiddle.z = READ_COORD();
	int radius = READ_LONG();
	char r = READ_CHAR();
	char g = READ_CHAR();
	char b = READ_CHAR();
	char a = READ_CHAR();
	AddExplosion(vecMiddle, radius, r, g, b, a);
	return 1;
}