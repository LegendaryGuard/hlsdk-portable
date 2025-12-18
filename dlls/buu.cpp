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

CBuu::CBuu ( CBasePlayer *player ): CBaseClass(player) 
{
	g_engfuncs.pfnSetClientKeyValue( m_pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( m_pPlayer->edict() ), "model", "notdone"); //sets the model for the player	
}

void CBuu::AddAllMagic (void)
{
	m_pPlayer->GiveNamedItem("weapon_melee");
	m_pPlayer->GiveNamedItem("weapon_kiblast");
}

 