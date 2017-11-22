#include <Arduino.h>
/*This example code uses bogde's excellent library: https://github.com/bogde/HX711

 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 Arduino pin
 3 -> HX711 CLK
 12 -> DOUT
 5V -> VCC
 GND -> GND

 Most any pin on the Arduino Uno will be compatible with DOUT/CLK for the HX711.

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.
*/

// include the library code for the LCD display:
#include <LiquidCrystal.h>
// include the library code for the load cell amplifier:
#include <HX711.h>

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// initialize the HX711 library with the numbers of the interface pins
#define DOUT 12
#define CLK 3

HX711 scale(DOUT, CLK);

//float calibration_factor = 886600; //for lbs on the 1kg scale
float calibration_factor = -1955; //1955 for g for my 1kg max scale setup

void setup()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("LCD Key Shield");
  lcd.setCursor(0, 1);
  lcd.print("Press Key:");

  Serial.begin(9600);
  Serial.println("HX711 calibration sketch modified by Derek.");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: ");           //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  lcd.clear();
} /*--(end setup )---*/

/*
 Adjust to your own needs
  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
  ...
  matrix.setRotation(0, 2);    // The first display is position upside down
  matrix.setRotation(3, 2);    // The same hold for the last display
*/
int wait = 50;
int inc = -2;

void loop() /*----( LOOP: RUNS CONSTANTLY )----*/
{
  scale.set_scale(calibration_factor);             //Adjust to this calibration factor

  float reading = 0;
  reading = scale.get_units(5), 1; //Read scaled units, average 5 readings, display 1 decimal places

  if (reading < .5){
    scale.tare();                    //Reset the scale to 0
    reading = scale.get_units(5), 1; //Read scaled units, average 5 readings, display 1 decimal places
  }

  Serial.print("Reading: "); // Serial.print(scale.get_units(15), 1); //Read scaled units, average 15 readings, display 1 decimal places
  Serial.print(reading);
  Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" cal_fctr: ");
  Serial.print(calibration_factor);
  Serial.print(" RawData: ");
  Serial.println(scale.read());
  Serial.println();

  lcd.setCursor(0, 0);
  lcd.print(reading);
  lcd.setCursor(8, 0);
  lcd.print(" Grams");

  //Read inputs for Left, Right, Up, Down and Select buttons that are connected to A0

  int x;
  x = analogRead(0);
  lcd.setCursor(10, 1);
  if (x < 60){
    lcd.print("Right ");
  }
  else if (x < 200){
    lcd.print("Up    ");
  }
  else if (x < 400){
    lcd.print("Down  ");
  }
  else if (x < 600){
    lcd.print("Left  ");
  }
  else if (x < 800){
    lcd.print("Zero");
    scale.tare(); //Reset the scale to 0
  }

  if (Serial.available()){
    char temp = Serial.read();
    if (temp == '+' || temp == 'a')
      calibration_factor += 1;
    else if (temp == '-' || temp == 'z')
      calibration_factor -= 1;
  }

} /* --(end main loop )-- */