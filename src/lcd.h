#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// ========== LCD GLOBAL INSTANCE ==========
extern LiquidCrystal_I2C lcd;

// ========== LCD FUNCTION DECLARATIONS ==========
void initializeLCD();
void updateLCDStatus();
void displayMotorInfo(int steps, String direction, int speed);
void displayTrafficInfo(String state, unsigned long timeLeft);
void displayCommand(String command);
void displayError(String error);
void clearLCDLine(int line);

// ========== LCD FUNCTION IMPLEMENTATIONS ==========

void initializeLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("HI ABDALAZIZ");
  
  delay(30000);  // 30 seconds
  lcd.clear();
  updateLCDStatus();
}

void updateLCDStatus() {
  lcd.setCursor(0, 0);
  lcd.print("M:STOP T:");
  
  extern TrafficLightState_t trafficLight;
  if (trafficLight.isRunning) {
    switch (trafficLight.currentState) {
      case TRAFFIC_RED:
        lcd.print("RED  ");
        break;
      case TRAFFIC_YELLOW:
        lcd.print("YEL  ");
        break;
      case TRAFFIC_GREEN:
        lcd.print("GRN  ");
        break;
    }
  } else {
    lcd.print("OFF  ");
  }
  
  lcd.setCursor(0, 1);
  extern MotorState motorState;
  if (motorState.isRunning) {
    lcd.print("Running Spd:");
    lcd.print(motorState.stepDelay);
    lcd.print("  ");
  } else {
    lcd.print("Ready   Spd:");
    lcd.print(motorState.stepDelay);
    lcd.print("  ");
  }
}

void displayMotorInfo(int steps, String direction, int speed) {
  clearLCDLine(1);
  lcd.setCursor(0, 1);
  String dir = (direction == "CLOCKWISE") ? "FWD" : "REV";
  lcd.print(dir + ":" + String(steps) + " S:" + String(speed));
}

void displayTrafficInfo(String state, unsigned long timeLeft) {
  clearLCDLine(0);
  lcd.setCursor(0, 0);
  lcd.print("Traffic: " + state);
  if (timeLeft > 0) {
    lcd.setCursor(11, 0);
    lcd.print(String(timeLeft/1000) + "s");
  }
}

void displayCommand(String command) {
  clearLCDLine(1);
  lcd.setCursor(0, 1);
  lcd.print("CMD: " + command.substring(0, 11));
}

void displayError(String error) {
  clearLCDLine(1);
  lcd.setCursor(0, 1);
  lcd.print("ERR: " + error.substring(0, 11));
  delay(2000);
  updateLCDStatus();
}

void clearLCDLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                ");
}

#endif // LCD_H