#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD address 0x27, 16 chars, 2 lines

#define sensor A0

#define relay1 8
#define relay2 9
#define relay3 10

float Rat2 = 0.66, Rat4 = 0.79, Rat6 = 0.85, Rat8 = 0.88;
int read_ADC;
int distance = 0;

// Custom arrow symbol for LCD
byte symbol[8] = {
    B00000,
    B00100,
    B00100,
    B00100,
    B11111,
    B01110,
    B00100,
    B00000};
int Ritsa;
void setup()
{
  Ritsa = analogRead(sensor);
  Serial.begin(4800);

  // Initialize I2C LCD
  lcd.init();
  lcd.backlight();

  // Create custom character
  lcd.createChar(1, symbol);

  // Setup pins
  pinMode(sensor, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  // Welcome screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cable Fault");
  lcd.setCursor(0, 1);
  lcd.print("Detection System");
  delay(2000);
  lcd.clear();

  // Initial scanning message
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait");
  delay(1500);
  lcd.clear();
}

void loop()
{
  // Display header
  lcd.setCursor(0, 0);
  lcd.print("Scanning Phases:");
  delay(1000);
  lcd.clear();

  // Test Red Phase
  lcd.setCursor(0, 0);
  lcd.print("Testing: RED");
  lcd.setCursor(0, 1);
  lcd.print("Status: ");

  digitalWrite(relay1, LOW);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  delay(500);

  data();

  if (distance > 0)
  {
    lcd.print("FAULT!");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RED Phase:");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" KM");
    delay(3000);
  }
  else
  {
    lcd.print("OK");
    delay(1500);
  }

  lcd.clear();

  // Test Yellow Phase
  lcd.setCursor(0, 0);
  lcd.print("Testing: YELLOW");
  lcd.setCursor(0, 1);
  lcd.print("Status: ");

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, HIGH);
  delay(500);

  data();

  if (distance > 0)
  {
    lcd.print("FAULT!");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("YELLOW Phase:");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" KM");
    delay(3000);
  }
  else
  {
    lcd.print("OK");
    delay(1500);
  }

  lcd.clear();

  // Test Blue Phase
  lcd.setCursor(0, 0);
  lcd.print("Testing: BLUE");
  lcd.setCursor(0, 1);
  lcd.print("Status: ");

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, LOW);
  delay(500);

  data();

  if (distance > 0)
  {
    lcd.print("FAULT!");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BLUE Phase:");
    lcd.setCursor(0, 1);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" KM");
    delay(3000);
  }
  else
  {
    lcd.print("OK");
    delay(1500);
  }

  lcd.clear();

  // Summary Display
  displaySummary();

  delay(5000); // Wait 5 seconds before next scan
}

void data()
{
  read_ADC = analogRead(sensor);
  if (read_ADC < Ritsa * Rat2)
  {
    distance = 2;
  }
  else if (read_ADC < Ritsa * Rat4)
  {
    distance = 4;
  }
  else if (read_ADC < Ritsa * Rat6)
  {
    distance = 6;
  }
  else if (read_ADC < Ritsa * Rat8)
  {
    distance = 8;
  }
  else
  {
    distance = 0;
  }
  Serial.print("value from the sensor: ");
  Serial.println(read_ADC);
}
}

void displaySummary()
{
  int distanceR, distanceY, distanceB;

  // Scan all phases quickly for summary
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  delay(300);
  data();
  distanceR = distance;

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, HIGH);
  delay(300);
  data();
  distanceY = distance;

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, LOW);
  delay(300);
  data();
  distanceB = distance;

  // Turn off all relays
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);

  // Display summary
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("R:");
  if (distanceR > 0)
  {
    lcd.print(distanceR);
    lcd.print("km");
  }
  else
  {
    lcd.print("OK");
  }

  lcd.setCursor(6, 0);
  lcd.print("Y:");
  if (distanceY > 0)
  {
    lcd.print(distanceY);
    lcd.print("km");
  }
  else
  {
    lcd.print("OK");
  }

  lcd.setCursor(12, 0);
  lcd.print("B:");
  if (distanceB > 0)
  {
    lcd.print(distanceB);
    lcd.print("km");
  }
  else
  {
    lcd.print("OK");
  }

  lcd.setCursor(0, 1);
  int faultCount = 0;
  if (distanceR > 0)
    faultCount++;
  if (distanceY > 0)
    faultCount++;
  if (distanceB > 0)
    faultCount++;

  if (faultCount == 0)
  {
    lcd.print("All Phases: OK  ");
  }
  else
  {
    lcd.print("Faults: ");
    lcd.print(faultCount);
    lcd.print(" Found ");
  }
}