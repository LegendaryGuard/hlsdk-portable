#include "hud.h"
#include "parsemsg.h"
#include "const.h"
#include "cl_util.h"
#include "r_efx.h"

DECLARE_MESSAGE(m_Trail, CreateTrail);
DECLARE_MESSAGE(m_Trail, RemoveTrail);
DECLARE_MESSAGE(m_Trail, AddPoint);

int CHudTrail::Init()
{
	HOOK_MESSAGE(CreateTrail);
	HOOK_MESSAGE(RemoveTrail);
	HOOK_MESSAGE(AddPoint);
	m_iTrails = 0;

	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

void CHudTrail::DrawTrail(trail *trTrail)
{
	trail_points *current = trTrail->points;
	vec3_t vecDir = Vector(0, 0, 0);
	vec3_t vecStart;
	vec3_t vecEnd;

	while (current->next)
	{
		vecEnd = current->next->point;
		vecStart = current->point;

		//adjust start of trail, to fill out holes
		if (vecDir.Length() > trTrail->width)
			vecStart = vecStart - vecDir.Normalize() * trTrail->width;

		gEngfuncs.pEfxAPI->R_BeamPoints(vecStart, vecEnd, trTrail->sprite, 0.3, trTrail->width, 0, 254, 0, 0, 10, 255, 255, 255);
		vecDir = vecEnd - vecStart;
		current = current->next;
	}
	gEngfuncs.pEfxAPI->R_BeamEntPoint( trTrail->target, current->point, trTrail->sprite, 0.3, trTrail->width, 0, 254, 0, 0, 10, 255, 255, 255);
}

int CHudTrail::Draw(float flTime)
{
	if (flTime < m_flLastBeamDraw || flTime - m_flLastBeamDraw > 0.27)
	{
		for (int n = 0; n < m_iTrails; n++)
		{
			DrawTrail(&m_Trail[n]);
		}

		m_flLastBeamDraw = flTime;
	}
	return 1;
}

int CHudTrail::MsgFunc_AddPoint(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	vec3_t vecPoint;
	vecPoint.x = READ_COORD();
	vecPoint.y = READ_COORD();
	vecPoint.z = READ_COORD();
	int target = READ_LONG();

	//find the trail
	int add_trail = FindTrail(target);
	if (add_trail == -1)
	{
		ConsolePrint("invalid addpoint trail message\n");
		return 1;
	}

	//create the new point
	trail_points *newpoint = new trail_points;
	newpoint->next = NULL;
	newpoint->point = vecPoint;

	//find last point
	trail_points *last = m_Trail[add_trail].points;
	while (last->next)
	{
		last = last->next;
	}

	//add the new point
	last->next = newpoint;
	return 1;
}

int CHudTrail::FindTrail(int target)
{
	//find the trail
	int find = -1;
	for (int n = 0; n < m_iTrails; n++)
	{
		if (m_Trail[n].target == target)
		{
			find = n;
			break;
		}
	}
	return find;
}

int CHudTrail::MsgFunc_RemoveTrail(const char *pszName, int iSize, void *pbuf)
{
	vec3_t vecStart;
	BEGIN_READ(pbuf, iSize);
	int target = READ_LONG();

	//find the trail
	int remove_trail=FindTrail(target);

	// trail found?
	if (remove_trail==-1)
		return 1;

	//remove all trails from target
	gEngfuncs.pEfxAPI->R_BeamKill(target);

	//free memory
	trail_points *current = m_Trail[remove_trail].points;
	trail_points *temp;
	while (current->next)
	{
		temp = current->next;
		delete current;
		current = temp;
	}
	delete current;

	//decrease trailcount
	m_iTrails--;
	m_Trail[remove_trail] = m_Trail[m_iTrails];	

	return 1;
}

void CHudTrail::AddTrail(Vector start, int target, short sprite, unsigned char width)
{
	if (m_iTrails < 100)
	{
		m_Trail[m_iTrails].points = new trail_points;
		m_Trail[m_iTrails].points->point = start;
		m_Trail[m_iTrails].points->next = NULL;
		m_Trail[m_iTrails].target = target;
		m_Trail[m_iTrails].sprite = sprite;
		m_Trail[m_iTrails].width = (float)width * 0.1;
		m_iTrails++;
	}
	else
		ConsolePrint("To many trails in the level\n");
}

int CHudTrail::MsgFunc_CreateTrail(const char *pszName, int iSize, void *pbuf)
{
	vec3_t vecStart;
	BEGIN_READ(pbuf, iSize);
	vecStart.x = READ_COORD();
	vecStart.y = READ_COORD();
	vecStart.z = READ_COORD();
	int target = READ_LONG();
	short sprite = READ_SHORT();
	unsigned char width = READ_BYTE();

	AddTrail(vecStart, target, sprite, width);
	return 1;
}