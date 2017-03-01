#include <ClanLib/display.h>
#include <ClanLib/sound.h>

#include "ship.h"
#include "spacewar.h"
#include "missile.h"

Ship::Ship (GameObjectType type, SpaceWar *game) : GameObject(game)
{
	rotateAngle=CL_Float("Configuration/Ships/Rotate/Angle",game->resources);
	rotateReloadTime=CL_Float("Configuration/Ships/Rotate/ReloadTime",game->resources);
	fillReloadTime=CL_Float("Configuration/Ships/fillReloadTime",game->resources);
	energyReloadTime=CL_Float("Configuration/Ships/EnergyReloadTime",game->resources);
	engineIncrement=CL_Float("Configuration/Ships/Engine/Increment",game->resources);
	engineMaxPower=CL_Float("Configuration/Ships/Engine/MaxPower",game->resources);
	engineReloadTime=CL_Float("Configuration/Ships/Engine/ReloadTime",game->resources);
	engineUnitCost=CL_Integer("Configuration/Ships/Engine/UnitCost",game->resources);
	hideReloadTime=CL_Float("Configuration/Ships/Hide/ReloadTime",game->resources);
	hideUnitCost=CL_Integer("Configuration/Ships/Hide/UnitCost",game->resources);
	teleportUnitCost=CL_Integer("Configuration/Ships/TeleportUnitCost",game->resources);

	maxMissilesFired = CL_Integer("Configuration/Ships/Missile/MaxMissiles", game->resources);
	missileReloadTime = CL_Float("Configuration/Ships/Missile/ReloadTime", game->resources);
	missileDamage = CL_Integer("Configuration/Ships/Missile/Damage", game->resources);
	missileUnitCost = CL_Integer("Configuration/Ships/Missile/UnitCost", game->resources);

	laserLength = CL_Integer("Configuration/Ships/Laser/Length", game->resources);
	laserReloadTime = CL_Float("Configuration/Ships/Laser/ReloadTime", game->resources);
	laserDamage = CL_Integer("Configuration/Ships/Laser/Damage", game->resources);
	laserUnitCost = CL_Integer("Configuration/Ships/Laser/UnitCost", game->resources);

	spriteLaser = new CL_Sprite("Sprites/Laser", game->resources);
	spriteExplosion = new CL_Sprite("Sprites/Explosion", game->resources);
	spriteTeleport = new CL_Sprite("Sprites/Teleport", game->resources);

	collisionLaser = new CL_CollisionOutline("gfx/laser.png",accuracy_raw);
	collisionLaser->set_alignment(origin_bottom_center);
	collisionLaser->set_rotation_hotspot(origin_bottom_center);

	switch(type)
	{
	case SHIP1:
		spriteBody = new CL_Sprite("Sprites/Ship1", game->resources);
		collisionBody = new CL_CollisionOutline("gfx/ship10.png");
		collisionBody->set_alignment(origin_center);
		missileType=MISSILE1;
		break;

	case SHIP2:
		spriteBody = new CL_Sprite("Sprites/Ship2", game->resources);
		collisionBody = new CL_CollisionOutline("gfx/ship8.png");
		collisionBody->set_alignment(origin_center);
		missileType=MISSILE2;
		break;
	}

	this->type=type;

	soundMissileFire = new CL_SoundBuffer("Sound/Missile", game->resources);
	soundLaserFire = new CL_SoundBuffer("Sound/Laser", game->resources);
	soundExplosion = new CL_SoundBuffer("Sound/Explosion", game->resources);
	soundTeleport = new CL_SoundBuffer("Sound/Teleport", game->resources);
	soundAlarm = new CL_SoundBuffer("Sound/Alarm", game->resources);
	soundAlarmSession=soundAlarm->prepare();
	soundAlarmSession.set_looping(true);
	soundAlarmSession.set_volume(0.5);

	bodyAngle=0.0f;

	sprite=spriteBody;

	rotatedSince=rotateReloadTime;
	filledSince=fillReloadTime;
	missileFiredSince=missileReloadTime;
	laserFiredSince=laserReloadTime;
	engineMovingSince=engineReloadTime;
	hiddenSince=hideReloadTime;
	energyReloadSince=0;

	canFireMissile=true;

	maxEnergy = CL_Integer("Configuration/Ships/MaxEnergy", game->resources);
	maxShield = CL_Integer("Configuration/Ships/MaxShield", game->resources);
	energy = CL_Integer("Configuration/Ships/Energy", game->resources);
	shield = CL_Integer("Configuration/Ships/Shield", game->resources);

	deltaX=0.0f;
	deltaY=0.0f;

	exploding = false;
	teleporting = false;

}

Ship::~Ship ()
{
	soundAlarmSession.stop();

	delete spriteLaser;
	delete spriteExplosion;
	delete spriteTeleport;
	delete collisionLaser;
	delete spriteBody;
	delete collisionBody;
	delete soundMissileFire;
	delete soundLaserFire;
	delete soundExplosion;
	delete soundTeleport;
	delete soundAlarm;
}


void Ship::rotateLeft ()
{
	if (exploding || teleporting) {
		return;
	}

	if (rotatedSince>=rotateReloadTime) {
		bodyAngle = bodyAngle - rotateAngle;
		if(bodyAngle < -180.0f) {
			bodyAngle += 360.0f;
		}
		collisionBody->set_angle(bodyAngle);
		rotatedSince=0;
	}
}

void Ship::rotateRight ()
{
	if (exploding || teleporting) {
		return;
	}

	if (rotatedSince>=rotateReloadTime) {
		bodyAngle = bodyAngle + rotateAngle;
		if(bodyAngle > 180.0f)
		{
			bodyAngle -= 360.0f;
		}
		collisionBody->set_angle(bodyAngle);
		rotatedSince=0;
	}
}

void Ship::move ()
{
	float ex,ey,a;

	if (exploding || teleporting) {
		return;
	}

	if (energy<engineUnitCost) {
		return;
	}

	// Quita energía si le toca 
	if (engineMovingSince>=engineReloadTime) {
		energy-=engineUnitCost;
		engineMovingSince=0;
	}

	// Saca las componentes cartesianas del vector engine(bodyAngle)
	a=(bodyAngle * M_PI) / 180.0f;

	ex=cos(a-M_PI_2) * engineIncrement;
	ey=sin(a-M_PI_2) * engineIncrement;

	// Suma el vector incremento (ex,ey) al de avance (deltaX,deltaY)	
	if (hypot(deltaX+ex,deltaY+ey)<=engineMaxPower) {	
		deltaX=deltaX+ex;
		deltaY=deltaY+ey;
	}
	
	// Si estoy al 100% de velocidad, reducela a 95% para poder maniobrar
	if (hypot(deltaX+ex,deltaY+ey)>=engineMaxPower) {
		deltaX*=0.95;
		deltaY*=0.95;
	}
}

void Ship::fireMissile ()
{
	float a;
	
	std::list<GameObject *>::iterator it;
	int missilesFired=0;

	if (exploding || teleporting) {
		return;
	}

	if (energy<missileUnitCost) {
		return;
	}
	
	// Comprueba que el numero de misiles disparados sea menor del maximo permitido
	for (it=game->objects.begin(); it!=game->objects.end(); ++it) {
		if ((*it)->type==missileType) {
			if (((Ship*)(*it))->exploding==false) missilesFired++;
		}
	}
	if (missileFiredSince>=missileReloadTime && missilesFired<maxMissilesFired) {
		soundMissileFire->play();
		Missile *missile = new Missile(missileType,game,this);
		missile->setPos(
			(int)(posX + (hypot(posX-posXant,posY-posYant)+width()*5.0/6.0)* float(sin(bodyAngle * M_PI / 180.0f))),
			(int)(posY + (hypot(posX-posXant,posY-posYant)+height()*5.0/6.0)* float(-cos(bodyAngle * M_PI / 180.0f))));

		missile->setAngle(bodyAngle);
		missile->move(deltaX,deltaY);

		game->addObject(missile);

		missileFiredSince=0;
		energy-=missileUnitCost;

	}
}

void Ship::fireLaser ()
{
	if (exploding || teleporting) {
		return;
	}

	if (energy<laserUnitCost) {
		return;
	}

	if (laserFiredSince>=laserReloadTime) {
		soundLaserFire->play();
		spriteLaser->set_angle(bodyAngle);
		laserFiredSince=0;
		energy-=laserUnitCost;
	}
}

void Ship::hide ()
{
	if (exploding || teleporting) {
		return;
	}

	if (energy<hideUnitCost) {
		return;
	}

	// Quita energía si le toca 
	if (hiddenSince>=hideReloadTime) {
		energy-=hideUnitCost;
		hiddenSince=0;
	}

	hidden=true;
}

void Ship::unHide ()
{
	if (exploding || teleporting) {
		return;
	}

	hidden=false;
}

void Ship::teleport ()
{
	std::list<GameObject *>::iterator it;
	bool posOk;

	if (exploding || teleporting) {
		return;
	}

	if (energy<teleportUnitCost) {
		return;
	}

	soundTeleport->play();
	sprite=spriteTeleport;
	sprite->set_angle(rand()%360);
	sprite->set_scale(1.0f,1.0f);
	sprite->set_alpha(0.7f);
	energy-=teleportUnitCost;
	teleporting=true;

	// Comprueba que la posición a la que salte, no esté cerca de ninguna nave
	do {
		newX=rand()%game->width;
		newY=rand()%game->height;
		posOk=true;
		for (it=game->objects.begin(); it!=game->objects.end();) {
			if ((*it)->type==SHIP1 || (*it)->type==SHIP2) {
				if (hypot(((Ship*)(*it))->posX-newX,((Ship*)(*it))->posY-newY)<((Ship*)(*it))->sprite->get_width()) {
					posOk=false;
					break;
				}
			}
			++it;
		}
	} while (!posOk);
	
	// Para el avance de la nave
	deltaX=(newX-posX)/sprite->get_frame_count();
	deltaY=(newY-posY)/sprite->get_frame_count();
	
	posXorig=posX;
	posYorig=posY;
}

void Ship::fillShield ()
{
	if (exploding) {
		return;
	}

	if (energy>0 && shield<maxShield) {
		if (filledSince>=fillReloadTime) {
			energy--;
			shield++;
			filledSince=0;
		}
	}
}

void Ship::fillEnergy ()
{
	if (exploding) {
		return;
	}

	if (shield>0 && energy<maxEnergy) {
		if (filledSince>=fillReloadTime) {
			energy++;
			shield--;
			filledSince=0;
		}
	}	
}

void Ship::explode ()
{
	if (exploding || teleporting) {
		return;
	}

	hidden=false;
	soundExplosion->set_volume(1.0f);
	soundExplosion->play();
	sprite=spriteExplosion;
	sprite->set_angle(rand()%360);
	sprite->set_scale(2.0f,2.0f);
	sprite->set_alpha(0.8f);
	exploding=true;
}

void Ship::setPos(float x, float y)
{
	posX=x;
	posY=y;
	posXant=posX;
	posYant=posY;

	collisionBody->set_translation((int)x%game->width,(int)y%game->height);
}

void Ship::setAngle(float angle)
{
	bodyAngle = angle;
	collisionBody->set_angle(angle);
}

bool Ship::hitCheck(CL_CollisionOutline *outline, GameObject *other)
{
	return collisionBody->collide(*outline) && !exploding && !teleporting;
}

bool Ship::update(float timeElapsed)
{
	std::list<GameObject *>::iterator it;
	float ex,ey,a;

	sprite->update(timeElapsed);
	
	if (rotatedSince<=rotateReloadTime) rotatedSince+=timeElapsed;
	if (missileFiredSince<=missileReloadTime) missileFiredSince+=timeElapsed;
	if (laserFiredSince<=laserReloadTime) laserFiredSince+=timeElapsed;
	if (engineMovingSince<=engineReloadTime) engineMovingSince+=timeElapsed;
	if (hiddenSince<=hideReloadTime) hiddenSince+=timeElapsed;
	if (filledSince<=fillReloadTime) filledSince+=timeElapsed;

	// Alarma al quedar menos del 12.5% del escudo
	if (shield<maxShield/8 && shield>=0) {
		if (!soundAlarmSession.is_playing()) {
			soundAlarmSession.play();
		}
	} else {
		soundAlarmSession.stop();
	}

	if (exploding) {
		if (sprite->is_finished()) {
			return false;
		}
	} else if (teleporting) {
		posX=posXorig+deltaX*sprite->get_current_frame();
		posY=posYorig+deltaY*sprite->get_current_frame();

		//std::cout << "Teleport!!" << std::endl;
		if (sprite->is_finished()) {	
			sprite->restart();
			setPos(newX,newY);
			sprite=spriteBody;
			teleporting=false;
			deltaX=0;
			deltaY=0;
		}		
	} else {
	
		// Comprueba la vida de la nave
		if (shield<0) {
			explode();
		}
	
		// Recarga de energía
		if (energyReloadSince<=energyReloadTime) {
			energyReloadSince+=timeElapsed;
		} else {
			energyReloadSince-=energyReloadTime;
			if (energy<maxEnergy && energy>0) energy++;
		}

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

		posXant=posX;
		posYant=posY;
	
		posX+=deltaX*timeElapsed;
		posY+=deltaY*timeElapsed;
	
		if (posX<0) posX+=game->width;
	
		if (posX>game->width) posX-=game->width;

		if (posY<0) posY+=game->height;

		if (posY>game->height) posY-=game->height;

		sprite->set_angle(bodyAngle);	
		collisionBody->set_angle(bodyAngle);
		collisionBody->set_translation(posX,posY);

		spriteLaser->set_angle(bodyAngle);
		collisionLaser->set_angle(bodyAngle);
		
		// Comprueba colisiones y reacciona
		for(it = game->objects.begin();it!=game->objects.end(); ++it) {
			if((*it) != this) {
				if ((*it)->hitCheck(collisionBody, this)) {
					switch ((*it)->type) {
					case PLANET:
						//std::cout << "Colision con planeta!!!" << std::endl;
						shield-=int(maxShield*timeElapsed*10);
						break;
					case SHIP1:
					case SHIP2:
						//std::cout << "Cambia de " << posX << "," << posY << " a " << lastGoodX << "," << lastGoodY << std::endl;

						// Coloco las dos naves a una distancia prudencial (sin colision)
						
						// Saca el punto medio entre las dos naves						
						
						//posX=lastGoodX;
						//posY=lastGoodY;
						
						//((Ship*)(*it))->posX=((Ship*)(*it))->lastGoodX;
						//((Ship*)(*it))->posY=((Ship*)(*it))->lastGoodY;
						
						distance=hypot(posX-((Ship*)(*it))->posX,posY-((Ship*)(*it))->posY);
						
						// Intercambian los desplazamientos en negativo si su distancia es menor a la anterior
						if (distance<distanceAnt) {
							//std::cout << "Inversion!!!" << std::endl;
							float tempDeltaX,tempDeltaY;
							tempDeltaX=this->deltaX;
							tempDeltaY=this->deltaY;
							
							this->deltaX=((Ship*)(*it))->deltaX;
							this->deltaY=((Ship*)(*it))->deltaY;
							((Ship*)(*it))->deltaX=tempDeltaX;
							((Ship*)(*it))->deltaY=tempDeltaY;
						}
						break;
					case MISSILE1:
					case MISSILE2:
						//std::cout << "misil contra nave!!!" << std::endl;
						this->shield-=missileDamage;
						((Missile*)(*it))->explode();
						break;
					default:
						std::cout << "Colision de la nave con algo!!!" << std::endl;
					}
				}
				if (laserFiredSince<laserReloadTime && laserFiredSince<=timeElapsed) {
					// Laser collision checks
					int i;
					float laserX,laserY;
					for (i=0;i<laserLength;i+=4) {
						laserX=posX + (i + sprite->get_height()/2) * float(sin(bodyAngle * PI / 180.0f));
						laserY=posY + (i + sprite->get_height()/2) * float(-cos(bodyAngle * PI / 180.0f));
						if (laserX<0) laserX+=game->width;
						if (laserX>game->width) laserX-=game->width;
						if (laserY<0) laserY+=game->height;
						if (laserY>game->height) laserY-=game->height;
						collisionLaser->set_translation(laserX,laserY);
						
						//std::cout << "Comprobando laser en " << (*it)->posX << " sobre " << laserX << std::endl;
	
						if ((*it)->hitCheck(collisionLaser, this)) {
							//std::cout << "collision!" << std::endl;
							switch ((*it)->type) {
							case SHIP1:
							case SHIP2:
								((Ship*)(*it))->shield-=laserDamage;
								break;
							case MISSILE1:
							case MISSILE2:
								((Missile*)(*it))->explode();
									break;
							case PLANET:
								//std::cout << "Laser contra planeta" << std::endl;
								break;
							default:
								std::cout << "Laser contra algo!" << std::endl;
							}
							break;
						}
					}
				}
			}
		}
	}
	
	distanceAnt=distance;
		
	return true;
}

void Ship::draw()
{
	if (!hidden) {
		sprite->draw((int)posX, (int)posY);
	}

	// Draw the laser if exists
	if (laserFiredSince<laserReloadTime/4) {
		int i;
		float laserX,laserY;
		for (i=0;i<laserLength;i+=4) {
			laserX=posX + (i + sprite->get_height()/2) * float(sin(bodyAngle * PI / 180.0f));
			laserY=posY + (i + sprite->get_height()/2) * float(-cos(bodyAngle * PI / 180.0f));
			if (laserX<0) laserX+=game->width;
			if (laserX>game->width) laserX-=game->width;
			if (laserY<0) laserY+=game->height;
			if (laserY>game->height) laserY-=game->height;
			spriteLaser->draw(laserX,laserY);
		}
	}	
}	


	
