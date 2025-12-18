#ifndef CLASSES_H
#define CLASSES_H

#include "aura.h"
#include "gamerules.h"

#define PC_BASE		-1
#define PC_PICCOLO	0
#define PC_GOKU		1
#define PC_VEGETA	2
#define PC_KRILLIN	3
#define PC_TRUNKS	4
#define PC_FRIEZA	5
#define PC_BUU		6
#define PC_GOHAN	7
#define PC_CELL		8
 
#define PC_LAST		9 //for use in loops etc.

/**
* Player base class 
* @version 19-4-2001
* @author Herwin 'harSens' van Welbergen
*/
class CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CBaseClass(CBasePlayer *player);
	
	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 200; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 100; }

	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 100000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 100000; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void) {}
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify(void) { return PC_BASE; }

	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }

	/**
	* get the name of the player class
	* @return const char *: the player class name
	*/
	static const char *GetName(int id);

protected:
	CBasePlayer *m_pPlayer;	
};

/**
* player class Piccolo
* @author Herwin 'harSens' van Welbergen
* @version 27-4-2001
*/
class CPiccolo : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CPiccolo(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 250; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 500000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 750000; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 120; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_PICCOLO; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

/**
* player class Goku
* @author Herwin 'harSens' van Welbergen
* @version 27-4-2001
*/
class CGoku : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CGoku(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 300; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 1000000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 1000000; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 110; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_GOKU; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

/**
* player class Vegeta
* @author Herwin 'harSens' van Welbergen
* @version 27-4-2001
*/
class CVegeta : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CVegeta(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 300; }
	
	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 110; }

	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 750000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 750000; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_VEGETA; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

/**
* player class krillin
* @author Herwin 'harSens' van Welbergen
* @version 27-4-2001
*/
class CKrillin : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CKrillin(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 150; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 150000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 250000; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 75; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_KRILLIN; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

/**
* player class trunks
* @author Herwin 'harSens' van Welbergen
* @version 1-5-2001
*/
class CTrunks : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CTrunks(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 250; }
	
	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 110; }

	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 750000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 800000; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_TRUNKS; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

/**
* player class frieza
* @author Herwin 'harSens' van Welbergen
* @version 12-5-2001
*/
class CFrieza : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CFrieza(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 250; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 400000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 500000; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 150; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_FRIEZA; }
	virtual int GetAuraSkin(void) { return AURA_PURPLE_SKIN; }
};

/**
* player class cell
* @author Herwin 'harSens' van Welbergen
* @version 12-5-2001
*/
class CCell : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CCell(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 300; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 500000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 750000; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 90; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic();
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_CELL; };
	virtual int GetAuraSkin(void) { return AURA_GREEN_SKIN; }
};

/**
* player class buu
* @author Herwin 'harSens' van Welbergen
* @version 12-5-2001
*/
class CBuu : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CBuu(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 250; }
	
	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 100; }

	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 1000000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 750000; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_BUU; };
	virtual int GetAuraSkin(void) { return AURA_PURPLE_SKIN; }
};

/**
* player class gohan
* @author Herwin 'harSens' van Welbergen
* @version 1-5-2001
*/
class CGohan : public CBaseClass
{
public:
	/**
	* constructor
	* @param CBasePlayer *player: the player to whom the class refers
	*/
	CGohan(CBasePlayer *player);

	/**
	* gets the speed for this class
	* @return int: the maximum speed
	*/
	virtual int	GetStartSpeed(void) { return 250; }

	/** 
	* gets the health for this class
	* @return int: the maximum health
	*/
	virtual int GetStartHealth(void) { return 90; }
	
	/**
	* gets the maximum ki for this class
	* @return int: the maximum ki
	*/
	virtual int GetStartKi(void) { return 200000; }

	/**
	* gets the maximum powerlevel for this class
	* @return int: the maximum powerlevel
	*/
	virtual int GetStartPowerLevel(void) { return 750000; }

	/**
	* add all magic this class has to the player
	*/
	virtual void AddAllMagic(void);
	
	/**
	* get the player class identifier
	* @return int: the player class identifier
	*/
	virtual int Classify() { return PC_GOHAN; }
	virtual int GetAuraSkin(void) { return AURA_BLUE_SKIN; }
};

#endif