/*
 * Viewer.cpp
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#include "Viewer.h"
#include "Logger.h"
#include "Scheduler.h"
#include "Explosion.h"
#include "ShellPool.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <chrono>

namespace RobotGame {

unsigned int Viewer::colors[] = {
		0xFFF000FF,
		0xFF0F00FF,
		0xFF00F0FF,
		0xFF000FFF
};

const int Viewer::maxRobots = 4;

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

void Viewer::End() {
	Viewer& inst = getViewer();

	inst.DoDie();
}

void Viewer::DoDie() {
	goDie = true;
}

void Viewer::PostEvent(RobEvent* ev)
{
	Logger::Log("Event", ev->getName() + std::string("(") +
				std::to_string(ev->getId()) + std::string(")"));
	getViewer().evQueue.enqueue(ev);
}

void Viewer::tick(unsigned int tick)
{
	getViewer().eventProcess.unlock();
}

void Viewer::ClearArena()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );
}

void Viewer::CannonShow(int id, int x1, int y1, int x2, int y2, bool blasted)
{
	struct ShellPos shell;
	shell.x1 = x1/POS_TO_MAP_SCALE;
	shell.x2 = x2/POS_TO_MAP_SCALE;
	shell.y1 = y1/POS_TO_MAP_SCALE;
	shell.y2 = y2/POS_TO_MAP_SCALE;
	shell.blasted = blasted;
	getViewer()._CannonShow(shell);
}
void Viewer::_CannonShow(struct ShellPos shell)
{
	// Create explosion at shell end position if blasted
	if (shell.blasted) {
		Vector2 explosionPos(static_cast<float>(shell.x2), static_cast<float>(shell.y2));
		createExplosionEffect(static_cast<int>(explosionPos.x), static_cast<int>(explosionPos.y));
	}
	
	// Create animated shell with velocity
	Vector2 startPos(static_cast<float>(shell.x1), static_cast<float>(shell.y1));
	Vector2 endPos(static_cast<float>(shell.x2), static_cast<float>(shell.y2));
	Vector2 velocity = endPos - startPos;
	velocity = velocity.normalized() * 300.0f; // Shell speed: 300 pixels per second
	
	// Get current time for animation
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	uint32_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	
	// Create animated shell
	shellPool->createShell(startPos, velocity, currentTime);
}

void Viewer::RobotShow(int id, int x, int y)
{
	getViewer()._RobotShow(id, x, y);
}

void Viewer::_RobotShow(int id, int x, int y)
{
	if (id >= maxRobots)
		return;

	if (! robots.count(id)) {
		robots[id].color = colors[id];
		robots[id].nameTexture = 0;
	}

	robots[id].x = x/POS_TO_MAP_SCALE;
	robots[id].y = y/POS_TO_MAP_SCALE;
}

void Viewer::RobotDataShow(int id, std::string name, int armor, int energy)
{
	getViewer()._RobotDataShow(id, name, armor, energy);
}

void Viewer::_RobotDataShow(int id, std::string name, int armor, int energy)
{
	SDL_Color col = {
			static_cast<Uint8>((robots[id].color >> 24) & 0xff),
			static_cast<Uint8>((robots[id].color >> 16) & 0xff),
			static_cast<Uint8>((robots[id].color >> 8) & 0xff),
			static_cast<Uint8>((robots[id].color >> 0) & 0xff)
	};
	if (!robots[id].nameTexture) {
		SDL_Surface* surface = TTF_RenderText_Solid( gFont, name.c_str(), col );

		robots[id].nameTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

		surface = TTF_RenderText_Solid( gFont, "E: ", col);
		robots[id].energyTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

		surface = TTF_RenderText_Solid( gFont, "A: ", col);
		robots[id].armorTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
		SDL_FreeSurface(surface);

	}
	robots[id].armor = armor;
	robots[id].energy = energy;
}

void Viewer::Runner()
{
	while(!goDie) {

		eventProcess.lock();
		Logger::LogDebug(std::string("Queue has ") + std::to_string(evQueue.size()) + std::string(" element(s)"));
		while (!evQueue.empty()) {
			RobEvent* ev = evQueue.dequeue();

			ev->execute();
			delete ev;
		}
		// Update animations
		updateExplosionEffects();
		
		// Update shell pool with current time
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		uint32_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		shellPool->updateShells(currentTime);
		
		// Render using layered compositing
		renderFrameWithLayers();
		
		SDL_RenderPresent( gRenderer );

		SetStatusViewPort();
		ClearStatus();
		for (unsigned int i=0; i < robots.size(); i++) {
			PrintRobotStatus(i);
		}
		SDL_RenderPresent( gRenderer );

		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if( event.type == SDL_QUIT )
			{
				goDie = true;
			}
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

	//Create renderer for window with VSync fallback
	Logger::LogDebug("Initializing renderer with VSync fallback system...");
	gRenderer = createRendererWithVSyncFallback(gWindow);
	if( gRenderer == NULL ) {
		Logger::LogError("Failed to create any renderer - game cannot start");
		goDie = true;
		return;
	}
	
	// Log successful renderer creation for debugging
	SDL_RendererInfo rendererInfo;
	if (SDL_GetRendererInfo(gRenderer, &rendererInfo) == 0) {
		std::string rendererType = "Unknown";
		if (rendererInfo.flags & SDL_RENDERER_SOFTWARE) {
			rendererType = "Software";
		} else if (rendererInfo.flags & SDL_RENDERER_ACCELERATED) {
			rendererType = "Hardware Accelerated";
		}
		
		bool vsyncEnabled = (rendererInfo.flags & SDL_RENDERER_PRESENTVSYNC) != 0;
		Logger::LogDebug(std::string("Renderer created successfully: ") + rendererType + 
			std::string(" | VSync: ") + (vsyncEnabled ? "Enabled" : "Disabled") +
			std::string(" | Max Texture: ") + std::to_string(rendererInfo.max_texture_width) + 
			"x" + std::to_string(rendererInfo.max_texture_height));
	} else {
		Logger::LogWarning("Could not retrieve renderer information for debugging");
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

	// font handling
	TTF_Init();
	gFont = TTF_OpenFont( "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 18 );

	// Initialize animation system
	shellPool = new ShellPool();
	initializeRenderLayers();
	
	// Wait for scheduler to make us run
	eventProcess.lock();
}

SDL_Renderer* Viewer::createRendererWithVSyncFallback(SDL_Window* window) {
	// Try VSync + accelerated renderer first
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	if (renderer) {
		Logger::LogDebug("Created hardware accelerated renderer with VSync");
		return renderer;
	}
	
	// Log VSync failure with detailed error
	std::string vsyncError = SDL_GetError();
	Logger::LogWarning(std::string("VSync renderer failed, attempting accelerated fallback: ") + vsyncError);
	
	// Fallback: accelerated without VSync
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer) {
		Logger::LogWarning("Created hardware accelerated renderer without VSync - screen tearing may occur");
		return renderer;
	}
	
	// Log accelerated failure with detailed error
	std::string accelError = SDL_GetError();
	Logger::LogError(std::string("Accelerated renderer failed, attempting software fallback: ") + accelError);
	
	// Final fallback: software renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (renderer) {
		Logger::LogWarning("Created software renderer (performance may be limited, no hardware acceleration)");
		return renderer;
	}
	
	// All attempts failed - log detailed error information
	std::string softwareError = SDL_GetError();
	Logger::LogError(std::string("All renderer creation attempts failed!") +
		std::string(" | VSync Error: ") + vsyncError +
		std::string(" | Accelerated Error: ") + accelError + 
		std::string(" | Software Error: ") + softwareError);
	
	return NULL;
}

Viewer::~Viewer() {
	while (!evQueue.empty()) {
		RobEvent* ev = evQueue.dequeue();
		delete ev;
	}
	
	// Cleanup animation system
	delete shellPool;
	shellPool = nullptr;
	
	// Cleanup render layers
	if (terrainLayer) {
		SDL_DestroyTexture(terrainLayer);
		terrainLayer = nullptr;
		Logger::LogDebug("Terrain layer cleaned up");
	}
	if (robotLayer) {
		SDL_DestroyTexture(robotLayer);
		robotLayer = nullptr;
		Logger::LogDebug("Robot layer cleaned up");
	}
	if (explosionLayer) {
		SDL_DestroyTexture(explosionLayer);
		explosionLayer = nullptr;
		Logger::LogDebug("Explosion layer cleaned up");
	}
	
	// Clear all explosion state
	::clearAllExplosions();
	Logger::LogDebug("All animation systems cleaned up");
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



void Viewer::PrintRobot(int id)
{
#define ROBOT_RADIUS 5


	filledCircleColor(gRenderer, robots[id].x, robots[id].y, ROBOT_RADIUS, robots[id].color);
}

void Viewer::SetStatusViewPort()
{
	statusViewPort.x = 0;
	statusViewPort.y = 0;
	statusViewPort.w = STATUS_WIDTH;
	statusViewPort.h = ARENA_HEIGHT;
	SDL_RenderSetViewport( gRenderer, &statusViewPort );
}

void Viewer::ClearStatus()
{
	// Black - clear
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_RenderClear( gRenderer );

}
void Viewer::PrintRobotStatus(int id)
{
	int robStatusHeight = ARENA_HEIGHT / maxRobots;
	int robStatusY      = id * robStatusHeight;

	const int blockHeight = 40;
	const int prefixWidth = 40;

	SDL_Rect viewPort;
	viewPort.x = 0;
	viewPort.y = robStatusY;
	viewPort.w = STATUS_WIDTH;
	viewPort.h = robStatusHeight;
	SDL_RenderSetViewport(gRenderer, &viewPort);

	SDL_SetRenderDrawColor( gRenderer,
			(robots[id].color >> 24)& 0xFF, (robots[id].color >> 16) & 0xFF,
			(robots[id].color >> 8) & 0xFF, robots[id].color & 0xFF);

	SDL_Rect nameRect;
	nameRect.x = 1;
	nameRect.y = 1;
	nameRect.w = STATUS_WIDTH-2;
	nameRect.h = 2*blockHeight-2;
	if (SDL_RenderCopy( gRenderer, robots[id].nameTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering name");
	}

	nameRect.x = 1;
	nameRect.y = 2*blockHeight + 1;
	nameRect.w = prefixWidth - 2;
	nameRect.h = blockHeight - 2;
	if (SDL_RenderCopy( gRenderer, robots[id].energyTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering energy");
	}

	nameRect.x = 1;
	nameRect.y = 3*blockHeight + 1;
	nameRect.w = prefixWidth - 2;
	nameRect.h = blockHeight - 2;
	if (SDL_RenderCopy( gRenderer, robots[id].armorTexture, 0, &nameRect)) {
		Logger::LogDebug("Error rendering energy");
	}

	int energy = robots[id].energy * (STATUS_WIDTH - prefixWidth) / MAX_ENERGY;
	int armor = robots[id].armor * (STATUS_WIDTH - prefixWidth) / MAX_ARMOR;
	SDL_RenderDrawLine(gRenderer, prefixWidth, 2.5*blockHeight, 40+energy, 2.5*blockHeight);
	SDL_RenderDrawLine(gRenderer, prefixWidth, 3.5*blockHeight, 40+armor, 3.5*blockHeight);
}

// Explosion animation system implementation
void Viewer::createExplosionEffect(int x, int y) {
	Vector2 position(static_cast<float>(x), static_cast<float>(y));
	::createExplosion(position); // Call global function from Explosion.cpp
}

void Viewer::updateExplosionEffects() {
	::updateExplosions(); // Call global function from Explosion.cpp
}

void Viewer::initializeRenderLayers() {
	// Create terrain layer
	terrainLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, ARENA_WIDTH, ARENA_HEIGHT);
	if (!terrainLayer) {
		Logger::LogError(std::string("Failed to create terrain layer texture: ") + SDL_GetError());
	} else {
		Logger::LogDebug("Terrain layer created successfully");
	}
	
	// Create robot layer
	robotLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, ARENA_WIDTH, ARENA_HEIGHT);
	if (!robotLayer) {
		Logger::LogError(std::string("Failed to create robot layer texture: ") + SDL_GetError());
	} else {
		Logger::LogDebug("Robot layer created successfully");
	}
	
	// Create explosion layer
	explosionLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, ARENA_WIDTH, ARENA_HEIGHT);
	if (!explosionLayer) {
		Logger::LogError(std::string("Failed to create explosion layer texture: ") + SDL_GetError());
	} else {
		Logger::LogDebug("Explosion layer created successfully");
		// Set blend mode for explosion layer
		if (SDL_SetTextureBlendMode(explosionLayer, SDL_BLENDMODE_ADD) != 0) {
			Logger::LogWarning(std::string("Failed to set explosion layer blend mode: ") + SDL_GetError());
		}
	}
}

void Viewer::renderFrameWithLayers() {
	// Initialize render layers if not already done
	if (!terrainLayer || !robotLayer || !explosionLayer) {
		initializeRenderLayers();
		if (!terrainLayer || !robotLayer || !explosionLayer) {
			// Fallback to direct rendering if layer creation failed
			SetArenaViewPort();
			ClearArena();
			for (unsigned int i = 0; i < robots.size(); i++) {
				PrintRobot(i);
			}
			return;
		}
	}
	
	// Get current time for animations
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	uint32_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	
	// === RENDER TERRAIN LAYER ===
	SDL_SetRenderTarget(gRenderer, terrainLayer);
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(gRenderer);
	
	// === RENDER ROBOT LAYER ===
	SDL_SetRenderTarget(gRenderer, robotLayer);
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);
	
	// Render robots on robot layer
	for (unsigned int i = 0; i < robots.size(); i++) {
		PrintRobot(i);
	}
	
	// === RENDER EXPLOSION LAYER ===
	SDL_SetRenderTarget(gRenderer, explosionLayer);
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(gRenderer);
	
	// Render explosions
	::renderExplosions(gRenderer);
	
	// === COMPOSITE LAYERS ===
	SDL_SetRenderTarget(gRenderer, nullptr);
	
	// Clear main render target
	SetArenaViewPort();
	ClearArena();
	
	// Composite layers in correct order
	SDL_Rect renderRect = {0, 0, ARENA_WIDTH, ARENA_HEIGHT};
	SDL_RenderCopy(gRenderer, terrainLayer, nullptr, &renderRect);
	SDL_RenderCopy(gRenderer, robotLayer, nullptr, &renderRect);
	SDL_RenderCopy(gRenderer, explosionLayer, nullptr, &renderRect);
	
	// Render shells directly (they use their own rendering system)
	shellPool->renderShells(gRenderer);
}

}
/* namespace RobotGame */
