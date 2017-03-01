#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "gameobject.h"

class CL_Sprite;
class CL_SoundBuffer;

class Missile : public GameObject
{
// Construction
public:
	Missile(GameObjectType type, SpaceWar *game, GameObject *owner);
	~Missile();
	
// Attributes
public:
	float deltaX, deltaY;

// Operations
public:
	void setPos (int x, int y);
	void setAngle (float angle);
	void move (float ex,float ey);

	void explode ();

	bool hitCheck (CL_CollisionOutline *outline, GameObject *other);

	virtual void draw ();
	virtual bool update (float timeElapsed);

//	bool operator== (Missile &ref);

// Implementation:
private:
	CL_Sprite *spriteMissile;
	CL_Sprite *spriteExplosion;
	CL_SoundBuffer *soundExplosion;

	CL_CollisionOutline *collisionMissile;
	
	GameObject *owner;
	
	float angle;
	float speed;	

	float speedBase;
	float activeTime;

};

#endif
