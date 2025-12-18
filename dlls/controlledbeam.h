#ifndef CONTROLLEDBEAM_H
#define CONTROLLEDBEAM_H

/**
* Controlled Weapon
* @version 27-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CControlledBeam:public CBasePlayerWeapon
{	
public:
	/**
	* precaches sounds/models
	*/
	void Precache();
	
	/**
	* do idle animations, stop increasing magic damage
	*/
	void WeaponIdle( void );

	/**
	* Shoots a attack
	*/
	void PrimaryAttack( void );

	/**
	* Stops aiming
	*/
	void SecondaryAttack( void );

	/**
	* Called when the attack is selected as weapon
	*/
	BOOL Deploy();

	/**
	* Holsters the attack
	* @param int skiplocal: ???
	*/
	void Holster(int skiplocal);

protected:
	/**
	* Silence the sounds for the weapon
	*/
	void SilenceWeaponSounds(){EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);};

	/**
	* Silence the voice
	*/
	void SilenceVoice(){EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "common/null.wav", 1.0, ATTN_NORM);};
		
	/**
	* Emits control sound
	*/
	virtual void EmitControlSound();

	/**
	* Emits charge sound
	*/
	virtual void EmitChargeSound(){};

	/**
	* Emits shoot sound
	*/
	virtual void EmitShootSound(){};

	/**
	* Inits charge sprite
	*/
	virtual void InitChargeSprite(){};

	/**
	* Enable charge sprite
	*/
	virtual void EnableChargeSprite(){};

	/**
	* Disable charge sprite
	*/
	virtual void DisableChargeSprite(){};

	/**
	* Let the chargesprite grow
	*/
	virtual void GrowChargeSprite(){};

	/**
	* Remove charge sprite
	*/
	virtual void RemoveChargeSprite(){};

	/**
	* Stops Aiming the attack
	*/
	void StopAiming(void);

	/**
	* Shoots the attack
	*/
	void Shoot(void);

	/**
	* Charges the attack
	*/
	void Charge(void);

	enum {ATTACK_IDLE,ATTACK_CHARGE,ATTACK_AIM}m_eAttackState;
	float m_flStartShoot;			//shot starting time
	CMagicAttack *m_pMagic;			//the magic attack created after firing
	CSprite *m_pBeamStart;			//the beam start sprite
	short m_sTrail;					//trail sprite
	unsigned char m_pColor[3];		//color for the explosion
	int m_iCharge;					//how far is it charged
	int m_iMaxCharge;				//max charge for this attack
	int m_iSpeed;					//the speed of the attack
	int m_iKiCost;					//ki needed for 1 chargepoint
	int m_iDamage;					//damage for 1 chargepoint
	int m_iRadius;					//radius for the attack
	char m_pAttackName[255];		//name of the attack
};
#endif