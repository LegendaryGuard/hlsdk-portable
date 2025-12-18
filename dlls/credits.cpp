#include "extdll.h"
#include "util.h"
#include "decals.h"
#include "credits.h"
bool used = false; //to see if the credits are already playing
static char *CR[] =
{
	"Special Thanks to update HL1 SDK code later 2025:\nLegendGuard\n",
	"2d Artists:\nAndrew 'Mastasurf' Skinner\nJoseph 'pcJoe' Florencio",

	"Mappers:\nTim 'Stryker' Skinner\n Lee 'TheLeakage' Gunselman\n",
	
	"Player Models:\nDennis 'Tarkonian' Lewis\n Eric 'SS_Vegeta' Woll\n Joseph 'pcJoe' Florencio\n",
	
	"Weapon Models:\nDr. Evil\nJoseph 'pcJoe' Florencio\nEric 'SS_Vegeta' Woll",

	"Animator:\nJoseph 'pcJoe' Florencio",
	
	"Web Master:\nAndrew 'Mastasurf' Skinner\nMike 'vt' Wilkinson",
	
	"Sound FX:\nSncShdw\n Herwin 'harSens' van Welbergen",

	"Coder:\nHerwin 'harSens' van Welbergen",
	"Testers:\nJarvis 'Exocode' Derek\nMystic Gohan\nCombat Hamster\n",
	"Special Home Made Brownies:\n Combat Hamster\n", 
	"Thanks to:\nAdrian 'Fireball' LaVallee\nBlake Grant\nrkzad\nwiznut\nCorvidae\nMoOg\nPsychoDude\nGr|m\nIceBryce\nSpInNa\nEastside\nBlobby\nEveryone on the esf forum\nAnd you for reading this\n",	
	"And remember:\n Reality is just a mass-hallucination caused by a lack of alcohol\n"
};

void CCredits::Spawn()
{
	m_iIndex = 0;
	textparms.x = -1;
	textparms.y = -1;
	textparms.effect = 0;
	textparms.r1 = 200;
	textparms.g1 = 200;
	textparms.b1 = 200;
	textparms.a1 = 64;
	textparms.r2 = 200;
	textparms.g2 = 200;
	textparms.b2 = 200;
	textparms.a2 = 64;
	textparms.fadeinTime = 1.0;
	textparms.fadeoutTime = 1.0;
	textparms.holdTime = 3.0;
	textparms.fxTime = 1.0;
	textparms.channel = 1;
}

void CCredits::Think( void )
{
	UTIL_HudMessage(m_pPlayer, textparms, CR[m_iIndex]);
	if (m_iIndex < LAST_CREDIT_INDEX)
	{
		m_iIndex++;
		pev->nextthink = gpGlobals->time + 5.0;
	}
	else
	{
		used = false;
		UTIL_Remove(this);
	}
}

void CCredits::StartCredits(CBasePlayer *player)
{
	if (used) return;
	used = true;
	CCredits *pCredits = GetClassPtr((CCredits *)NULL);
	pCredits->m_pPlayer = player;
	pCredits->Spawn();
	pCredits->SetThink(&Think);
	pCredits->pev->nextthink = gpGlobals->time;
}