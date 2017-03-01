#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/application.h>
#include <string.h>
#include "spacewar.h"
#include "ship.h"
#include "building.h"
#include "missile.h"

SpaceWar::SpaceWar (CL_ResourceManager *rsc)
{
	resources=rsc;

	// Imagen de fondo
	background = new CL_Surface("background", resources);
	logo = new CL_Surface("Logo", resources);

	// Fuente de caracteres
	font = new CL_Font("font", resources);

	// Semilla "aleatoria"
	srand(CL_System::get_time());		

	// Configuraciones generales del juego
	width=CL_Integer("Configuration/Screen/Width", resources);
	height=CL_Integer("Configuration/Screen/Height",resources);

	// Default options
	options[FULL_SCREEN]=(int)CL_Boolean("Configuration/Screen/FullScreen", resources);
	options[WITH_PLANET]=(int)CL_Boolean("Configuration/Options/Planet", resources);
	options[WITH_GRAVITY]=(int)CL_Boolean("Configuration/Options/Gravity", resources);

	gravityPower=CL_Float("Configuration/Options/GravityPower",resources);

	// Música para la intro
	music=new CL_SoundBuffer("Sound/IntroMusic", resources);
	musicSession=music->prepare();
	musicSession.set_looping(true);
	musicSession.set_volume(0.2);

	// Menu del juego
	menu=new Menu(this,font,rsc);
	
	// Para los mensajes dentro del juego (solo una linea). Se muestran mientras waitFor tenga un valor distinto de 0
	message=new char[100];
	// Decrece hasta 0 si es mayor de 0
	waitFor=0;

	// Inicializa la red
	network=new NetGame();

	quit=false;
	playing=false;

	ship1Wins=0;
	ship2Wins=0;
	
	canPressEscape=true;

	// Empieza con la música
	musicSession.play();

}

SpaceWar::~SpaceWar()
{
	// Delete all gameobjects
	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); )
	{
		switch ((*it)->type) {
		case PLANET:
			delete (Building*)(*it);
			break;
		case SHIP1:
		case SHIP2:
			delete (Ship*)(*it);
			break;
		case MISSILE1:
		case MISSILE2:
			delete (Missile*)(*it);
			break;
		default:
			delete (*it);
		}
		it = objects.erase(it);
	}

	delete background;
	delete logo;
	delete font;
	delete menu;
	delete message;
	delete network;
}

void SpaceWar::initGame ()
{
	int i;

	// Para la música de la intro
	musicSession.set_volume(0.1);

	// Pilla las teclas del resource para todo el juego
	keys[PL1_ENGINE]=		CL_Integer("Configuration/Controls/Player1/Engine", 	resources);
	keys[PL1_ROTATELEFT]=	CL_Integer("Configuration/Controls/Player1/RotateLeft", resources);
	keys[PL1_ROTATERIGHT]=	CL_Integer("Configuration/Controls/Player1/RotateRight",resources);
	keys[PL1_MISSILE]=		CL_Integer("Configuration/Controls/Player1/Missile", 	resources);
	keys[PL1_LASER]=		CL_Integer("Configuration/Controls/Player1/Laser", 		resources);
	keys[PL1_HIDE]=			CL_Integer("Configuration/Controls/Player1/Hide", 		resources);
	keys[PL1_TELEPORT]=		CL_Integer("Configuration/Controls/Player1/Teleport", 	resources);
	keys[PL1_FILLSHIELD]=	CL_Integer("Configuration/Controls/Player1/FillShield", resources);
	keys[PL1_FILLENERGY]=	CL_Integer("Configuration/Controls/Player1/FillEnergy", resources);

	keys[PL2_ENGINE]=		CL_Integer("Configuration/Controls/Player2/Engine", 	resources);
	keys[PL2_ROTATELEFT]=	CL_Integer("Configuration/Controls/Player2/RotateLeft", resources);
	keys[PL2_ROTATERIGHT]=	CL_Integer("Configuration/Controls/Player2/RotateRight",resources);
	keys[PL2_MISSILE]=		CL_Integer("Configuration/Controls/Player2/Missile", 	resources);
	keys[PL2_LASER]=		CL_Integer("Configuration/Controls/Player2/Laser", 		resources);
	keys[PL2_HIDE]=			CL_Integer("Configuration/Controls/Player2/Hide", 		resources);
	keys[PL2_TELEPORT]=		CL_Integer("Configuration/Controls/Player2/Teleport", 	resources);
	keys[PL2_FILLSHIELD]=	CL_Integer("Configuration/Controls/Player2/FillShield", resources);
	keys[PL2_FILLENERGY]=	CL_Integer("Configuration/Controls/Player2/FillEnergy", resources);

//	for (i=0;i<10;i++) {
//		std::cout << "Tecla para " << i << "=" << keys[i] << std::endl; 
//	}

	ship1 = new Ship(SHIP1, this);
	ship1->setPos(width/4, height/4);

	ship2 = new Ship(SHIP2, this);
	ship2->setPos(width - width/4, 3*height/4);
	
	if (options[WITH_PLANET]) {
		planet=new Building(PLANET, this);
		planet->setPos(width/2, height/2);
		addObject(planet);
	}

	addShip(ship1);
	addShip(ship2);

	playing=true;

	showMessage("PEPARADOS!!",5);	// Número de segundos hasta empezar
}

void SpaceWar::termGame ()
{	
	// Vuelve a empezar la música
	musicSession.set_volume(0.7);

	std::list<GameObject *>::iterator it;
	for(it = objects.begin(); it != objects.end(); )
	{
		switch ((*it)->type) {
		case PLANET:
			delete (Building*)(*it);
			break;
		case SHIP1:
		case SHIP2:
			delete (Ship*)(*it);
			break;
		case MISSILE1:
		case MISSILE2:
			delete (Missile*)(*it);
			break;
		default:
			delete (*it);
		}
		it = objects.erase(it);
	}	
	playing=false;

	menu->setSubMenu(MENU_PRINCIPAL);
}

void SpaceWar::addObject(GameObject *object)
{
	objects.push_back(object);
}

void SpaceWar::addShip (Ship *ship)
{
	objects.push_back(ship);
}

bool SpaceWar::hitCheck(CL_CollisionOutline *outline, GameObject *other)
{
	std::list<GameObject *>::iterator it;
	for (it=objects.begin();it!=objects.end();++it) {
		if ((*it)!=other) {
			if ((*it)->hitCheck(outline,other)) {
				return true;
			}
		}
	}	
	return false;
}

void SpaceWar::pollKeys ()
{
	int i;


	if (playing) {
		if (waitFor==0) {
		// SHIP 1
			if (CL_Keyboard::get_keycode(keys[PL1_ENGINE])) {
				ship1->move();
			}
			if (CL_Keyboard::get_keycode(keys[PL1_MISSILE])) {
				if (ship1->canFireMissile) {
					ship1->fireMissile();
					ship1->canFireMissile=false;
				}
			} else {
				ship1->canFireMissile=true;
			}
			if (CL_Keyboard::get_keycode(keys[PL1_LASER])) {
				ship1->fireLaser();
			}
			if (CL_Keyboard::get_keycode(keys[PL1_HIDE])) {
				ship1->hide();
			} else {
				ship1->unHide();
			}
			if (CL_Keyboard::get_keycode(keys[PL1_TELEPORT])) {
				//std::cout << "teleport" << std::endl;
				ship1->teleport();
			}
		}
		if (CL_Keyboard::get_keycode(keys[PL1_ROTATELEFT])) {
			ship1->rotateLeft();
		}
		if (CL_Keyboard::get_keycode(keys[PL1_ROTATERIGHT])) {
			ship1->rotateRight();
		}
		if (CL_Keyboard::get_keycode(keys[PL1_FILLSHIELD])) {
			//std::cout << "fill escudo" << std::endl;
			ship1->fillShield();
			
		}
		if (CL_Keyboard::get_keycode(keys[PL1_FILLENERGY])) {
			//std::cout << "fill energia" << std::endl;
			ship1->fillEnergy();
		}
	
		// SHIP 2
		if (waitFor==0) {
			if (CL_Keyboard::get_keycode(keys[PL2_ENGINE])) {
				ship2->move();
			}
			if (CL_Keyboard::get_keycode(keys[PL2_MISSILE])) {
				if (ship2->canFireMissile) {
					ship2->fireMissile();
					ship2->canFireMissile=false;
					}
			} else {
				ship2->canFireMissile=true;
			}
			if (CL_Keyboard::get_keycode(keys[PL2_LASER])) {
					ship2->fireLaser();
			}
			if (CL_Keyboard::get_keycode(keys[PL2_HIDE])) {
				ship2->hide();
			} else {
				ship2->unHide();
			}
			if (CL_Keyboard::get_keycode(keys[PL2_TELEPORT])) {
				//std::cout << "teleport2" << std::endl;
				ship2->teleport();
			}
		}
		if (CL_Keyboard::get_keycode(keys[PL2_ROTATELEFT])) {
			ship2->rotateLeft();
		}
		if (CL_Keyboard::get_keycode(keys[PL2_ROTATERIGHT])) {
			ship2->rotateRight();
		}
		if (CL_Keyboard::get_keycode(keys[PL2_FILLSHIELD])) {
			//std::cout << "fill escudo2" << std::endl;
			ship2->fillShield();
		}
		if (CL_Keyboard::get_keycode(keys[PL2_FILLENERGY])) {
			//std::cout << "fill energia2" << std::endl;
			ship2->fillEnergy();
		}
	} else {
		if (CL_Keyboard::get_keycode(CL_KEY_UP)) {
			menu->moveUp();
		}
		if (CL_Keyboard::get_keycode(CL_KEY_DOWN)) {
			menu->moveDown();
		}
		if (CL_Keyboard::get_keycode(CL_KEY_ENTER)) {
			menu->select();
		}
	}

	if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE)) {
		if (canPressEscape) {
			if (playing) {
				termGame();
			} else {
				quit=true;
			}
			canPressEscape=false;
		}
	} else {
		canPressEscape=true;
	}

/*
	for (i=0;i<250;i++) {
		if (CL_Keyboard::get_keycode(i)) {
			std::cout << "Tecla " << CL_Keyboard::get_key_name(i) << " (" << i << ") pulsada" << std::endl;
		}
	}
*/
}


void SpaceWar::showMessage (char *message, float waitFor)
{
	strcpy(this->message,message);
	this->waitFor=waitFor;
}


// Envia el estado del juego por red
void SpaceWar::sendStatus ()
{
	std::cout << "Enviando desde " << network->isServer << std::endl;
	network->send();
	return;
}

// Refresca los objetos del juego para el jugador contrario
void SpaceWar::syncStatus (char *packet)
{
	int i;

	if (packet!=0) {
		for (i=0;i<strlen(packet);i++) {
			
		}
	}
}

void SpaceWar::run()
{
	while (!quit) {
		update();
		draw();
		CL_Display::flip();
		CL_System::keep_alive();
		pollKeys();
		if (network->connected && playing) {
			// Enviar paquete de datos
			sendStatus();
		}
	}
}

void SpaceWar::stop ()
{
	quit=true;
}

void SpaceWar::update()
{
	float timeElapsed = calcTimeElapsed();
	GameObjectType whoHasWon=NONE;

	if (playing) {
		// Update all gameobjects
		std::list<GameObject *>::iterator it;
		for(it = objects.begin(); it != objects.end(); ) {
			// If update returns false, object should be deleted
			if ((*it)->update(timeElapsed) == false) {
				if ((*it)->type==SHIP1) {
					whoHasWon=SHIP2;
					ship2Wins++;
				} else if ((*it)->type==SHIP2) {
					whoHasWon=SHIP1;
					ship1Wins++;
				}
				switch ((*it)->type) {
				case PLANET:
					delete (Building*)(*it);
					break;
				case SHIP1:
				case SHIP2:
					delete (Ship*)(*it);
					break;
				case MISSILE1:
				case MISSILE2:
					delete (Missile*)(*it);
					break;
				default:
					delete (*it);
				}
				it = objects.erase(it);
			} else {
				++it;
			}
		}
		if (whoHasWon!=NONE) {
			termGame();
		}
		if (waitFor>0) {
			waitFor-=timeElapsed;
			if (waitFor<0) waitFor=0;
		}	
	} else {
		menu->update(timeElapsed);
	}
	
}

// Calculate amount of time since last frame
float SpaceWar::calcTimeElapsed()
{
	static float lastTime = 0;

	float newTime = (float)CL_System::get_time();
	if(lastTime == 0)
		lastTime = newTime;

	float deltaTime = (newTime - lastTime) / 1000.0f;
	lastTime = newTime;
	
	return deltaTime;
}

void SpaceWar::draw()
{
	// Draw background 
	background->draw();

	if (playing) {
		
		// Draw all gameobjects
		std::list<GameObject *>::iterator it;
		for(it = objects.begin(); it != objects.end(); ++it) {
			(*it)->draw();
		}

		// Draw ships info
		if (ship1->shield<ship1->maxShield/8) {
			if (CL_System::get_time()%1000<500) {
				font->draw(5,height-35, "S");
			}
		} else {
			font->draw(5,height-35, "S");
		}
		font->draw(5,height-15, "E");
		if (ship2->shield<ship2->maxShield/8) {
			if (CL_System::get_time()%1000<500) {
				font->draw(width-5-font->get_width("S"),height-35, "S");
			}
		} else {
			font->draw(width-5-font->get_width("S"),height-35, "S");
		}
		font->draw(width-5-font->get_width("E"),height-15, "E");

		if (ship1->shield>0)
			CL_Display::fill_rect(CL_Rect(20,height-30-1,20+ship1->shield,height-30+1),CL_Color(128,255,128));
		if (ship1->energy>0)
			CL_Display::fill_rect(CL_Rect(20,height-10-1,20+ship1->energy,height-10+1),CL_Color(128,128,255));
		if (ship2->shield>0)
			CL_Display::fill_rect(CL_Rect(width-22-ship2->shield,height-30-1,width-22,height-30+1),CL_Color(128,255,128));
		if (ship2->energy>0)
			CL_Display::fill_rect(CL_Rect(width-22-ship2->energy,height-10-1,width-22,height-10+1),CL_Color(128,128,255));

		// Messages while playing
		if (waitFor!=0) {
			font->draw((int)(width/2-(strlen(message)*8/2.0)),(int)(height/3.0), message);
		}

	} else {

		// Draw logo
		logo->draw(width/2-logo->get_width()/2,height/4-logo->get_height()/2);
	
		// Score
		drawScore(height/6*5);		

		// Menu options
		menu->draw(height/2);

		// Other messages
//		font->draw(width/2-150,(int)(height*2.0/3.0), "PULSA ESPACIO PARA JUGAR");
	}
}

void SpaceWar::drawScore (int h)
{
	std::string p1=CL_String::from_int(ship1Wins)+ " =";
	std::string p2="= "+CL_String::from_int(ship2Wins);
	CL_Sprite p1s=CL_Sprite("Sprites/Ship1",resources);
	CL_Sprite p2s=CL_Sprite("Sprites/Ship2",resources);

	// Draw ships score
	p1s.draw(
		width/3,
		h);
	font->draw(
		width/3-font->get_width(p1)-p1s.get_width()/2,
		h,p1);
	p2s.draw(
		(width/3)*2,
		h);
	font->draw(
		(width/3)*2+p2s.get_width()/2,
		h,p2);
}

