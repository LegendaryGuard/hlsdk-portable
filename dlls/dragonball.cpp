#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "dragonball.h"

LINK_ENTITY_TO_CLASS( dragonball, CDragonBall );

void CDragonBall::Spawn()
{
	Precache();	
}

void CDragonBall::Precache()
{
	PRECACHE_MODEL("models/db1.mdl");
	PRECACHE_MODEL("models/db2.mdl");
	PRECACHE_MODEL("models/db3.mdl");
	PRECACHE_MODEL("models/db4.mdl");
	PRECACHE_MODEL("models/db5.mdl");
	PRECACHE_MODEL("models/db6.mdl");
	PRECACHE_MODEL("models/db7.mdl");
}

CDragonBall *CDragonBall::CreateDragonBall(Vector origin, int nr)
{
	CDragonBall *pDragonBall = GetClassPtr( (CDragonBall *)NULL );
	pDragonBall->Spawn();
	
	if ( nr < 0 || nr > 6 )
		return NULL;

	char model[255];
	sprintf(model, "models/db%d.mdl", nr + 1);
	SET_MODEL(ENT(pDragonBall->pev), model);

	pDragonBall->pev->origin = origin;
	UTIL_SetOrigin(pDragonBall->pev, origin);

	pDragonBall->pev->movetype = MOVETYPE_TOSS;
	pDragonBall->pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pDragonBall->pev, Vector(-16, -16, 0), Vector(16, 16, 16));
	
	if (DROP_TO_FLOOR(ENT(pDragonBall->pev)) == 0)
	{
		ALERT(at_error, "Item %s fell out of level at %f,%f,%f", STRING( pDragonBall->pev->classname ), pDragonBall->pev->origin.x, pDragonBall->pev->origin.y, pDragonBall->pev->origin.z);
		UTIL_Remove( pDragonBall );
		return NULL;
	}

	return pDragonBall;
}