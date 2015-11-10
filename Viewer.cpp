/*
 * Viewer.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "Viewer.h"
#include "Logger.h"
#include "Scheduler.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

//#include <stdlib.h>

namespace RobotGame {

unsigned int Viewer::colors[] = {
		0xFFF000FF,
		0xFF0F00FF,
		0xFF00F0FF,
		0xFF000FFF
};

#define ARENA_WIDTH 1000
#define ARENA_HEIGHT 1000
#define STATUS_WIDTH 400
#define STATUS_HEIGHT 400
#define SCREEN_WIDTH  (ARENA_WIDTH + STATUS_WIDTH)
#define SCREEN_HEIGHT (std::max (ARENA_HEIGHT,STATUS_HEIGHT))

#define POS_TO_MAP_SCALE	(10) // Divide all x,y position with 10
// Singleton
Viewer& Viewer::getViewer() {
	static Viewer instance;
	return instance;
}


void Viewer::Start() {
	getViewer().Runner();
}

void Viewer::PostEvent(RobEvent* ev)
{
	getViewer().evQueue.enqueue(ev);
}

void Viewer::ClearArena()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );
}


void Viewer::RobotShow(int id, int x, int y)
{
	getViewer()._RobotShow(id, x, y);
}

void Viewer::_RobotShow(int id, int x, int y)
{
	if (id >= 4)
		return;

	if (! robots.count(id)) {
		robots[id].color = colors[id];
	}

	robots[id].x = x/POS_TO_MAP_SCALE;
	robots[id].y = y/POS_TO_MAP_SCALE;
}

void Viewer::Runner()
{
	while(!goDie) {
		do {
			RobEvent* ev = evQueue.dequeue();

			ev->execute();
			delete ev;
		} while(!evQueue.empty());

		SetArenaViewPort();
		ClearArena();

		for (unsigned int i=0; i < robots.size(); i++) {
			PrintRobot(robots[i].color, robots[i].x, robots[i].y);
		}
		SDL_RenderPresent( gRenderer );

		SDL_PollEvent(0);
		Scheduler::end();
	}
}


Viewer::Viewer()
: goDie(false)
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		Logger::LogError("Viewer - Init fail - exiting");
		goDie = true;
		return;
	}
	//Set texture filtering to linear
	if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
		Logger::LogWarning( "Linear texture filtering not enabled!" );
	}

	//Create window
	gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL ) {
		Logger::LogError( std::string("Window could not be created! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}

	//Create renderer for window
	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gRenderer == NULL )	{
		Logger::LogError(std::string("Renderer could not be created! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}
	//Initialize renderer color
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		Logger::LogError(std::string("SDL_image could not initialize! SDL Error: ") + SDL_GetError() );
		goDie = true;
		return;
	}
}

Viewer::~Viewer() {
	while (!evQueue.empty()) {
		RobEvent* ev = evQueue.dequeue();
		delete ev;
	}
}

void Viewer::StatusUpdate(int w, int h)
{
	//Render red filled quad
	SDL_Rect fillRect = { 0, 0, w, h };
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_RenderFillRect( gRenderer, &fillRect );

}

void Viewer::SetArenaViewPort()
{
	arenaViewport.x = STATUS_WIDTH;
	arenaViewport.y = 0;
	arenaViewport.w = ARENA_WIDTH;
	arenaViewport.h = ARENA_HEIGHT;
	SDL_RenderSetViewport( gRenderer, &arenaViewport );
}

void Viewer::PrintRobot(unsigned int color, int x, int y)
{
#define ROBOT_RADIUS 5

//	SDL_Rect fillRect = { x-2, y-2, 5, 5 };
//	SDL_SetRenderDrawColor(gRenderer,
//							(color>>0) & 0xFF,
//							(color>>8) & 0xFF,
//							(color>>16) & 0xFF,
//							(color>>24) & 0xFF);
//	SDL_RenderFillRect( gRenderer, &fillRect );

	filledCircleColor(gRenderer, x, y, ROBOT_RADIUS, color);
}

}
/* namespace RobotGame */
