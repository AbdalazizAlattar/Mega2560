#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "config.h"
#include "motor.h"
#include "traffic_light.h"

// ========== COMMAND FUNCTION TYPE ==========
typedef void (*CommandFunction)(String);

// ========== COMMAND STRUCTURE ==========
struct Command {
  const char* name;
  CommandFunction function;
  const char* description;
};

// ========== COMMAND FUNCTION DECLARATIONS ==========
void handleForwardCommand(String args);
void handleReverseCommand(String args);
void handleSpeedCommand(String args);
void handleStopCommand(String args);
void handleDemoCommand(String args);
void handleTrafficCommand(String args);
void handleRedCommand(String args);
void handleYellowCommand(String args);
void handleGreenCommand(String args);
void handleAllOffCommand(String args);
void handleAllOnCommand(String args);
void handleFlashCommand(String args);
void handleEmergencyCommand(String args);
void handleTimingCommand(String args);
void handleLoopCommand(String args);
void handleHelpCommand(String args);

void processCommand(String input);
void printHelp();

// ========== COMMAND LOOKUP TABLE ==========
const Command COMMAND_TABLE[] = {
  {"f", handleForwardCommand, "'f' + number - Move forward (e.g., f100)"},
  {"r", handleReverseCommand, "'r' + number - Move reverse (e.g., r100)"},
  {"s", handleSpeedCommand, "'s' + number - Set speed (1-20, lower = faster)"},
  {"stop", handleStopCommand, "'stop' - Stop motor"},
  {"demo", handleDemoCommand, "'demo' - Run motor demonstration"},
  {"traffic", handleTrafficCommand, "'traffic' - Start/Stop automatic traffic light cycle"},
  {"red", handleRedCommand, "'red' - Turn on RED light only"},
  {"yellow", handleYellowCommand, "'yellow' - Turn on YELLOW light only"},
  {"green", handleGreenCommand, "'green' - Turn on GREEN light only"},
  {"alloff", handleAllOffCommand, "'alloff' - Turn off all traffic lights"},
  {"allon", handleAllOnCommand, "'allon' - Turn on all traffic lights"},
  {"flash", handleFlashCommand, "'flash' - Flash all lights"},
  {"emergency", handleEmergencyCommand, "'emergency' - Emergency flashing red"},
  {"timing", handleTimingCommand, "'timing' + r,y,g - Set timing (e.g., timing5000,2000,4000)"},
  {"loop", handleLoopCommand, "'loop' - Move motor 10,000 steps forward with circulating lights"},
  {"help", handleHelpCommand, "'help' - Show this help message"}
};

const int COMMAND_COUNT = sizeof(COMMAND_TABLE) / sizeof(Command);

// ========== COMMAND FUNCTION IMPLEMENTATIONS ==========

void handleForwardCommand(String args) {
  int steps = args.toInt();
  if (validateStepCount(steps)) {
    Serial.println("Moving forward " + String(steps) + " steps");
    extern void displayCommand(String command);
    displayCommand("FWD " + String(steps));
    moveSteps(steps, CLOCKWISE);
  } else {
    Serial.println("Invalid step count");
    extern void displayError(String error);
    displayError("Invalid steps");
  }
}

void handleReverseCommand(String args) {
  int steps = args.toInt();
  if (validateStepCount(steps)) {
    Serial.println("Moving reverse " + String(steps) + " steps");
    extern void displayCommand(String command);
    displayCommand("REV " + String(steps));
    moveSteps(steps, COUNTER_CLOCKWISE);
  } else {
    Serial.println("Invalid step count");
    extern void displayError(String error);
    displayError("Invalid steps");
  }
}

void handleSpeedCommand(String args) {
  int speed = args.toInt();
  if (setMotorSpeed(speed)) {
    Serial.println("Speed set to " + String(speed));
    extern void displayCommand(String command);
    displayCommand("SPD " + String(speed));
  } else {
    Serial.println("Speed must be between " + String(MIN_STEP_DELAY) + " and " + String(MAX_STEP_DELAY));
    extern void displayError(String error);
    displayError("Invalid speed");
  }
}

void handleStopCommand(String args) {
  stopMotor();
  Serial.println("Motor stopped");
  extern void displayCommand(String command);
  displayCommand("STOP");
}

void handleDemoCommand(String args) {
  extern void displayCommand(String command);
  displayCommand("DEMO");
  runMotorDemo();
}

void handleTrafficCommand(String args) {
  toggleTrafficLightCycle();
}

void handleRedCommand(String args) {
  trafficLight.isRunning = false;
  setTrafficLightByColor(LIGHT_RED);
  Serial.println("RED light ON");
}

void handleYellowCommand(String args) {
  trafficLight.isRunning = false;
  setTrafficLightByColor(LIGHT_YELLOW);
  Serial.println("YELLOW light ON");
}

void handleGreenCommand(String args) {
  trafficLight.isRunning = false;
  setTrafficLightByColor(LIGHT_GREEN);
  Serial.println("GREEN light ON");
}

void handleAllOffCommand(String args) {
  trafficLight.isRunning = false;
  setTrafficLight(false, false, false);
  Serial.println("All traffic lights OFF");
}

void handleAllOnCommand(String args) {
  trafficLight.isRunning = false;
  setTrafficLight(true, true, true);
  Serial.println("All traffic lights ON");
}

void handleFlashCommand(String args) {
  flashAllLights();
}

void handleEmergencyCommand(String args) {
  emergencyFlash();
}

void handleTimingCommand(String args) {
  if (!parseTimingCommand("timing" + args)) {
    Serial.println("Failed to set timing");
  }
}

void handleLoopCommand(String args) {
  Serial.println("Starting loop sequence: " + String(LOOP_SEQUENCE_STEPS) + " steps forward with circulating lights");
  
  extern void displayCommand(String command);
  displayCommand("LOOP START");
  delay(2000);  // Show "LOOP START" for 2 seconds
  
  extern bool disableAutoLCDUpdate;
  disableAutoLCDUpdate = true;  // Disable automatic LCD updates
  
  trafficLight.isRunning = false;
  setTrafficLight(false, false, false);
  
  unsigned long startTime = millis();
  unsigned long lastLightChange = millis();
  unsigned long lastStepUpdate = millis();
  LightColor currentLight = LIGHT_RED;
  LightColor previousLight = LIGHT_GREEN; // Different from current to force first update
  int lastDisplayedStep = -1;
  
  setTrafficLightByColor(currentLight);
  Serial.println("Starting with RED light");
  
  motorState.isRunning = true;
  
  // Initial LCD setup
  extern LiquidCrystal_I2C lcd;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LOOP SEQUENCE ACTIVE");
  
  for (int step = 0; step < LOOP_SEQUENCE_STEPS; step++) {
    if (millis() - lastLightChange >= LIGHT_CIRCULATION_DELAY_MS) {
      currentLight = (LightColor)((currentLight + 1) % 3);
      lastLightChange = millis();
      
      setTrafficLightByColor(currentLight);
      String lightName = (currentLight == LIGHT_RED) ? "RED" : 
                        (currentLight == LIGHT_YELLOW) ? "YELLOW" : "GREEN";
      Serial.println("Switching to " + lightName + " light - Steps completed: " + String(step));
    }
    
    // Only update LCD when light changes or every 500 steps
    if (currentLight != previousLight || step - lastDisplayedStep >= 500) {
      lcd.setCursor(0, 1);
      String lightName = (currentLight == LIGHT_RED) ? "RED   " : 
                        (currentLight == LIGHT_YELLOW) ? "YELLOW" : "GREEN ";
      lcd.print("LIGHT: " + lightName + "        ");
      
      lcd.setCursor(0, 2);
      lcd.print("STEP: " + String(step) + " / " + String(LOOP_SEQUENCE_STEPS) + "     ");
      
      lcd.setCursor(0, 3);
      float progress = (float)step / LOOP_SEQUENCE_STEPS * 100;
      lcd.print("PROGRESS: " + String(progress, 1) + "%      ");
      
      previousLight = currentLight;
      lastDisplayedStep = step;
    }
    
    executeStep(CLOCKWISE);
  }
  
  setTrafficLight(false, false, false);
  motorState.isRunning = false;
  stopMotor();
  
  disableAutoLCDUpdate = false;  // Re-enable automatic LCD updates
  
  extern LiquidCrystal_I2C lcd;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("THANK YOU AZIZ");
  delay(30000);  // 30 seconds
  
  unsigned long totalTime = millis() - startTime;
  Serial.println("Loop sequence completed!");
  Serial.println("Total time: " + String(totalTime/1000.0) + " seconds");
}

void handleHelpCommand(String args) {
  printHelp();
}

void processCommand(String input) {
  input.trim();
  input.toLowerCase();
  
  if (input.length() == 0) return;
  
  // Check for commands that start with a letter and have arguments
  for (int i = 0; i < COMMAND_COUNT; i++) {
    String commandName = String(COMMAND_TABLE[i].name);
    
    if (commandName.length() == 1 && input.startsWith(commandName)) {
      // Single letter command with arguments (f100, r50, s5)
      String args = input.substring(1);
      COMMAND_TABLE[i].function(args);
      return;
    } else if (input.startsWith(commandName)) {
      // Multi-character command
      String args = input.substring(commandName.length());
      COMMAND_TABLE[i].function(args);
      return;
    }
  }
  
  Serial.println("Unknown command: " + input);
  Serial.println("Type 'help' for available commands");
}

void printHelp() {
  Serial.println("=== STEPPER MOTOR AND TRAFFIC LIGHT CONTROLLER ===");
  Serial.println("=== MOTOR COMMANDS ===");
  for (int i = 0; i < COMMAND_COUNT; i++) {
    if (String(COMMAND_TABLE[i].name).startsWith("f") || 
        String(COMMAND_TABLE[i].name).startsWith("r") || 
        String(COMMAND_TABLE[i].name).startsWith("s") ||
        String(COMMAND_TABLE[i].name) == "stop" ||
        String(COMMAND_TABLE[i].name) == "demo") {
      Serial.println(COMMAND_TABLE[i].description);
    }
  }
  
  Serial.println();
  Serial.println("=== TRAFFIC LIGHT COMMANDS ===");
  for (int i = 0; i < COMMAND_COUNT; i++) {
    if (String(COMMAND_TABLE[i].name) != "f" && 
        String(COMMAND_TABLE[i].name) != "r" && 
        String(COMMAND_TABLE[i].name) != "s" &&
        String(COMMAND_TABLE[i].name) != "stop" &&
        String(COMMAND_TABLE[i].name) != "demo" &&
        String(COMMAND_TABLE[i].name) != "help") {
      Serial.println(COMMAND_TABLE[i].description);
    }
  }
  
  Serial.println();
  Serial.println("=== OTHER COMMANDS ===");
  Serial.println("'help' - Show this help message");
  Serial.println();
  printTrafficTiming();
}

#endif // COMMANDS_H