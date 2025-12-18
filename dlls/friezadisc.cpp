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
/**
* Frieza's Destructo Disc Weapon
* @version 15-6-2001
* @author Herwin 'harSens' van Welbergen
*/
class CFriezaDisc:public CBasePlayerWeapon
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
	* Get frieza disc slot
	* return int: the slot, is 3 for frieza disc
	*/
	int iItemSlot(void ) { return 4; }

	/**
	* Get destructodisc item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* called whenever the weapon is idle
	*/
	void WeaponIdle(void);

	/**
	* Shoots a frieza disc
	*/
	void PrimaryAttack(void );

	/**
	* Homes the disc(s)
	*/
	void SecondaryAttack(void );

	/**
	* Holsters the disc
	*/
	void Holster(int skiplocal);

	/**
	* Called when frieza is selected as weapon
	*/
	BOOL Deploy();

private:
	/**
	* Creates the frieza disc
	*/
	void CreateDisc(void);

	/**
	* Stops aiming the frieza disc
	*/
	void StopControl(void);
	enum { DISK_NOTCREATED, DISK_IDLE, DISK_SHOOTING, DISK_RETURNING} m_eDiscStatus;
	CMagicAttack *m_pMagic;
};

LINK_ENTITY_TO_CLASS(weapon_friezadisc, CFriezaDisc);

void CFriezaDisc::Spawn(void )
{
	Precache();
	m_iId = WEAPON_FRIEZADISC;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "models/w_friezadisc.mdl");
	m_iClip = -1;
	FallInit();// get ready to fall down.
}

void CFriezaDisc::Precache(void )
{
	PRECACHE_MODEL("models/w_friezadisc.mdl");
	PRECACHE_MODEL("models/v_friezadisc.mdl");
	PRECACHE_SOUND("weapons/rotating.wav");
}

int CFriezaDisc::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_FRIEZADISC;
	p->iWeight = FRIEZADISC_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CFriezaDisc::CreateDisc(void)
{
	// reduce our ammo only on creation, cause the disc in boomeranging
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > FRIEZADISC_KI_COST && !m_pPlayer->m_fBlock && !m_pPlayer->m_fPowerUp)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= FRIEZADISC_KI_COST;
		m_eDiscStatus = DISK_IDLE;
		m_pPlayer->pev->weaponmodel = MAKE_STRING("models/v_friezadisc.mdl");
		m_pPlayer->m_flSlowDown = 0.5;
		m_pPlayer->m_fHoldDisc = true;		
	}
}

BOOL CFriezaDisc::Deploy(void )
{
	m_eDiscStatus = DISK_NOTCREATED;
	return DefaultDeploy("friezadisc" );	
}

void CFriezaDisc::Holster(int skiplocal=0)
{
	if (m_eDiscStatus == DISK_RETURNING || m_eDiscStatus == DISK_SHOOTING)
		m_pMagic->SetThink(&CMagicAttack::NormalThink);

	if (m_eDiscStatus == DISK_SHOOTING)
		StopControl();

	if (m_eDiscStatus == DISK_IDLE)
	{
		m_pPlayer->pev->weaponmodel = 0;
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fHoldDisc = false;		
	}
}

void CFriezaDisc::WeaponIdle(void)
{
	Vector vecHome;
	Vector vecDir;
	switch (m_eDiscStatus)
	{
	case DISK_RETURNING:
		vecHome = m_pPlayer->GetGunPosition();
		vecDir = m_pMagic->pev->origin-vecHome;
		if (vecDir.Length() < 75)
		{
			//returned home
			m_pMagic->Remove();
			m_eDiscStatus = DISK_IDLE;
			m_pPlayer->pev->weaponmodel = MAKE_STRING("models/v_friezadisc.mdl");
			m_pPlayer->m_flSlowDown = 0.5;
			m_pPlayer->m_fHoldDisc = true;			
		}
		break;
	case DISK_NOTCREATED:
		CreateDisc();
		break;
	case DISK_SHOOTING:
		if (!UTIL_IsValidEntity(ENT(m_pMagic->pev)))
		{	StopControl();
			m_eDiscStatus=DISK_NOTCREATED;			
		}
		break;
	case DISK_IDLE:
		//do idle animation stuff here
		break;
	default:
		break;
	}
}

void CFriezaDisc::StopControl()
{
	m_pPlayer->m_fControl = false;
	m_pPlayer->m_flSlowDown = 1.0;
}

void CFriezaDisc::PrimaryAttack()
{
	//can't shoot if we have already launced a disc
	if (m_eDiscStatus != DISK_IDLE)
		return;

	m_eDiscStatus = DISK_SHOOTING;

	/*char shoot_sound[255];
	sprintf (shoot_sound,"%s/friezadisc.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);*/

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + Vector(0, 0, 25);

	//hackhack: set speed higher for a short while to get friezadisc speed
	m_pPlayer->m_flSlowDown = 1.0;	
	Vector vecThrow = gpGlobals->v_forward * FRIEZADISC_SPEED;
	m_pMagic = CMagicAttack::ShootFriezaDisc(m_pPlayer->pev, vecSrc, vecThrow);

	//kick in shoot animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	//remove disc
	m_pPlayer->pev->weaponmodel = 0;

	//player in control mode
	m_pPlayer->m_fControl = true;
	m_pPlayer->m_fHoldDisc = false;
	m_pPlayer->m_flSlowDown = 0.0;	
}

void CFriezaDisc::SecondaryAttack()
{
	if (m_eDiscStatus == DISK_SHOOTING)
	{
		m_pPlayer->m_fHoldDisc = true;
		StopControl();
		m_pMagic->SetThink(&CMagicAttack::HomeThink);
		m_eDiscStatus = DISK_RETURNING;
	}
}