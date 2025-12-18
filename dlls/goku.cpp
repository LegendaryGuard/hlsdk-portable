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

CGoku::CGoku ( CBasePlayer *player ): CBaseClass(player) 
{
	g_engfuncs.pfnSetClientKeyValue( m_pPlayer->entindex(), g_engfuncs.pfnGetInfoKeyBuffer( m_pPlayer->edict() ), "model", "goku"); //sets the model for the player	
}

void CGoku::AddAllMagic( void )
{
	m_pPlayer->GiveNamedItem("weapon_melee");
	m_pPlayer->GiveNamedItem("weapon_kiblast");
	m_pPlayer->GiveNamedItem("weapon_kamehameha");
	m_pPlayer->GiveNamedItem("weapon_spiritbomb");
	m_pPlayer->GiveNamedItem("weapon_solarflare");
	m_pPlayer->GiveNamedItem("weapon_sensu");
}