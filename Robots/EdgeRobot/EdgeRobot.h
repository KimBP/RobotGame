/*
 * EdgeRobot.h
 *
 *  Created on: 2026-01-12
 *      Author: AI Assistant
 */

#pragma once

#include "../../Robot.h"
#include <string>

namespace RobotGame {

class EdgeRobot: public Robot {
public:
    EdgeRobot(RobCtrl* robCtrl);
    virtual ~EdgeRobot();

    virtual struct tune_t init(std::string& name);
    virtual void run();
    virtual std::string name() const;

private:
    static const struct tune_t myTune;
    static const std::string myName;
    
    // Navigation state
    enum NavigationMode { EDGE_FOLLOW, THREAT_ENGAGE, AVOIDANCE };
    NavigationMode currentMode;
    
    // State tracking variables
    posx_t targetX;
    posy_t targetY;
    angle_t patrolDirection;
    bool threatDetected;
    angle_t threatDirection;
    range_t threatRange;
    bool shootingAtCenter;
    
    // Helper methods
    /** Calculate angle from current position to arena center (4999, 4999) */
    angle_t calculateAngleToCenter();
    
    /** Scan ±10° cone around current heading for threats at 75-100 unit range */
    bool detectThreatInCone(angle_t& threatDirection, range_t& threatRange);
    
    /** Navigate along arena edges while maintaining safe distance from walls */
    void navigateEdges();
    
    /** Engage detected threat by shooting at it */
    void engageThreat(angle_t direction, range_t range);
    
    /** Avoid obstacles while attempting to maintain edge proximity */
    void avoidObstacle();
    
    // Additional navigation helpers
    /** Calculate minimum distance to any arena edge */
    range_t calculateDistanceToNearestEdge(posx_t x, posy_t y);
    
    /** Check if position maintains safe distance (200+ units) from edges */
    bool isSafeFromEdges(posx_t x, posy_t y);
    
    /** Determine optimal direction to follow nearest edge */
    angle_t calculateEdgeFollowDirection(posx_t x, posy_t y, angle_t currentDirection);
};

} /* namespace RobotGame */
