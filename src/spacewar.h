#ifndef _WORLD_H_
#define _WORLD_H_

#include <ClanLib/core.h>
#include <ClanLib/sound.h>
#include <list>

#include "menu.h"
#include "netgame.h"

class CL_Surface;
class CL_InputEvent;
class GameObject;
class Building;
class Ship;
class Menu;

enum {
	PL1_GENERALES=0,
	
	PL1_ENGINE,
	PL1_ROTATELEFT,
	PL1_ROTATERIGHT,
	PL1_MISSILE,
	PL1_LASER,
	PL1_HIDE,
	PL1_TELEPORT,
	PL1_FILLSHIELD,
	PL1_FILLENERGY,

	PL2_ENGINE,
	PL2_ROTATELEFT,
	PL2_ROTATERIGHT,
	PL2_MISSILE,
	PL2_LASER,
	PL2_HIDE,
	PL2_TELEPORT,
	PL2_FILLSHIELD,
	PL2_FILLENERGY
};
	
enum {
	WITH_PLANET=0,
	WITH_GRAVITY,
	FULL_SCREEN,
};
	

class SpaceWar
{
// Construction
public:
	SpaceWar (CL_ResourceManager *rsc);
	~SpaceWar();
	
// Attributes:
public:
	CL_ResourceManager *resources;

	NetGame *network;

	std::list<GameObject *> objects;

	int keys[200];	
	int options[10];

	bool playing;
	
	char *message;
	float waitFor;		// Esperar 0 segudos espera indefinidamente
	
	float gravityPower;
	
	int width;
	int height;
	bool quit;

// Operations:
public:
	void initGame ();
	void termGame ();

	void drawScore (int h);
	
	bool hitCheck (CL_CollisionOutline *outline, GameObject *other);
	
	void addObject (GameObject *object);
	void addShip (Ship *ship);

	void run ();
	void stop ();

// Implementation:
private:
	void draw();
	void update();

	float calcTimeElapsed();

	void pollKeys ();

	void showMessage (char *message,float waitFor=0);

	void syncStatus (char *packet);		// Sincroniza el estado del juego con el paquete que me dan
	void sendStatus ();			// Envia el estado del juego al otro ordenador
	
	Menu *menu;

	CL_Surface *background;
	CL_Surface *logo;
	
	CL_SoundBuffer *music;
	CL_SoundBuffer_Session musicSession;

	CL_Font *font;

	Ship *ship1,*ship2;
	Building *planet;
	
	int ship1Wins;
	int ship2Wins;

	bool canPressEscape;

};

#endif
