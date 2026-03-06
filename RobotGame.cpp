/*
 * RobotGame.cpp
 *
 *  Created on: Oct 24, 2015
 *      Author: kbp
 */



#include <string>
#include <iostream>
#include <vector>
#include <dlfcn.h>
#include "Scheduler.h"
#include "Logger.h"
#include <thread>
#include "Viewer.h"
#include <unistd.h>
#include "Trigonometry.h"

#define SIZEOF_ARRAY(arr)       (sizeof(arr) / sizeof(arr[0]))

extern "C" {
typedef RobotGame::Robot* ((*getRobotFunc)(RobotGame::RobCtrl* robCtrl));
}

getRobotFunc loadPlugin(const char* plugin)
{
	void* lib_handle;
	lib_handle = dlopen(plugin, RTLD_LAZY);
	if (!lib_handle) {
		std::cout << "Error loading plugin " << plugin << " error : " << dlerror() << std::endl;
		return NULL;
	}

	getRobotFunc fn = reinterpret_cast<getRobotFunc>(dlsym(lib_handle, "getRobot"));
	char* error = dlerror();
	if (error != NULL) {
		std::cout << "Error finding getRobot() in " << plugin << std::endl;
		return NULL;
	}
	return fn;
}

int main(int argc, char** argv) {
	// Default battle delay: 16ms (60 FPS)
	int battleDelayMs = 16;
	bool stepMode = false;
	
	// Parse command-line arguments and collect robot plugins
	std::vector<std::string> robotPlugins;
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		
		if (arg == "--help" || arg == "-h") {
			std::cout << "Usage: " << argv[0] << " [options] robot1.so [robot2.so ...]\n"
					  << "Options:\n"
					  << "  -d, --delay <ms>    Battle delay in milliseconds (1-1000, default: 16)\n"
					  << "  --step              Enable step mode (pause after each tick, press Enter to advance)\n"
					  << "  -h, --help          Show this help message\n";
			return 0;
		}
		else if ((arg == "--delay" || arg == "-d") && i + 1 < argc) {
			try {
				battleDelayMs = std::stoi(argv[++i]);
				if (battleDelayMs < 1 || battleDelayMs > 1000) {
					std::cerr << "Error: Delay must be between 1 and 1000 milliseconds" << std::endl;
					return 1;
				}
				std::cout << "Battle delay set to " << battleDelayMs << "ms" << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "Error: Invalid delay value" << std::endl;
				return 1;
			}
		}
		else if (arg == "--step") {
			stepMode = true;
			std::cout << "Step mode enabled. Press Enter to advance each tick." << std::endl;
		}
		else {
			// This is a robot plugin
			robotPlugins.push_back(arg);
		}
	}

std::thread logger(RobotGame::Logger::Start );
	
	RobotGame::Trigonometry::test();
	
	std::thread viewer(RobotGame::Viewer::Start);
	
	RobotGame::Scheduler& scheduler = RobotGame::Scheduler::getScheduler();
	scheduler.setBattleDelay(battleDelayMs);
	scheduler.setStepMode(stepMode);  // Enable step mode if --step flag was provided
	
	// Load robot plugins
	for (const auto& plugin : robotPlugins) {
		getRobotFunc fn = loadPlugin(plugin.c_str());
		if (NULL == fn) {
			exit(1);
		}
		scheduler.addRobot(fn);
	}

	scheduler.run();

	RobotGame::Viewer::End();
	RobotGame::Logger::End();

	logger.join();
	viewer.join();

	return 0;
}
