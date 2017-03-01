#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "missile.h"
#include "spacewar.h"

Missile::Missile (GameObjectType type, SpaceWar *game, GameObject *_owner) : GameObject (game)
{
	speedBase = CL_Float("Configuration/Ships/Missile/SpeedBase", game->resources);
	activeTime = CL_Float("Configuration/Ships/Missile/ActiveTime", game->resources);

	soundExplosion = new CL_SoundBuffer("Sound/Explosion", game->resources);
	spriteExplosion = new CL_Sprite("Sprites/Explosion", game->resources);

	switch(type)
	{
	case MISSILE1:
		spriteMissile = new CL_Sprite("Sprites/Missile1", game->resources);
		collisionMissile = new CL_CollisionOutline("gfx/missile1.png",0,accuracy_raw);
		collisionMissile->set_alignment(origin_center);
		break;

	case MISSILE2:
		spriteMissile = new CL_Sprite("Sprites/Missile2", game->resources);
		collisionMissile = new CL_CollisionOutline("gfx/missile1.png",0,accuracy_raw);
		collisionMissile->set_alignment(origin_center);
		break;
	}

	this->type=type;

	sprite = spriteMissile;
	
	hidden = false;
	exploding = false;

	deltaX=0;
	deltaY=0;

	owner = _owner;
}

Missile::~Missile()
{
	delete soundExplosion;
	delete spriteExplosion;
	delete spriteMissile;
	delete collisionMissile;
}

/*
bool Missile::operator== (Missile &ref)
{
	return (type==ref.type && posX==ref.posX && posY==ref.posY);

}
*/

void Missile::setPos(int x, int y)
{
	posX = (float)x;
	posY = (float)y;
	collisionMissile->set_translation(x,y);
}

void Missile::setAngle(float newAngle)
{
	angle = newAngle;
	sprite->set_angle(angle);
	collisionMissile->set_angle(angle);
}

// Establece los deltas para el movimiento del misil
void Missile::move (float ex,float ey)
{	
	// Calcula el desplazamiento basado en ex,ey y en el angulo del misil
	deltaX=ex+sin((angle/180.0f*M_PI)) * speedBase;
	deltaY=ey-cos((angle/180.0f*M_PI)) * speedBase;
}

void Missile::explode ()
{
	soundExplosion->play();
	sprite=spriteExplosion;
	sprite->set_angle(rand()%360);
	sprite->set_alpha(0.8f);
	exploding=true;
	deltaX=0;
	deltaY=0;
}

bool Missile::hitCheck(CL_CollisionOutline *outline, GameObject *other)
{
	if (other->type!=this->type) {
		return collisionMissile->collide(*outline) && !exploding;
	} else {
		return false;
	}
}

void Missile::draw()
{
	if (!hidden) sprite->draw((int)posX,(int)posY);
}

bool Missile::update(float timeElapsed)
{
	std::list<GameObject *>::iterator it;
	float ex,ey,a;

	sprite->update(timeElapsed);
	if (exploding) {
		if (sprite->is_finished()) {
			return false;
		}
	} else {
	
		// Apply the gravity vector if needs
		if (game->waitFor==0 && game->options[WITH_GRAVITY]) {
			// Saca las componentes del vector normal al planeta, o centro de la pantalla
			a=atan2(posY-game->height/2,posX-game->width/2)+M_PI_2;
			
			ex=cos(a+M_PI_2)*game->gravityPower;
			ey=sin(a+M_PI_2)*game->gravityPower;

			// Suma el vector gravedad (ex,ey) al de avance (deltaX,deltaY)	
			deltaX=deltaX+ex;
			deltaY=deltaY+ey;
		}

		posX+=deltaX*timeElapsed;
		posY+=deltaY*timeElapsed;


		if (posX<0) posX+=game->width;
	
		if (posX>game->width) posX-=game->width;

		if (posY<0) posY+=game->height;

		if (posY>game->height) posY-=game->height;	

		collisionMissile->set_translation(posX,posY);

		activeTime-=timeElapsed;

		if (activeTime<=0) {
			explode();
		}
		
		for(it = game->objects.begin(); it != game->objects.end(); ++it) {
			if((*it) != this) {
				if ((*it)->hitCheck(collisionMissile, this)) {
					switch ((*it)->type) {
					case PLANET:
						explode();
						break;
					case MISSILE1:
					case MISSILE2:
						if ((*it)->type!=this->type) {
							explode();
							((Missile*)(*it))->explode();
						}
						break;
					}
				}
			}
		}
	}
	return true;
}




