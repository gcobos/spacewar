#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "building.h"
#include "spacewar.h"

Building::Building(GameObjectType type, SpaceWar *game) : GameObject(game)
{
	switch(type)
	{
	case PLANET:
		sprite = new CL_Sprite("Sprites/planet", game->resources);
		collisionBuilding = new CL_CollisionOutline("gfx/jupiter.png");		// Cutrada
//		collisionBuilding = new CL_CollisionOutline("Sprites/planet", &game->resources);
		collisionBuilding->set_alignment(origin_center);
		break;
	}	
	this->type=type;

	exploding = false;

	hidden=false;
}

Building::~Building()
{
	delete sprite;
	delete collisionBuilding;
}

void Building::setPos(int x, int y)
{
	posX = (float)x;
	posY = (float)y;
	collisionBuilding->set_translation(x%game->width,y%game->height);
}

void Building::setAngle(float newAngle)
{
	angle = newAngle;
	sprite->set_angle(angle);
	collisionBuilding->set_angle(angle);
}

void Building::draw()
{
	if (!hidden) {
		sprite->draw((int)posX, (int)posY);
	}
}

bool Building::update(float timeElapsed)
{
	sprite->update(timeElapsed);
	return true;
}

bool Building::hitCheck(CL_CollisionOutline *outline, GameObject *other)
{
	return (hypot(other->posX-posX,other->posY-posY)<((height()+width())/2+(other->width()+other->height())/2)/2) && !exploding;
}

