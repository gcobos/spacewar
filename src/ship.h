#ifndef _SHIP_H_
#define _SHIP_H_

#include <ClanLib/sound.h>
#include "gameobject.h"

class CL_Sprite;
class CL_SoundBuffer;
class CL_SoundBuffer_Session;

class Ship : public GameObject
{
// Enums
public:
	
// Construction
public:
	Ship (GameObjectType type, SpaceWar *game);
	~Ship ();

// Attributes:
public:
	float deltaX, deltaY;

	bool canFireMissile;
	bool teleporting;

	int maxEnergy, maxShield;
	int energy;
	int shield;

// Operations:
public:
	void move ();
	void rotateLeft ();
	void rotateRight ();
	void fireMissile ();
	void fireLaser ();

	void hide ();
	void unHide ();

	void explode ();

	void teleport ();

	void fillShield ();
	void fillEnergy ();

	void setAngle(float angle);
	void getPos(int &x, int &y);
	void setPos(float x, float y);
	
	bool hitCheck(CL_CollisionOutline *outline, GameObject *other);

	virtual void draw();
	virtual bool update(float timeElapsed);


// Implementation:
private:

	CL_Sprite *spriteBody;
	CL_CollisionOutline *collisionBody;
	CL_SoundBuffer *soundMissileFire;
	CL_SoundBuffer *soundLaserFire;
	CL_SoundBuffer *soundExplosion;
	CL_SoundBuffer *soundTeleport;
	CL_SoundBuffer *soundAlarm;
	CL_SoundBuffer_Session soundAlarmSession;

	CL_Sprite *spriteLaser;
	CL_Sprite *spriteExplosion;
	CL_Sprite *spriteTeleport;
	CL_CollisionOutline *collisionLaser;

	float bodyAngle;
	GameObjectType missileType;

	float rotateAngle;
	float rotateReloadTime, rotatedSince;
	float fillReloadTime, filledSince;
	float hideReloadTime, hiddenSince;
	float energyReloadTime, energyReloadSince;
	float engineReloadTime, engineMovingSince;
	float engineIncrement;
	float engineMaxPower;
	int engineUnitCost, hideUnitCost, teleportUnitCost;

	float missileReloadTime, missileFiredSince;
	int maxMissilesFired;
	int missileDamage;
	int missileUnitCost;

	int laserLength;
	float laserReloadTime, laserFiredSince;
	int laserDamage;
	int laserUnitCost;	

	// Para teleport
	int newX, newY;
	float posXorig, posYorig;
	
	// Para saber la posición anterior
	float posXant, posYant;

	// Para evitar que las naves se enganchen al colisionar
	float distance, distanceAnt;
};

#endif
