#ifndef TRAIL_H
#define TRAIL_H

struct TrailList
{
	Vector vecPoint;
	TrailList *next;
};

/**
* Magic trails
* @version 16-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CTrail : public CBaseEntity
{	
public:
	/**
	* Creates a trail
	* 
	*/
	static CTrail *CreateTrail(Vector start, int m_iTarget, short sprite, int width);

	/**
	* spawns the trail
	*/
	void Spawn();

	/**
	* Draws the trail
	*/
	void EXPORT DrawThink(void);
	
	/**
	* adds another point to the trail
	* @param Vector point: point to add
	*/
	void AddPoint(Vector point);

	/**
	* Removes the trail
	*/
	void Remove(void);
private:
	TrailList *m_pTrailList;	//list of points for the trail
	short m_sTrailSprite;
	int m_iWidth;
	int m_iTarget;
};
#endif