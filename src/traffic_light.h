#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <Arduino.h>
#include "config.h"

// ========== TRAFFIC LIGHT STATE STRUCTURE ==========
struct TrafficLightState_t {
  bool isRunning;
  unsigned long startTime;
  TrafficLightState currentState;
  unsigned long redTime;
  unsigned long yellowTime;
  unsigned long greenTime;
};

// ========== GLOBAL TRAFFIC LIGHT STATE ==========
extern TrafficLightState_t trafficLight;

// ========== TRAFFIC LIGHT FUNCTION DECLARATIONS ==========
void initializeTrafficLight();
void setTrafficLight(bool red, bool yellow, bool green);
void setTrafficLightByColor(LightColor color);
void toggleTrafficLightCycle();
void runTrafficLightCycle();
void flashAllLights();
void emergencyFlash();
bool setTrafficTiming(unsigned long red, unsigned long yellow, unsigned long green);
bool parseTimingCommand(String command);
void printTrafficTiming();

// ========== TRAFFIC LIGHT FUNCTION IMPLEMENTATIONS ==========

void initializeTrafficLight() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  
  trafficLight.isRunning = false;
  trafficLight.startTime = 0;
  trafficLight.currentState = TRAFFIC_RED;
  trafficLight.redTime = DEFAULT_RED_TIME_MS;
  trafficLight.yellowTime = DEFAULT_YELLOW_TIME_MS;
  trafficLight.greenTime = DEFAULT_GREEN_TIME_MS;
  
  setTrafficLight(false, false, false);
}

void setTrafficLight(bool red, bool yellow, bool green) {
  digitalWrite(RED_LED_PIN, red ? HIGH : LOW);
  digitalWrite(YELLOW_LED_PIN, yellow ? HIGH : LOW);
  digitalWrite(GREEN_LED_PIN, green ? HIGH : LOW);
}

void setTrafficLightByColor(LightColor color) {
  switch (color) {
    case LIGHT_RED:
      setTrafficLight(true, false, false);
      break;
    case LIGHT_YELLOW:
      setTrafficLight(false, true, false);
      break;
    case LIGHT_GREEN:
      setTrafficLight(false, false, true);
      break;
  }
}

void toggleTrafficLightCycle() {
  trafficLight.isRunning = !trafficLight.isRunning;
  
  if (trafficLight.isRunning) {
    Serial.println("Traffic light cycle STARTED");
    trafficLight.startTime = millis();
    trafficLight.currentState = TRAFFIC_RED;
    setTrafficLight(true, false, false);
    extern void displayTrafficInfo(String state, unsigned long timeLeft);
    displayTrafficInfo("STARTED", 0);
  } else {
    Serial.println("Traffic light cycle STOPPED");
    setTrafficLight(false, false, false);
    extern void displayTrafficInfo(String state, unsigned long timeLeft);
    displayTrafficInfo("STOPPED", 0);
  }
}

void runTrafficLightCycle() {
  if (!trafficLight.isRunning) return;
  
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - trafficLight.startTime;
  
  switch (trafficLight.currentState) {
    case TRAFFIC_RED:
      if (elapsedTime >= trafficLight.redTime) {
        trafficLight.currentState = TRAFFIC_GREEN;
        setTrafficLight(false, false, true);
        trafficLight.startTime = currentTime;
        Serial.println("Traffic: RED -> GREEN");
      }
      break;
      
    case TRAFFIC_GREEN:
      if (elapsedTime >= trafficLight.greenTime) {
        trafficLight.currentState = TRAFFIC_YELLOW;
        setTrafficLight(false, true, false);
        trafficLight.startTime = currentTime;
        Serial.println("Traffic: GREEN -> YELLOW");
      }
      break;
      
    case TRAFFIC_YELLOW:
      if (elapsedTime >= trafficLight.yellowTime) {
        trafficLight.currentState = TRAFFIC_RED;
        setTrafficLight(true, false, false);
        trafficLight.startTime = currentTime;
        Serial.println("Traffic: YELLOW -> RED");
      }
      break;
  }
}

void flashAllLights() {
  trafficLight.isRunning = false;
  Serial.println("Flashing all traffic lights");
  
  extern void displayTrafficInfo(String state, unsigned long timeLeft);
  displayTrafficInfo("FLASHING", 0);
  
  for (int i = 0; i < FLASH_CYCLES; i++) {
    setTrafficLight(true, true, true);
    delay(FLASH_DELAY_MS);
    setTrafficLight(false, false, false);
    delay(FLASH_DELAY_MS);
  }
  
  Serial.println("Flash complete");
  displayTrafficInfo("FLASH DONE", 0);
}

void emergencyFlash() {
  trafficLight.isRunning = false;
  Serial.println("Emergency flashing RED");
  
  extern void displayTrafficInfo(String state, unsigned long timeLeft);
  displayTrafficInfo("EMERGENCY", 0);
  
  for (int i = 0; i < EMERGENCY_FLASH_CYCLES; i++) {
    setTrafficLight(true, false, false);
    delay(EMERGENCY_FLASH_DELAY_MS);
    setTrafficLight(false, false, false);
    delay(EMERGENCY_FLASH_DELAY_MS);
  }
  
  Serial.println("Emergency flash complete");
  displayTrafficInfo("EMRG DONE", 0);
}

bool setTrafficTiming(unsigned long red, unsigned long yellow, unsigned long green) {
  if (red >= MIN_TIMING_MS && yellow >= MIN_TIMING_MS && green >= MIN_TIMING_MS) {
    trafficLight.redTime = red;
    trafficLight.yellowTime = yellow;
    trafficLight.greenTime = green;
    return true;
  }
  return false;
}

bool parseTimingCommand(String command) {
  // Expected format: timing5000,2000,4000
  String timingStr = command.substring(6); // Remove "timing"
  
  int firstComma = timingStr.indexOf(',');
  int secondComma = timingStr.indexOf(',', firstComma + 1);
  
  if (firstComma > 0 && secondComma > firstComma) {
    unsigned long newRedTime = timingStr.substring(0, firstComma).toInt();
    unsigned long newYellowTime = timingStr.substring(firstComma + 1, secondComma).toInt();
    unsigned long newGreenTime = timingStr.substring(secondComma + 1).toInt();
    
    if (setTrafficTiming(newRedTime, newYellowTime, newGreenTime)) {
      printTrafficTiming();
      return true;
    } else {
      Serial.println("All timing values must be at least " + String(MIN_TIMING_MS) + "ms");
      return false;
    }
  } else {
    Serial.println("Invalid timing format. Use: timing5000,2000,4000");
    return false;
  }
}

void printTrafficTiming() {
  Serial.println("Traffic timing updated:");
  Serial.println("RED: " + String(trafficLight.redTime/1000) + "s");
  Serial.println("YELLOW: " + String(trafficLight.yellowTime/1000) + "s");
  Serial.println("GREEN: " + String(trafficLight.greenTime/1000) + "s");
}

#endif // TRAFFIC_LIGHT_H