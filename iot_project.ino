#define BLYNK_TEMPLATE_ID "TMPL3FY5OMVYi"
#define BLYNK_TEMPLATE_NAME "IOT Power meter"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial

#define APP_DEBUG

//Library Inclusion
#include <Wire.h>
#include "BlynkEdgent.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>
#include <Fonts/FreeSerifBold12pt7b.h>


Adafruit_INA219 ina219;

//Display 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;
float energy = 0;
unsigned long previousMillis = 0;
unsigned long lastMillis = millis();

void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    
  // Clear the Display
  display.clearDisplay();
  
  display.setFont(&FreeSerifBold12pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 25);
  display.println("POWER");
  display.setCursor(22, 55);
  display.println("METER");
  display.display();
  delay(2000);
  
  // Initialize the INA219.
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  // By default the initialization will use the largest range (32V, 2A).
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  BlynkEdgent.begin();

}

void loop(void) 
{
  BlynkEdgent.run();
  
  Readvalues();
  Displayvalue();
    
}


void Readvalues()
{

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  Blynk.virtualWrite(V4, power_mW);
  
  unsigned long currentMillis = millis();
  energy += power_mW * (currentMillis - lastMillis) / 3600000.0;
  lastMillis = currentMillis;
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.print("Energy:        "); Serial.print(energy); Serial.println(" mWh");
  Serial.println("---------------------------------");

}

void Displayvalue()
{
  display.clearDisplay();
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  
  //For Oled Screen GFX
  display.setCursor(0, 30);
  display.println("---------------------");
  display.setCursor(62, 0);
  display.println("|");
  display.setCursor(62, 8);
  display.println("|");
  display.setCursor(62, 16);
  display.println("|");
  display.setCursor(62, 24);
  display.println("|");
  display.setCursor(62, 32);
  display.println("|");
  display.setCursor(62, 40);
  display.println("|");
  display.setCursor(62, 48);
  display.println("|");
  display.setCursor(62, 56);
  display.println("|");
  display.setCursor(62, 64);
  display.println("|");
  
  
  //Voltage
  display.setCursor(16, 12);
  Blynk.virtualWrite(V0, String((loadvoltage ), 2) + String(" V") );
  display.print(loadvoltage);
  display.println(" V");
  

  //Current
  if (current_mA > 1000) {
    Blynk.virtualWrite(V1, String((current_mA / 1000), 2) + String(" A") );
    display.setCursor(77, 12);
    display.print((current_mA / 1000), 2);
    display.println(" A");
  }
  else 
  {
    Blynk.virtualWrite(V1, String(current_mA, 2) + String(" mA"));
    display.setCursor(77, 12);
    display.print(current_mA, 1);
    display.println(" mA");
  }
  
  
  //Power
  if (power_mW > 1000) {
    Blynk.virtualWrite(V2, String((power_mW / 1000), 2) + String(" W") );
    display.setCursor(17, 46);
    display.print(String((power_mW / 1000), 2));
    display.println(" W");
  }
  else {
    Blynk.virtualWrite(V2, String(power_mW) + String(" mW") );
    display.setCursor(17, 46);
    display.print(power_mW, 0);
    display.println(" mW");
  }


  //Energy
  if (energy > 1000) {
    Blynk.virtualWrite(V3, String((energy / 1000), 2) + String(" Wh") );
    display.setCursor(77, 46);
    display.print((energy / 1000), 2);
    display.println(" Wh");
  }
  else if (energy > 10) {
    Blynk.virtualWrite(V3, String(energy, 2) + String(" mWh") );
    display.setCursor(77, 46);
    display.print(energy, 1);
    display.println(" mWh");
  }
  else 
  {
    Blynk.virtualWrite(V3, String(energy, 2) + String(" mWh"));
    display.setCursor(77, 46);
    display.print(energy, 2);
    display.println(" mWh");
  }

  display.display();

  delay(1000);

}
