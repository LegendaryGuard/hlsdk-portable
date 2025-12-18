#ifndef CAPTURETHEDRAGONBALLS_H
#define CAPTURETHEDRAGONBALLS_H

#include "dragonball.h"


/**
* Capture the dragonballs gamerules
* @version 16-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CHalfLifeCaptureTheDragonBalls : public CHalfLifeTeamplay
{
public:
	/**
	* Constructor
	*/
	CHalfLifeCaptureTheDragonBalls();

	/**
	* Think function
	*/
	void Think();

	CDragonBall *m_pDragonBalls[7];
private:
	/**
	* searches places to spawn the dragonballs
	*/
	void SpawnDragonBalls();
	int m_fSpawnedBalls;
};

#endif
