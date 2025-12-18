#ifndef AURA_H
#define AURA_H

#include "cbase.h"
#include "effects.h"
#include "gamerules.h"

#define AURA_BLUE_SKIN		0
#define AURA_RED_SKIN		1
#define AURA_YELLOW_SKIN	2
#define AURA_GREEN_SKIN		3
#define AURA_PURPLE_SKIN	4

class CAura : public CBaseMonster
{	
public:
	/**
	* spawns it
	*/
	void Spawn();

	/**
	* precaches the aura
	*/
	void Precache();

	/**
	* removes the aura
	*/
	void Destroy();

	/**
	* aura's thinking function
	*/
	void EXPORT RemoveThink();

	/**
	* set the aura's animation
	*/
	void SetAnimation(const char *animation);

	/**
	* creates an aura
	* @param CBasePlayer *player:the player who should get the aura
	*/
	static CAura *Create(CBasePlayer *pevOwner);
	BOOL m_fHasTrail;
private:
	CSprite *m_pAuraSprite;
	CBasePlayer *m_pPlayer;
	short m_sTrailSprite;	
};
#endif