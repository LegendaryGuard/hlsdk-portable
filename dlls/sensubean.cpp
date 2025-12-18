#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "gamerules.h"

extern int gmsgSensuBean;

/**
* Bean
* @version 25-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CBean: public CBaseMonster
{
public:
	/**
	* Spawns the ball
	*/
	void Spawn();

	/**
	* Precaches the models
	*/
	void Precache();

	void Touch(CBaseEntity *pOther);

	/**
	* Throws a bean
	* @param Vector origin: the origin of the bean
	* @param Vector velocity: the velocity of the bean
	*/
	static CBean *ThrowBean(entvars_t *pevOwner,Vector origin, Vector velocity);
};
LINK_ENTITY_TO_CLASS(bean, CBean);


void CBean::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_sensubean.mdl");
	UTIL_SetSize(pev, Vector(-8, -8, -8),Vector(8, 8, 8));
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_TRIGGER;	
}

void CBean::Precache()
{
	PRECACHE_MODEL("models/w_sensubean.mdl");
}

void CBean::Touch(CBaseEntity *pOther)
{
	if (pOther->edict() == pev->owner)
		return;

	if (pOther->IsPlayer())
	{
		if (!pev->owner)
			((CBasePlayer*)pOther)->SetAnimation(PLAYER_CATCHBEAN);

		if( ((CBasePlayer*)pOther)->GiveBean(1))
			UTIL_Remove(this);
	}
	pev->owner = NULL;
	pev->velocity = pev->velocity * 0.5;
}

CBean *CBean::ThrowBean(entvars_t *pevOwner,Vector vecStart, Vector vecVelocity)
{
	CBean *pBean = GetClassPtr((CBean *)NULL);
	pBean->Spawn();
	pBean->pev->owner = ENT(pevOwner);

	UTIL_SetOrigin(pBean->pev,vecStart);
	pBean->pev->velocity = vecVelocity;

	pBean->pev->gravity = 0.5;

	return pBean;
}

/**
* Sensubean "Weapon"
* @version : 22-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CSensuBean:public CBasePlayerWeapon
{	
public:
	/**
	* Init function
	*/
	void Spawn();

	/**
	* Precaches sprites and sounds
	*/
	void Precache();

	/**
	* Get sensubean slot
	* return int: the slot, is 9 for sensubean
	*/
	int iItemSlot( void ) { return 9; }

	/**
	* Get sensubean item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* Called when sensubean is selected as weapon
	*/
	BOOL Deploy(void);

	/**
	* idle
	*/
	void WeaponIdle(void);

	/**
	* Eats a sensubean
	*/
	void PrimaryAttack(void);

	/**
	* Throws a sensubean
	*/
	void SecondaryAttack(void);
private:
	int m_fEating;
	int m_fThrowing;
};

LINK_ENTITY_TO_CLASS(weapon_sensu, CSensuBean);

void CSensuBean::Spawn(void)
{
	Precache();
	m_iId = WEAPON_SENSUBEAN;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	FallInit(); // get ready to fall down.
}

void CSensuBean::Precache()
{
	UTIL_PrecacheOther("bean");
}

int CSensuBean::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 8;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_SENSUBEAN;
	p->iWeight = SENSUBEAN_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CSensuBean::Deploy(void)
{
	m_fEating = false;
	m_fThrowing = false;
	return DefaultDeploy("bean");
}

void CSensuBean::WeaponIdle(void)
{
	m_fEating = false;
	m_fThrowing = false;
}

void CSensuBean::PrimaryAttack(void)
{
	if (!m_fEating)
	{
		if (m_pPlayer->TakeBean())
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		m_fEating = true;
	}
}

void CSensuBean::SecondaryAttack(void)
{
	if (!m_fThrowing)
	{
		m_fThrowing = true;
		if (m_pPlayer->m_iSensuBeans>0)
		{
			UTIL_MakeVectors(m_pPlayer->pev->v_angle);
			Vector vecThrow = gpGlobals->v_forward * SENSU_SPEED;

			m_pPlayer->SetAnimation(PLAYER_ATTACK2);
			CBean::ThrowBean(m_pPlayer->pev, m_pPlayer->GetGunPosition(), vecThrow);
			//reduce beans
			m_pPlayer->m_iSensuBeans--;
			//update HUD
			MESSAGE_BEGIN(MSG_ONE, gmsgSensuBean, NULL, m_pPlayer->pev);
				WRITE_BYTE(m_pPlayer->m_iSensuBeans);
			MESSAGE_END();
		}
	}
}