/*
 * Logger.cpp
 *
 *  Created on: Oct 25, 2015
 *      Author: kbp
 */

#include "Logger.h"
#include "iostream"

namespace RobotGame {

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

void Logger::Log(std::string name, std::string msg)
{
	Logger& inst = getLogger();
	inst.queue.enqueue("[" + name + "]: "+msg);
}
void Logger::Log(const Robot* robot, std::string msg)
{
	Log(robot->name(),msg);
}

void Logger::LogHead(std::string msg)
{
	Logger& inst = getLogger();
	inst.queue.enqueue(std::string("**** ") + msg + std::string(" ****"));
}

void Logger::LogError(std::string error)
{
	Logger& inst = getLogger();
	inst.queue.enqueue(std::string("ERROR: ") + error);
}

void Logger::LogWarning(std::string warning)
{
	Logger& inst = getLogger();
	inst.queue.enqueue(std::string("WARNING: ") + warning);
}

void Logger::Runner() {
	while(!goDie) {
		std::string str = queue.dequeue();
		std::cout << str << std::endl;
	}

}
void Logger::DoDie() {
	goDie = true;
}

void Logger::Start() {
	Logger& inst = getLogger();

	inst.Runner();
}

void Logger::End() {
	Logger& inst = getLogger();

	inst.DoDie();
}

// Singleton
Logger& Logger::getLogger() {
	static Logger instance;
	return instance;
}


} /* namespace RobotGame */
