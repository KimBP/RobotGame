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

#define SIZEOF_ARRAY(arr)       (sizeof(arr) / sizeof(arr[0]))

const char* plugins[] = {
		"Robots/Scanner/Debug/libScanner.so",
		"Robots/Runner/Debug/libRunner.so",
};

extern "C" {
typedef RobotGame::Robot* ((*getRobotFunc)(RobotGame::RobCtrl* robCtrl));
}

getRobotFunc loadPlugin(const char* plugin)
{
	void* lib_handle;
	lib_handle = dlopen(plugin, RTLD_LAZY);
	if (!lib_handle) {
		std::cout << "Error loading plugin " << plugin << std::endl;
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

int main() {
	std::thread logger(RobotGame::Logger::Start );

	std::thread viewer(RobotGame::Viewer::Start);

	RobotGame::Scheduler& scheduler = RobotGame::Scheduler::getScheduler();


	for (unsigned int i=0; i< SIZEOF_ARRAY(plugins);i++) {
		getRobotFunc fn = loadPlugin(plugins[i]);
		if (NULL == fn) {
			exit(1);
		}
		scheduler.addRobot(fn);
	}

	scheduler.run();

	RobotGame::Logger::End();
	// TODO: join logger + viewer

	return 0;
}
