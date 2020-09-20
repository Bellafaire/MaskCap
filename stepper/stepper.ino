// Include the Arduino Stepper Library
#include <Stepper.h>

// Number of steps per output rotation
const int stepsPerRevolution = 600;

// Create Instance of Stepper library
//Stepper myStepper(stepsPerRevolution, 5, 4, 3, 2);
//Stepper rightStepper(stepsPerRevolution, 32, 33, 25, 26);
Stepper rightStepper(stepsPerRevolution, 32, 33, 25, 26);
//Stepper leftStepper(stepsPerRevolution, 19, 18, 17, 16);
Stepper leftStepper(stepsPerRevolution, 22, 23, 21, 19);


void setup()
{
  // set the speed at 20 rpm:
  rightStepper.setSpeed(25);
  leftStepper.setSpeed(25);
  // initialize the serial port:
  Serial.begin(115200);
}


void raiseLeft(void * pvParameters) {
  leftStepper.setSpeed(20);
  leftStepper.step(-1500);
  vTaskDelete(NULL);
}

void lowerLeft(void * pvParameters) {
  leftStepper.setSpeed(35);
  leftStepper.step(1000);
  vTaskDelete(NULL);
}
void raiseRight(void * pvParameters) {
  rightStepper.setSpeed(20);
  rightStepper.step(-1000);
  vTaskDelete(NULL);
}

void lowerRight(void * pvParameters) {
  rightStepper.setSpeed(35);
  rightStepper.step(1500);
  vTaskDelete(NULL);
}

void raiseMask() {

  TaskHandle_t xRaiseLeft = NULL;
  TaskHandle_t xRaiseRight = NULL;
  xTaskCreatePinnedToCore( raiseRight, "raise_right", 4096, (void *) 1 , tskIDLE_PRIORITY, &xRaiseRight, 0 );
  xTaskCreatePinnedToCore( raiseLeft, "raise_left", 4096, (void *) 1 , tskIDLE_PRIORITY, &xRaiseLeft, 0 );

  delay(15000);
}

void lowerMask() {
  TaskHandle_t xLowerLeft = NULL;
  TaskHandle_t xLowerRight = NULL;
  xTaskCreatePinnedToCore( lowerRight, "raise_right", 4096, (void *) 1 , tskIDLE_PRIORITY, &xLowerRight, 0 );
  xTaskCreatePinnedToCore( lowerLeft, "raise_left", 4096, (void *) 1 , tskIDLE_PRIORITY, &xLowerLeft, 0 );

  delay(15000);
}


void loop()
{
  Serial.println("Raising mask");
  raiseMask();

  Serial.println("Lowering mask");
  lowerMask();

}
