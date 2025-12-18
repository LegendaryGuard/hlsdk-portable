#include "extdll.h"
#include "util.h"

#include "cbase.h"
#include "nodes.h"
#include "weapons.h"
#include "soundent.h"
#include "monsters.h"
#include "decals.h"
#include "gamerules.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"

const char *class_names[] =
{
	"piccolo",
	"goku",
	"vegeta",
	"krillin",
	"trunks",
	"frieza",
	"buu",
	"gohan",
	"cell"	
};

CBaseClass::CBaseClass ( CBasePlayer *player )
{
	m_pPlayer = player;
}

const char *CBaseClass::GetName( int id )
{
	if (id >= 0 && id < PC_LAST)
		return class_names[id];
	else 
		return NULL;
}

