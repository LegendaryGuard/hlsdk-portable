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

CCell::CCell ( CBasePlayer *player ): CBaseClass(player) 
{
	g_engfuncs.pfnSetClientKeyValue( m_pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( m_pPlayer->edict() ), "model", "cell"); //sets the model for the player	
}

void CCell::AddAllMagic()
{
	m_pPlayer->GiveNamedItem("weapon_melee");
	m_pPlayer->GiveNamedItem("weapon_kiblast");
	m_pPlayer->GiveNamedItem("weapon_friezadisc");
	m_pPlayer->GiveNamedItem("weapon_kamehameha");
	m_pPlayer->GiveNamedItem("weapon_gallitgun");
	m_pPlayer->GiveNamedItem("weapon_specialbeamcannon");
	m_pPlayer->GiveNamedItem("weapon_solarflare");
	m_pPlayer->GiveNamedItem("weapon_sensu");
}