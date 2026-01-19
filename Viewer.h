/*
 * Viewer.h
 *
 *  Created on: Nov 2, 2015
 *      Author: kbp
 */

#ifndef VIEWER_H_
#define VIEWER_H_

#include <SDL2/SDL.h>
#include "RobEvent.h"
#include "safequeue.h"
#include <map>
#include <mutex>
#include <string>
#include <SDL2/SDL_ttf.h>
#include "Vector2.h"
#include "ShellPool.h"

namespace RobotGame {

class Viewer {
private:
	struct RobPos {
		int x;
		int y;
		unsigned int color;
		int energy;
		int armor;
		SDL_Texture* nameTexture;
		SDL_Texture* energyTexture;
		SDL_Texture* armorTexture;
	};

	struct ShellPos {
		int x1;
		int y1;
		int x2;
		int y2;
		bool blasted;
	};

public:
	Viewer();
	virtual ~Viewer();

	void Runner();

	static void End();
	static void tick(unsigned int tick);
	static void Start();
	static void PostEvent(RobEvent* ev);

	static void RobotShow(int id, int x, int y);
	static void RobotDataShow(int id, std::string name, int armor, int energy);
	static void CannonShow(int id, int x1, int y1, int x2, int y2, bool blasted);
	static void cleanupRobotTextures(int id);
	void drawArenaBorder();
 private:
	static const int maxRobots;

	void DoDie();
	void _RobotShow(int id, int x, int y);
	void _RobotDataShow(int id, std::string name, int armor, int energy);
	void _CannonShow(struct ShellPos shell);
	void ClearArena();
	void RenderArena();
	static unsigned int colors[];
	static Viewer& getViewer();
	std::mutex eventProcess;

	void SetArenaViewPort();
	void PrintRobot(int id);
	// void PrintShell(struct ShellPos shell); // DEPRECATED - replaced with animation system
	void ArenaUpdate(int w, int h);
	void StatusUpdate(int w, int h);
	
	void SetStatusViewPort();
	void ClearStatus();
	void PrintRobotStatus(int id);
	
	// NEW: Animation and rendering system declarations
	SDL_Renderer* createRendererWithVSyncFallback(SDL_Window* window);
	void initializeRenderLayers();
	void renderFrameWithLayers();
	void createExplosionEffect(int x, int y);
	void updateExplosionEffects();

	bool goDie;
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	SDL_Rect arenaViewport;
	SDL_Rect statusViewPort;
	TTF_Font *gFont;

	SafeQueue<RobEvent*> evQueue;
	std::map<int, struct RobPos> robots;
	// std::vector<struct ShellPos> shells; // Replaced with animation system
	
	// NEW: Animation system members
	SDL_Texture* terrainLayer;
	SDL_Texture* robotLayer;
	SDL_Texture* explosionLayer;
	ShellPool* shellPool;
};

} /* namespace RobotGame */
#endif /* VIEWER_H_ */
