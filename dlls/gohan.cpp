#include "extdll.h"
#include "util.h"

#include "cbase.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "nodes.h"
#include "weapons.h"
#include "soundent.h"
#include "monsters.h"
#include "decals.h"
#include "gamerules.h"

CGohan::CGohan ( CBasePlayer *player ): CBaseClass(player) 
{
	g_engfuncs.pfnSetClientKeyValue( m_pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( m_pPlayer->edict() ), "model", "notdone" ); //sets the model for the player	
}

void CGohan::AddAllMagic (void)
{
	m_pPlayer->GiveNamedItem("weapon_melee");
	m_pPlayer->GiveNamedItem("weapon_kiblast");
	m_pPlayer->GiveNamedItem("weapon_kamehameha");
	m_pPlayer->GiveNamedItem("weapon_masenko");
	m_pPlayer->GiveNamedItem("weapon_sensu");
}
 