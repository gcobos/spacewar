#ifndef _BUILDING_H_
#define _BUILDING_H_

#include "gameobject.h"

class CL_Sprite;
class CL_SoundBuffer;

class Building : public GameObject
{
// Enums
public:

// Construction
public:
	Building(GameObjectType building_type, SpaceWar *game);
	~Building();
	
// Attributes
public:
	
// Operations
public:
	void setPos(int x, int y);
	void setAngle(float angle);

	virtual bool hitCheck (CL_CollisionOutline *outline, GameObject *other);

	void draw();
	bool update(float timeElapsed);


// Implementation:
private:
	CL_CollisionOutline *collisionBuilding;
	
	float angle;	
	float posX;
	float posY;
};

#endif
