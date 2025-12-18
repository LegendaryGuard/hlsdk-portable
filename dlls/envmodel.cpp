#include "extdll.h" 
#include "decals.h" 
#include "util.h" 
#include "cbase.h" 
#include "monsters.h" 
#include "weapons.h" 
#include "nodes.h" 
#include "effects.h"
#include "soundent.h" 
#include "shake.h" 
#include "gamerules.h" 

/**
* Adds models to the map
* @author: Herwin 'harSens' van Welbergen
* @version: 21-6-2001
*/
class CEnvModel: public CBaseEntity 
{	
public: 
    /**
	* Spawns the env model
	*/
	void Spawn(); 

	/**
	* Precaches the env model
	*/
    void Precache();
};
LINK_ENTITY_TO_CLASS(env_model, CEnvModel);

void CEnvModel::Spawn()
{
	Precache();
	SET_MODEL( ENT(pev), STRING(pev->model) );
	UTIL_SetOrigin( pev, pev->origin ); 
    pev->movetype = MOVETYPE_TOSS; 
    pev->solid = SOLID_BBOX; 
}

void CEnvModel::Precache()
{
	PRECACHE_MODEL( (char*)STRING(pev->model) );	
}

