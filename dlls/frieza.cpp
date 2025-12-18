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


CFrieza::CFrieza ( CBasePlayer *player ): CBaseClass(player) 
{
	g_engfuncs.pfnSetClientKeyValue( m_pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( m_pPlayer->edict() ), "model", "frieza"); //sets the model for the player	
}

void CFrieza::AddAllMagic (void)
{
	m_pPlayer->GiveNamedItem("weapon_melee");
	m_pPlayer->GiveNamedItem("weapon_kiblast");
	m_pPlayer->GiveNamedItem("weapon_fingerlaser");
	m_pPlayer->GiveNamedItem("weapon_friezadisc");
	m_pPlayer->GiveNamedItem("weapon_deathball");
	m_pPlayer->GiveNamedItem("weapon_sensu");
} 
