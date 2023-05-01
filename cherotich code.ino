#include <LiquidCrystal.h>
#include <TinyGPS.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
const int relay_Pin = 2;
const int buzzer_Pin = 3;
const int ir_Sensor = 10;
const int vibration_Sensor = 11;

TinyGPS gps;
long lat, lon;
bool ir_status = LOW;
bool vibration_status = LOW;

void setup() {
  pinMode(relay_Pin, OUTPUT);
  pinMode(buzzer_Pin, OUTPUT);
  pinMode(ir_Sensor, INPUT);
  pinMode(vibration_Sensor, INPUT);

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("ACCIDENT DETECTION");
  lcd.setCursor(3, 2);
  lcd.print("SYSTEM");
}

void loop() {
  ir_status = digitalRead(ir_Sensor);
  delay(100);
  
  if(ir_status == HIGH) {
    digitalWrite(buzzer_Pin, LOW);   
    delay(200);
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3, 2);
    lcd.print("Detected");
    delay(500);  
    
    digitalWrite(relay_Pin, HIGH);  // Turn on the relay
    
    while(1) {
      vibration_status = digitalRead(vibration_Sensor);
      delay(100);   
      
      if(vibration_status == HIGH) {
        lcd.clear();
        lcd.print("Accident Detected");
        lcd.setCursor(3, 2);
        lcd.print("Sending Msg");
        delay(500);    
        
        digitalWrite(relay_Pin, LOW);   // Turn off the relay
        digitalWrite(buzzer_Pin, HIGH); // Turn on the buzzer
        
        Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
        delay(1000);  // Delay of 1000 milli seconds or 1 second
        Serial.println("AT+CMGS=\"+254724904084\"\r"); // Replace x with mobile number
        delay(1000);
        Serial.println("Accident Detected, please check location");// The SMS text you want to send
        while(1) {
          gps_read();
        }
      } else {
        digitalWrite(relay_Pin, HIGH);  // Turn on the relay
        digitalWrite(buzzer_Pin, LOW);  // Turn off the buzzer
      }
    }
  } else {
    lcd.clear();
    lcd.print("Seat Belt");
    lcd.setCursor(3, 2);
    lcd.print("not Detected");
    digitalWrite(relay_Pin, LOW);   
    delay(200);              
    digitalWrite(buzzer_Pin, HIGH);   
    delay(200);          
  }
}

void gps_read() { 
  byte a;
  
  if(Serial.available()) {
    a = Serial.read();
   
    while(gps.encode(a)) {   // encode gps data 
      gps.get_position(&lat,&lon); // get latitude and longitude
    
      Serial.println("Position: ");
      Serial.print("lat:");
      Serial.println((lat*0.000001),8);
      Serial.print("log:");
      Serial.println((lon*0.000001),8);
    }
  }
}
