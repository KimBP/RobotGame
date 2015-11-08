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
	getViewer()._ClearArena();
}

void Viewer::RenderArena()
{
	getViewer()._RenderArena();
}

void Viewer::_ClearArena()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );
}

void Viewer::_RenderArena()
{
	//Update screen
	SDL_RenderPresent( gRenderer );
}

void Viewer::RobotShow(int id, int x, int y)
{
	Viewer& inst = getViewer();

	inst._RobotShow(id, x, y);
}

void Viewer::_RobotShow(int id, int x, int y)
{
	if (! robots.count(id)) {
		robots[id].color = 0x00ff00ff;
	}

	robots[id].x = x;
	robots[id].y = y;
	SetArenaViewPort();
	PrintRobot(robots[id].color, x, y);
}

void Viewer::Runner()
{
	while(!goDie) {
		while(!evQueue.empty()) {
			RobEvent* ev = evQueue.dequeue();

			ev->execute();
			delete ev;
		}
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

void Viewer::ArenaUpdate(int w, int h)
{
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

	//Render red filled quad
	SDL_Rect fillRect = { w / 4, h / 4, w / 2, h / 2 };
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
	SDL_RenderFillRect( gRenderer, &fillRect );

	//Render green outlined quad
	SDL_Rect outlineRect = { w / 6, h / 6, w * 2 / 3, h * 2 / 3 };
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	SDL_RenderDrawRect( gRenderer, &outlineRect );

	//Draw blue horizontal line
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
	SDL_RenderDrawLine( gRenderer, 0, ARENA_HEIGHT / 2, ARENA_WIDTH, ARENA_HEIGHT / 2 );

	//Draw vertical line of yellow dots
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );
	for( int i = 0; i < h; i += 4 )
	{
		SDL_RenderDrawPoint( gRenderer, w / 2, i );
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
	SDL_Rect topLeftViewport;
	topLeftViewport.x = STATUS_WIDTH;
	topLeftViewport.y = 0;
	topLeftViewport.w = ARENA_WIDTH;
	topLeftViewport.h = ARENA_HEIGHT;
	SDL_RenderSetViewport( gRenderer, &topLeftViewport );
}

void Viewer::PrintRobot(unsigned int color, int x, int y)
{
	SDL_Rect fillRect = { x-2, y-2, 5, 5 };
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
	SDL_RenderFillRect( gRenderer, &fillRect );

//	SDL_SetRenderDrawColor(gRenderer,
//							(color>>0) & 0xFF,
//							(color>>8) & 0xFF,
//							(color>>16) & 0xFF,
//							(color>>24) & 0xFF);

#define ROBOT_RADIUS 5
//	Draw_FillCircle(gRenderer,
//	                     x, y, ROBOT_RADIUS,
//	                     color);

//	filledCircleColor(gRenderer, x, y, ROBOT_RADIUS, color);
}

}
/* namespace RobotGame */
