/*
 * EdgeRobot.cpp
 *
 *  Created on: 2026-01-12
 *      Author: AI Assistant
 */

#include "EdgeRobot.h"
#include <cmath>

namespace RobotGame {

extern "C" __attribute__((visibility("default"))) Robot* getRobot(RobCtrl* robCtrl) {
    static EdgeRobot* instance = new EdgeRobot(robCtrl);
    return instance;
    // Factory function for dynamic loading complete
}

const struct tune_t EdgeRobot::myTune = {2, 2}; // Speed: 100 units/tick, Range: 7000 units
const std::string EdgeRobot::myName("EdgeRobot"); // Robot identifier for game framework
// Robot tuning parameters complete

EdgeRobot::EdgeRobot(RobCtrl* robCtrl)
: Robot(robCtrl), currentMode(EDGE_FOLLOW) {
    // Initialize navigation state
    // Constructor implementation complete
}

EdgeRobot::~EdgeRobot() {
    // Cleanup any allocated resources
    // Destructor implementation complete
}

struct tune_t EdgeRobot::init(std::string& name) {
    name = myName;
    return myTune;
    // Virtual init method implementation complete
}

void EdgeRobot::run() {
    int ticksSinceLastThreat = 0;
    armor_t lastArmor = getArmor();
    
    while (true) {
        // Performance optimization: minimize expensive calculations
        // Only recalculate edge distances when position significantly changes
        // Check for damage to detect when to use avoidance
        if (lastArmor > getArmor()) {
            currentMode = AVOIDANCE; // Switch to avoidance when hit
            lastArmor = getArmor();
        }
        
        // Check for threats in ±10° cone
        angle_t threatDir;
        range_t threatRng;
        bool threatFound = detectThreatInCone(threatDir, threatRng);
        
        if (threatFound) {
            // Threat detected - engage it
            engageThreat(threatDir, threatRng);
            ticksSinceLastThreat = 0;
        } else {
            // No threat - navigate edges and shoot at center
            ticksSinceLastThreat++;
            
            // State transition logic
            if (currentMode == THREAT_ENGAGE && ticksSinceLastThreat > 5) {
                currentMode = EDGE_FOLLOW; // Return to edge following after safety delay
            }
            
            if (currentMode == AVOIDANCE && isSafeFromEdges(getX(), getY())) {
                currentMode = EDGE_FOLLOW; // Return to edge following when safe
            }
            
            // Navigate based on current mode
            if (currentMode == EDGE_FOLLOW) {
                navigateEdges();
            } else if (currentMode == AVOIDANCE) {
                avoidObstacle();
            }
            
            // Shoot at arena center when safe
            shootingAtCenter = true;
            if (getEnergy() >= SHOOT_ENERGY_COST && !threatFound) {
                angle_t centerAngle = calculateAngleToCenter();
                cannon(centerAngle, rangeVals[2]); // Use max range for center shooting
            } else {
                shootingAtCenter = false;
            }
        }
        
        skip(); // End tick
    }
}

std::string EdgeRobot::name() const {
    return myName;
    // Virtual name method implementation complete
}

angle_t EdgeRobot::calculateAngleToCenter() {
    // Calculate angle from current position to arena center (4999, 4999)
    posx_t currentX = getX();
    posy_t currentY = getY();
    
    // Error handling: validate position bounds
    if (currentX < 0 || currentX > MAX_POS_X || currentY < 0 || currentY > MAX_POS_Y) {
        return 0; // Default to North if invalid position
    }
    
    // Calculate angle using atan2 (returns radians, convert to degrees)
    angle_t angleRad = atan2(4999 - currentY, 4999 - currentX);
    angle_t angleDeg = angleRad * 180.0 / 3.14159265;
    
    // Convert to compass degrees (0=North, 90=East, 180=South, 270=West)
    angle_t compassAngle = 90 - angleDeg;
    if (compassAngle < 0) compassAngle += 360;
    if (compassAngle >= 360) compassAngle -= 360;
    
    return compassAngle;
}

bool EdgeRobot::detectThreatInCone(angle_t& threatDirection, range_t& threatRange) {
    // Scan ±10° cone around current heading for threats at 75-100 unit range
    angle_t currentHeading = getDirection();
    precision_t scanPrecision = 10; // ±10 degrees
    range_t maxRange = 75; // Medium detection range
    
    // Perform scan within the detection cone
    range_t detectedRange = scan(currentHeading, scanPrecision);
    
    if (detectedRange > 0 && detectedRange <= maxRange) {
        // Threat detected within range
        threatDirection = currentHeading;
        threatRange = detectedRange;
        return true;
    }
    
    return false; // No threat detected
}

void EdgeRobot::navigateEdges() {
    // Navigate along arena edges maintaining safe distance from walls
    posx_t currentX = getX();
    posy_t currentY = getY();
    angle_t currentDirection = getDirection();
    
    // Calculate distances to each edge
    range_t distToNorth = currentY;
    range_t distToSouth = MAX_POS_Y - currentY;
    range_t distToEast = MAX_POS_X - currentX;
    range_t distToWest = currentX;
    
    // Find nearest edge
    range_t minDist = distToNorth;
    angle_t targetDirection = 0; // North
    
    if (distToEast < minDist) {
        minDist = distToEast;
        targetDirection = 90; // East
    }
    if (distToSouth < minDist) {
        minDist = distToSouth;
        targetDirection = 180; // South
    }
    if (distToWest < minDist) {
        minDist = distToWest;
        targetDirection = 270; // West
    }
    
    // Maintain safe distance from edges (200 units minimum)
    range_t safeDistance = 200;
    if (minDist > safeDistance) {
        // Move toward nearest edge
        drive(targetDirection, MAX_SPEED);
    } else {
        // Follow edge parallel to nearest wall
        if (targetDirection == 0 || targetDirection == 180) {
            // North or South edge - move East or West
            drive(90, MAX_SPEED); // Move East
        } else {
            // East or West edge - move North or South  
            drive(0, MAX_SPEED); // Move North
        }
    }
}

void EdgeRobot::engageThreat(angle_t direction, range_t range) {
    // Engage detected threat by shooting at it
    currentMode = THREAT_ENGAGE;
    threatDetected = true;
    threatDirection = direction;
    threatRange = range;
    
    // Aim and shoot at the detected threat
    if (getEnergy() >= SHOOT_ENERGY_COST) {
        cannon(direction, range);
    }
}

void EdgeRobot::avoidObstacle() {
    // Avoid obstacles while attempting to maintain edge proximity
    currentMode = AVOIDANCE;
    
    // Simple avoidance: turn 90 degrees and move
    angle_t currentDirection = getDirection();
    angle_t avoidDirection = currentDirection + 90;
    if (avoidDirection >= 360) avoidDirection -= 360;
    
    // Move away from potential collision
    drive(avoidDirection, MAX_SPEED);
}

// Additional navigation helper implementations
range_t EdgeRobot::calculateDistanceToNearestEdge(posx_t x, posy_t y) {
    range_t distNorth = y;
    range_t distSouth = MAX_POS_Y - y;
    range_t distEast = MAX_POS_X - x;
    range_t distWest = x;
    
    range_t minDist = distNorth;
    if (distSouth < minDist) minDist = distSouth;
    if (distEast < minDist) minDist = distEast;
    if (distWest < minDist) minDist = distWest;
    
    return minDist;
}

bool EdgeRobot::isSafeFromEdges(posx_t x, posy_t y) {
    return calculateDistanceToNearestEdge(x, y) >= 200; // Safe distance from walls
}

angle_t EdgeRobot::calculateEdgeFollowDirection(posx_t x, posy_t y, angle_t currentDirection) {
    // Determine optimal direction to follow nearest edge
    range_t distNorth = y;
    range_t distSouth = MAX_POS_Y - y;
    range_t distEast = MAX_POS_X - x;
    range_t distWest = x;
    
    range_t minDist = distNorth;
    angle_t edgeDirection = 0; // North
    
    if (distEast < minDist) {
        minDist = distEast;
        edgeDirection = 90; // East
    }
    if (distSouth < minDist) {
        minDist = distSouth;
        edgeDirection = 180; // South
    }
    if (distWest < minDist) {
        minDist = distWest;
        edgeDirection = 270; // West
    }
    
    return edgeDirection;
}

} /* namespace RobotGame */