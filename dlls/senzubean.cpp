#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "classes.h"
#include "aura.h"
#include "player.h"
#include "items.h"
#include "gamerules.h"

/**
* Senzubean class
* @version: 16-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CSenzuBean : public CItem
{	/**
	* Spawns the bean
	*/
	void Spawn();

	/**
	* Precaches the models
	*/
	void Precache();
};
LINK_ENTITY_TO_CLASS(item_senzubean, CSenzuBean);

void CSenzuBean::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_senzubean.mdl");
	CItem::Spawn();
}

void CSenzuBean::Precache()
{
	PRECACHE_MODEL("models/w_senzubean.mdl");
}