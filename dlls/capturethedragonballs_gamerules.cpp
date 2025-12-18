//
// capture the dragonballs gamerules
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"effects.h"
#include	"aura.h"
#include	"classes.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"dragonball.h"
#include	"teamplay_gamerules.h"
#include	"capturethedragonballs_gamerules.h"
#include	"game.h"

CHalfLifeCaptureTheDragonBalls :: CHalfLifeCaptureTheDragonBalls()
{
	CHalfLifeTeamplay::CHalfLifeTeamplay();
}

void CHalfLifeCaptureTheDragonBalls :: Think()
{
	if (!m_fSpawnedBalls)
		SpawnDragonBalls();

	CHalfLifeTeamplay :: Think();
}

void CHalfLifeCaptureTheDragonBalls :: SpawnDragonBalls()
{	//find item_dragonball spots on the map
	edict_t *pSpot[100];
	int nr_of_balls = 0;
	pSpot[0] = FIND_ENTITY_BY_CLASSNAME( NULL, "item_dragonball");

	while( nr_of_balls<100 && !FNullEnt(pSpot[nr_of_balls]))
	{	nr_of_balls++;
		pSpot[nr_of_balls] = FIND_ENTITY_BY_CLASSNAME( pSpot[nr_of_balls-1], "item_dragonball");
	}
		
	//enough dragonball spots found?
	if (nr_of_balls < 7)
	{	ALERT(at_console, "not enough dragonballs in this map\n");
		return;
	}

	//randomly find 7 dragonball indices
	int spots[7];
	int i, n;
	int newspot;
	for (n = 0; n < 7; n++)
	{
		do
		{
			i = RANDOM_LONG(0, nr_of_balls - 1);
			newspot = true;
			for(int m = 0; m < n; m++)
			{
				if (spots[m] == i)
					newspot = false;
			}
		}
		while (!newspot);
		spots[n] = i;
	}

	for (n = 0; n < 7; n++)
	{
		m_pDragonBalls[n] = CDragonBall::CreateDragonBall(pSpot[spots[n]]->v.origin, n);
		//invalid spawn point
		if (!m_pDragonBalls[n]) 
			return;
	}

	m_fSpawnedBalls = true;
}

/**
* DragonBallPoint class, spawnpoints for dragonballs
* @version 19-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CDragonBallPoint:public CPointEntity
{
	void Spawn();		
};

void CDragonBallPoint::Spawn()
{
	UTIL_SetOrigin(pev, pev->origin);
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;
}
LINK_ENTITY_TO_CLASS(item_dragonball, CDragonBallPoint);