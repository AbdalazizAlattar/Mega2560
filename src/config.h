#ifndef CONFIG_H
#define CONFIG_H

// ========== PIN DEFINITIONS ==========
// Stepper motor pin connections (ULN2003 Driver)
#define MOTOR_IN1_PIN 8
#define MOTOR_IN2_PIN 9
#define MOTOR_IN3_PIN 10
#define MOTOR_IN4_PIN 11

// Traffic light LED pins
#define RED_LED_PIN 12
#define YELLOW_LED_PIN 13
#define GREEN_LED_PIN 7

// ========== MOTOR CONSTANTS ==========
#define MOTOR_STEPS_PER_REVOLUTION 8
#define DEFAULT_STEP_DELAY_MS 2
#define MIN_STEP_DELAY 1
#define MAX_STEP_DELAY 20
#define DEMO_STEPS 512
#define LOOP_SEQUENCE_STEPS 10000

// ========== TRAFFIC LIGHT CONSTANTS ==========
#define DEFAULT_RED_TIME_MS 10000
#define DEFAULT_YELLOW_TIME_MS 1500
#define DEFAULT_GREEN_TIME_MS 10000
#define MIN_TIMING_MS 1000
#define FLASH_CYCLES 6
#define EMERGENCY_FLASH_CYCLES 10
#define FLASH_DELAY_MS 300
#define EMERGENCY_FLASH_DELAY_MS 500
#define LIGHT_CIRCULATION_DELAY_MS 1000

// ========== SERIAL CONSTANTS ==========
#define SERIAL_BAUD_RATE 9600

// ========== ENUMS ==========
enum MotorDirection {
  CLOCKWISE = true,
  COUNTER_CLOCKWISE = false
};

enum TrafficLightState {
  TRAFFIC_RED = 0,
  TRAFFIC_GREEN = 1,
  TRAFFIC_YELLOW = 2
};

enum LightColor {
  LIGHT_RED = 0,
  LIGHT_YELLOW = 1,
  LIGHT_GREEN = 2
};

// ========== MOTOR STEP SEQUENCE ==========
const int MOTOR_STEP_SEQUENCE[MOTOR_STEPS_PER_REVOLUTION][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

#endif // CONFIG_H