#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <math.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define MQ2_PIN PA1
#define RL_VALUE 5.0 // kΩ
#define CLEAN_AIR_ADC 450 // Based on your observation
#define RO_CLEAN_AIR_FACTOR 9.83 // For LPG from datasheet
float Ro = 5;
void setup() {
 Serial.begin(9600);
 analogReadResolution(12); // STM32 Blue Pill ADC 12-bit (0–4095)

 
 // Initialize OLED
 if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
 Serial.println("OLED failed");
 while (true);
 }
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 Ro = calibrateSensor();
 Serial.print("Calibrated Ro: ");
 Serial.println(Ro);
}
void loop() {
 int adcValue = analogRead(MQ2_PIN);
 float voltage = (adcValue / 4095.0) * 3.3;
 float Rs = ((3.3 - voltage) / voltage) * RL_VALUE;
 float ratio = Rs / Ro;

 // Estimate PPM (LPG example): log(ppm) = a*log(Rs/Ro) + b
 float ppm = pow(10, (-0.47 * log10(ratio) + 1.63));
 Serial.print("ADC: "); Serial.print(adcValue);
 Serial.print(" | Rs: "); Serial.print(Rs);
 Serial.print(" | Ratio: "); Serial.print(ratio);
 Serial.print(" | Estimated PPM: "); Serial.println(ppm);
 display.clearDisplay();
 display.setCursor(0, 0);
 display.print("ADC: "); display.println(adcValue);
 display.print("Rs: "); display.println(Rs, 2);
 display.print("Ratio: "); display.println(ratio, 2);
 display.print("PPM: "); display.println(ppm, 1);
 display.display();
 delay(1000);
}
float calibrateSensor() {
 float rs = ((4095.0 - CLEAN_AIR_ADC) / CLEAN_AIR_ADC) * RL_VALUE;
 return rs / RO_CLEAN_AIR_FACTOR;
}