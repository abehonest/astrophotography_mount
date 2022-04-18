#include <AccelStepper.h>
#include <LiquidCrystal.h>


// Define the steppers and camera and the pins they will use
AccelStepper stepperAz(1,9,7);
AccelStepper stepperAlt(1,8,5);
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);


// Declare Alt-Az Coordinates
int input;
unsigned long startTime;
boolean reset = true;

float deficitX = 0;
float deficitY = 0;

void setup() {
  // put your setup code here, to run once:

  //
  Serial.begin(115200);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 

  stepperAlt.setCurrentPosition(AltPositionLong);
  stepperAz.setCurrentPosition(AzPositionLong);

  lcd.clear();
  lcd.setCursor(0,0);
  

  lcd.setCursor(0,1);

  // set speeds and accelerations
  stepperAlt.setMaxSpeed(1000);
  stepperAlt.setAcceleration(500);

  stepperAz.setMaxSpeed(1000);
  stepperAz.setAcceleration(500);

  delay(3000);

}

void loop() {
  // put your main code here, to run repeatedly:

  if(reset)
  {
    startTime = millis();
    reset = false;
  }

   // check new frame every half second
  if (millis() - startTime > 500)
  {
 
    // Assign String
    String inputLine = Serial.readStringUntil('\n');

    if(checkLine(inputLine))
    {
  
      // read Alt offset
      D1 = inputLine.charAt(0) - 48;
      D2 = inputLine.charAt(1) - 48;
      D3 = inputLine.charAt(2) - 48;
      D4 = inputLine.charAt(3) - 48;
      D5 = inputLine.charAt(4) - 48;

      Alt = D1*10000 + D2*1000 + D3*100 + D4*10 + D5
      
      // read Az offset
      D1 = inputLine.charAt(5) - 48;
      D2 = inputLine.charAt(6) - 48;
      D3 = inputLine.charAt(7) - 48;
      D4 = inputLine.charAt(8) - 48;
      D5 = inputLine.charAt(9) - 48;

      Az = D1*10000 + D2*1000 + D3*100 + D4*10 + D5
      
      lcd.clear();
      
      lcd.setCursor(0,0);     
      lcd.print(Alt);
  
      lcd.setCursor(0,1);
      lcd.print(Az);
      
      stepperAlt.move(Alt);
      stepperAlt.setSpeed(150);
      stepperAz.move(Az);
      stepperAz.setSpeed(150);
      reset = true;
    }

  }

  stepperAlt.runSpeedToPosition();
  stepperAz.runSpeedToPosition();
  
}
