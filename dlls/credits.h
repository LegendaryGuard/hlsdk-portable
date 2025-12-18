#ifndef CREDITS_H
#define CREDITS_H

#include "cbase.h"
#include "classes.h"
#include "effects.h"
#include "aura.h"
#include "player.h"
#define LAST_CREDIT_INDEX 11

/**
* Credit playing class
* @author Herwin 'harSens' van Welbergen
* @version 8-5-2001
*/
class CCredits:public CBaseEntity
{	
public:
	/**
	* Initialize
	*/
	void Spawn();

	/**
	* Used to change the credits to next credit string
	*/
	void Think( void );

	/**
	* Starts playing credits
	* @param CBasePlayer *player: the player who will see the credits
	*/
	static void StartCredits(CBasePlayer *player);
private:
	int m_iIndex;				//index in credit string
	hudtextparms_t textparms;	//parameters for text display
	CBasePlayer *m_pPlayer;		//the player who will see the credits
};
#endif