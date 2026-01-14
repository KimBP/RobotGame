// MyRobot.cpp - Template robot implementation
//
// This is a starting point for creating your own robot.
// Replace with template behavior with your own logic.
//
// HOW TO USE:
// 1. Save this file as MyRobot.cpp
// 2. Modify to init() and run() methods below
// 3. Compile: g++ -shared -fPIC -o MyRobot.so MyRobot.cpp -I.
// 4. Run: ./bin/robot-arena --robots lib/MyRobot.so
//
// ROBOT API:
// - scan(direction, precision): Scan for enemies
// - cannon(direction, range): Fire cannon
// - drive(direction, speed): Move robot
// - skip(): End current tick
// - getSpeed(), getDirection(): Get current state
// - getX(), getY(): Get position
// - getTick(), getArmor(), getEnergy(): Get status
// - armorToEnergy(), energyToArmor(): Convert resources

#include <string>

// Basic types (you may need to adjust based on actual Robot.h)
typedef int angle_t;
typedef int precision_t; 
typedef unsigned int range_t;
typedef int speed_t;
typedef int armor_t;
typedef int energy_t;

struct tune_t {
    int tune_speed;
    int tune_cannon;
};

// Forward declarations
class RobCtrl;

// Template Robot base class (adjust based on actual Robot.h)
class Robot {
protected:
    RobCtrl* controller;
    std::string robot_name;
    
public:
    Robot(RobCtrl* ctrl) : controller(ctrl) {}
    virtual ~Robot() {}
    
    virtual struct tune_t init(std::string& name) = 0;
    virtual void run() = 0;
    virtual std::string name() const = 0;
    
    // Basic interface methods (implement as needed)
    virtual range_t scan(angle_t direction, precision_t precision) { return 0; }
    virtual int cannon(angle_t direction, range_t range) { return 0; }
    virtual void drive(angle_t direction, speed_t speed) {}
    virtual void skip() {}
    virtual speed_t getSpeed() { return 0; }
    virtual angle_t getDirection() { return 0; }
    virtual unsigned int getX() { return 0; }
    virtual unsigned int getY() { return 0; }
    virtual int getTick() { return 0; }
    virtual armor_t getArmor() { return 100; }
    virtual energy_t getEnergy() { return 1000; }
    virtual void armorToEnergy(armor_t amount) {}
    virtual void energyToArmor(energy_t amount) {}
};

class RobCtrl {};

class MyRobot : public Robot {
private:
    // Add your private member variables here
    bool enemy_detected;
    int last_scan_range;
    
public:
    /**
     * Constructor - initialize base class
     */
    MyRobot(RobCtrl* robCtrl) : Robot(robCtrl) {
        // Initialize member variables
        enemy_detected = false;
        last_scan_range = 0;
    }
    
    /**
     * Initialize your robot
     * Called once at the start of the game
     * 
     * Add initialization code here:
     * - Set initial strategy
     * - Configure scanning behavior
     * - Set up any internal state
     */
    struct tune_t init(std::string& robot_name) override {
        robot_name = "MyRobot";
        
        // Your initialization code here
        scan(0, 10);  // Initial scan forward
        
        struct tune_t tune;
        tune.tune_speed = 2;  // maxSpeed
        tune.tune_cannon = 2;  // maxCannonRange
        
        return tune;
    }
    
    /**
     * Main robot logic - called every tick
     * This is where you implement your robot's behavior
     */
    void run() override {
        energy_t current_energy = getEnergy();
        
        // Template behavior - replace with your own logic
        if (current_energy > 50) {
            // Look for enemies
            last_scan_range = scan(0, 5);  // Scan forward with 5 degree precision
            
            if (last_scan_range > 0 && last_scan_range < 500) {
                enemy_detected = true;
                // Fire cannon if enemy detected and in range
                cannon(0, 300);  // Fire forward, range 300
            } else {
                enemy_detected = false;
                // Move around if no enemy
                drive(90, 20);  // Turn right
                drive(0, 30);   // Move forward
            }
        } else {
            // Low energy - defensive behavior
            if (current_energy > 20) {
                armorToEnergy(getArmor() / 2);  // Convert half armor to energy
            } else {
                // Critical energy - escape
                drive(180, 50);  // Move backward
            }
        }
        
        // Add your robot logic here
    }
    
    /**
     * Get robot name for identification
     */
    std::string name() const override {
        return "MyRobot";
    }
};

// Factory function (required for plugin loading)
extern "C" Robot* getRobot(RobCtrl* robCtrl) {
    return new MyRobot(robCtrl);
}