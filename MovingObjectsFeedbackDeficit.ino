// Libraries for devices used in this implementation
#include <AccelStepper.h>
#include <LiquidCrystal.h>

// Define the steppers and camera and the pins they will use
AccelStepper stepperAz(1,9,7); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
AccelStepper stepperAlt(1,8,5);
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

// Define system constants for the motors
float AltGearRatio = 43.2;
int AltBaseSPR = 400; // steps per revolution
int AltMicroRatio = 32;

float AzGearRatio = 90;
int AzBaseSPR = 200;
int AzMicroRatio = 16;


// Declare Alt-Az Coordinates (each coordinate is read as XXX:XX:XX.X, with hours, minutes, and seconds to one decimal point of accuracy)
int AltH;
int AltM;
float AltS;

int AzH;
int AzM;
float AzS;

float Alt;
float Az;

float oldAlt;
float oldAz;

float AltPosition;
float AzPosition;

int input;
unsigned long startTime;
boolean reset = true;

float deficitAlt = 0;
float deficitAz = 0;

// check input line to ensure that each read string is of the form '[Alt Coordinate] [Az Coordinate]'
boolean checkLine(String line)
{
  if(line.charAt(0) > 51 || line.charAt(0) < 48)
  {
    return false;
  }
  else if(line.charAt(3) != 58)
  {
    return false;
  }
  else if(line.charAt(6) != 58)
  {
    return false;
  }
  else if(line.charAt(9) != 46)
  {
    return false;
  }
  else if(line.charAt(11) != 32)
  {
    return false;
  }
  else if(line.charAt(12) > 51 || line.charAt(12) < 48)
  {
    return false;
  }
  else if(line.charAt(15) != 58)
  {
    return false;
  }
  else if(line.charAt(18) != 58)
  {
    return false;
  }
  else if(line.charAt(21) != 46)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 

  int hundreds;
  int tens;
  int ones;
  int decimalA;
  String inputLine;

  // Wait until the input is received before setting up program
  while(Serial.available() < 1)
  {
    lcd.setCursor(0,0);
    lcd.print("Not yet");
  }

  // Read current position of the North Star (Polaris) to standardize the frame of reference

  // Successive sets of coordinates are separated by newline sequences
  inputLine = Serial.readStringUntil('\n');
    
  // read Alt Coordinate
  // hours
  hundreds = inputLine.charAt(0) - 48;
  tens = inputLine.charAt(1) - 48;
  ones = inputLine.charAt(2) - 48;
  
  AltH = hundreds * 100 + tens * 10 + ones;
  
  // minutes
  tens = inputLine.charAt(4) - 48;
  ones = inputLine.charAt(5) - 48;
  
  AltM = tens * 10 + ones;
  
  // seconds
  tens = inputLine.charAt(7) - 48;
  ones = inputLine.charAt(8) - 48;
  decimalA = inputLine.charAt(10) - 48;
  
  AltS = tens * 10 + ones + decimalA * 0.1;
  
  // read Az Coordinate
  // hours
  hundreds = inputLine.charAt(12) - 48;
  tens = inputLine.charAt(13) - 48;
  ones = inputLine.charAt(14) - 48;
  
  AzH = hundreds * 100 + tens * 10 + ones;
  
  // minutes
  tens = inputLine.charAt(16) - 48;
  ones = inputLine.charAt(17) - 48;
  
  AzM = tens * 10 + ones;
  
  // seconds
  tens = inputLine.charAt(19) - 48;
  ones = inputLine.charAt(20) - 48;
  decimalA = inputLine.charAt(22) - 48;

  AzS = tens * 10 + ones + decimalA * 0.1;
  
  Alt = AltH + (AltM * 60 + AltS)/3600;
  Az = AzH + (AzM * 60 + AzS)/3600;

  
  // convert to motor rotations
  AltPosition = (Alt)*(AltBaseSPR*AltMicroRatio*AltGearRatio)/360;
  AzPosition = (Az)*(AzBaseSPR*AzMicroRatio*AzGearRatio)/360;

  // cast to long type to be readable by stepper
  long AltPositionLong = (long) (AltPosition);
  long AzPositionLong = (long) (AzPosition);

  // Set the current position as the North Star's coordinates
  stepperAlt.setCurrentPosition(AltPositionLong);
  stepperAz.setCurrentPosition(AzPositionLong);

  // LCD output for debugging
  lcd.clear();
  lcd.setCursor(0,0);
  
  lcd.print("Alt:");
  lcd.print(AltH);
  lcd.print(" ");
  lcd.print(AltPositionLong);

  lcd.setCursor(0,1);
  lcd.print("Az:");
  lcd.print(AzH);
  lcd.print(" ");
  lcd.print(AzPositionLong);

  // set max speeds and accelerations
  stepperAlt.setMaxSpeed(1000);
  stepperAlt.setAcceleration(500);

  stepperAz.setMaxSpeed(1000);
  stepperAz.setAcceleration(500);

  // Store coordinates as new ones are read
  oldAlt = AltPositionLong;
  oldAz = AzPositionLong;

  delay(3000);

}

void loop() {
  // put your main code here, to run repeatedly:

  // measure time for each iterative loop
  if(reset)
  {
    startTime = millis();
    reset = false;
  }

  // only continue when target position has been reached and when more than 0.5 seconds have passed
  if (millis() - startTime > 500 && abs(stepperAz.currentPosition() - stepperAz.targetPosition()) == 0 && abs(stepperAlt.currentPosition() - stepperAlt.targetPosition()) == 0)
  {
    int hundreds;
    int tens;
    int ones;
    int decimalA;
  
    // Assign String
    String inputLine = Serial.readStringUntil('\n');

    if(checkLine(inputLine))
    {
      hundreds = inputLine.charAt(0) - 48;
      tens = inputLine.charAt(1) - 48;
      ones = inputLine.charAt(2) - 48;
      
      AltH = hundreds * 100 + tens * 10 + ones;
      
      // minutes
      tens = inputLine.charAt(4) - 48;
      ones = inputLine.charAt(5) - 48;
      
      AltM = tens * 10 + ones;
      
      // seconds
      tens = inputLine.charAt(7) - 48;
      ones = inputLine.charAt(8) - 48;
      decimalA = inputLine.charAt(10) - 48;
      
      AltS = tens * 10 + ones + decimalA * 0.1;
      
      // read Az Coordinate
      // hours
      hundreds = inputLine.charAt(12) - 48;
      tens = inputLine.charAt(13) - 48;
      ones = inputLine.charAt(14) - 48;
      
      AzH = hundreds * 100 + tens * 10 + ones;
      
      // minutes
      tens = inputLine.charAt(16) - 48;
      ones = inputLine.charAt(17) - 48;
        
      AzM = tens * 10 + ones;
      
      // seconds
      tens = inputLine.charAt(19) - 48;
      ones = inputLine.charAt(20) - 48;    
      decimalA = inputLine.charAt(22) - 48;
  
      AzS = tens * 10 + ones + decimalA * 0.1;
      
      Alt = AltH + (AltM * 60 + AltS)/3600;
      Az = AzH + (AzM * 60 + AzS)/3600;
  
      AltPosition = (Alt)*(AltBaseSPR*AltMicroRatio*AltGearRatio)/360;
      AzPosition = (Az)*(AzBaseSPR*AzMicroRatio*AzGearRatio)/360;

      // Calculate displacement needed to reach new coordinate
      float AltPositionNet = AltPosition - oldAlt;
      float AzPositionNet = AzPosition - oldAz;

      // Cast displacement into a long type to be read by Stepper
      long AltPositionLong = (long) (AltPositionNet);
      long AzPositionLong = (long) (AzPositionNet);

      // Accumulate the fractional steps lost while casting and add to total displacement
      deficitAlt += AltPositionNet - AltPositionLong;
      deficitAz += AzPositionNet - AzPositionLong;

      
      if(deficitAlt > 1.0 || deficitAlt < 1.0)
      {
        AltPositionLong += (long) deficitAlt;
        deficitAlt -= (long) deficitAlt;
      }

      if(deficitAz > 1.0 || deficitAz < 1.0)
      {
        AzPositionLong += deficitAz;
        deficitAz -= (long) deficitAz;
      }

      // Prevent rotations of more than half of a full revolution
      if(abs(AltPositionLong) > 0.5 * AltBaseSPR*AltMicroRatio*AltGearRatio)
      {
        if (AltPositionLong > 0)
        {
          AltPositionLong =  -1 * (AltBaseSPR*AltMicroRatio*AltGearRatio - abs(AltPositionLong));
        }
        else
        {
          AltPositionLong =  (AltBaseSPR*AltMicroRatio*AltGearRatio - abs(AltPositionLong));
        }
      }
    
      if(abs(AzPositionLong) > 0.5 * AzBaseSPR*AzMicroRatio*AzGearRatio)
      {
        if (AzPositionLong > 0)
        {
          AzPositionLong =  -1 * (AzBaseSPR*AzMicroRatio*AzGearRatio - abs(AzPositionLong));
        }
        else
        {
          AzPositionLong =  (AzBaseSPR*AzMicroRatio*AzGearRatio - abs(AzPositionLong));
        }
      }

      // Print data on LCD for debugging
      lcd.clear();
      lcd.setCursor(0,0);

      lcd.print(AltPositionLong);
      lcd.print(" ");
      lcd.print(deficitAlt);
      lcd.print(" ");
      lcd.print(AltPosition);
      lcd.print(" ");
      lcd.print(oldAlt);
  
      lcd.setCursor(0,1);
      lcd.print(AzPositionLong);
      lcd.print(" ");
      lcd.print(deficitAz);
      lcd.print(" ");
      lcd.print(AzPosition);
      lcd.print(" ");
      lcd.print(oldAz);
  
      // move the motors by the required displacement amount to reach new position
      stepperAlt.move(AltPositionLong);
      stepperAlt.setSpeed(450);
      stepperAz.move(AzPositionLong);
      stepperAz.setSpeed(450);
      reset = true;

      // Keep track of old coordinates
      oldAlt = AltPosition;
      oldAz = AzPosition;
    }

  }

  // Run in the given speen until position is reached
  stepperAlt.runSpeedToPosition();
  stepperAz.runSpeedToPosition();
  
}
