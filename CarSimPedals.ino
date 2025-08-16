#include "HX711.h"
#include "Joystick.h"

#define DOUT2 23
#define CLK2 22
#define joyZ A0
#define joyThrottle A1

// Setup HX711 boards
HX711 brakeScale;

// Create raw variables
float rawBrake = 0;
// Create conversion variables
// Line joining (a,0) to (b, 1023) with gradient m=1023/(b-a) and c=-ma
const float minRawBrake = 15000;
const float maxRawBrake = 800000;
const float mBrake = 1023/(maxRawBrake - minRawBrake);
const float cBrake = - mBrake * minRawBrake;
// Create output variables
float adjBrake = 0;

int zAxis_ = 0;
int throttle_ = 0;

// Create Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID + 1, // Don't use first joystick ID, as this is taken by the steering wheel device.
  JOYSTICK_TYPE_MULTI_AXIS, 0, 0,             // Button Counts
  false, false, true, false, false, false,   // Axes Includes
  false, true, false, true, false);           // Rudder, throttle, accelerator, brake, steering

// Set auto send mode on joystick
const bool testAutoSendMode = true;

// Plotting - Comment line(s) to disable plotting and/or timer counter
//#define PLOT_RAW;
#define PLOT_ADJ;
//#define PLOT_PER;


void setup() {
  // Set Inbuilt LED High During Calibration
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Begin connection
  #ifdef PLOT_RAW || PLOT_ADJ || PLOT_PER || TIMER_COUNTER
    Serial.begin(9600);
  #endif
  
  // Setup second HX711
  brakeScale.begin(DOUT2, CLK2);
  brakeScale.set_scale();
  brakeScale.tare(); // Set scale to 0
  long zero_factor2 = brakeScale.read_average();

  // Set Range Values for Joystick
  //Joystick.setThrottleRange(220, 1023);
  Joystick.setBrakeRange(-50, -3500);

  // Start joystick connection
  Joystick.begin(testAutoSendMode);

  // Calibration finished, turn off LED
  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
  /* ------------ Read and set Values  ------------ */
  // Read load cell values
  rawBrake = brakeScale.get_units();
  // Convert values

  // Write values to joystick
  adjBrake = (mBrake * rawBrake) + cBrake;
  Joystick.setBrake(adjBrake);

  throttle_ = analogRead(joyThrottle);
  throttle_ = map(throttle_,1023,220,0,1023);
  Joystick.setThrottle(throttle_);

  zAxis_= (analogRead(joyZ));
  zAxis_ = map(zAxis_,1023,220,0,1023);
  Joystick.setZAxis(zAxis_);

  Serial.println(adjBrake, 1);
  //Serial.println(throttle_);
  //Serial.println(zAxis_);

}