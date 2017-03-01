#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

class SpaceWar;
class CL_CollisionOutline;

enum GameObjectType {
	NONE=0,
	PLANET,
	SHIP1,
	SHIP2,
	MISSILE1,
	MISSILE2,
	LASER,
};

class GameObject
{
// Construction
public:
	GameObject (SpaceWar *game) { this->game = game; }

// Attributes
public:
	float posX;
	float posY;

	bool hidden;
	bool exploding;

	GameObjectType type;
	
// Operations
public:
	virtual void draw() {};
	virtual bool update(float timeElapsed) { return false; }
	virtual bool hitCheck(CL_CollisionOutline *outline, GameObject *object) { return false; }

	virtual int width () {  return sprite->get_width();  }
	virtual int height () { return sprite->get_height(); }
	
protected:
	CL_Sprite *sprite;
	SpaceWar *game;
};

#endif
