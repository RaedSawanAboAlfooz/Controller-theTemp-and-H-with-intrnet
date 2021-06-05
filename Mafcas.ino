#define BLYNK_PRINT Serial
#include <Wire.h>   //  library for I2C protocol
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#define MotorPin1 14 //D5
#define MotorPin2 12 //D6
#define ControlTemp D7 //D7 اللمبات 
#define ControlH D0 // D8
#define PinDhtMaf D9 //D3
//============Mafcas===============
bool boolMon = 0, boolHon = 0;
bool control = 0;
int number = 0;
#define add D3
#define min D8
#define ok 2//tx
bool Connected2Blynk = false;
//#define controlHhad 16 //D0
//#define controlThad 5 //D1
//#define PinDhtHad 0 //D3
int i = 1, h = 46, TimerController = 30, HH = 46 ;
bool x = 0 , boolMotor1 = 0, boolMotor2 = 0;
float temp = 37.33   ;
unsigned long  time2;
char ssid[]            = "aser";
char pass[]            = "@@@@####";
char auth[]            = "ghMpzrKxb_7cPj6zGSPuqY5ASxNZ7PH4";
char server[]          = "blynk-cloud.com";
unsigned int port      = 8442;

DHT dhtMaf(PinDhtMaf, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer1;
//DHT dhtHad(PinDhtHad, DHT11);
BLYNK_WRITE(V0)
{
  temp = param.asFloat();
  char s[3];
  String a = dtostrf(temp, 1, 3, s);
  for (int i = 0; i < a.length(); ++i)
  {
    EEPROM.write(i, a[i]);
    Serial.print("Wrote: ");
    Serial.println(a[i]);
  }
  EEPROM.commit();
  Serial.print ("temp = ");
  Serial.println (a);
}
BLYNK_WRITE(V1)
{
  h = param.asInt(); // assigning incoming value from pin V1 to a variable
  String ass = String(h);
  for (int i = 700; i < ass.length() + 700; ++i)
  {
    EEPROM.write(i, ass[i - 700]);
    Serial.print("Wrote: ");
    Serial.println(ass[i - 700]);
  }
  EEPROM.commit();
  Serial.print ("H = ");
  Serial.println (ass);
}
BLYNK_WRITE(V2)
{
  TimerController = param.asInt(); // assigning incoming value from pin V1 to a variable
  String ass = String(TimerController);
  for (int i = 60; i < ass.length() + 60; ++i)
  {
    EEPROM.write(i, ass[i - 60]);
    Serial.print("Wrote: ");
    Serial.println(ass[i - 60]);
  }
  EEPROM.commit();
  Serial.print ("TimerController = ");
  Serial.println(ass);
  Serial.println (ass.length());
}

void setup() {
  Serial.begin (9600);
  EEPROM.begin(1024);
  delay(20);
  dhtMaf.begin();
  pinMode (ok, 1);


  WiFi.begin(ssid, pass);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  //  Blynk.begin(auth, ssid, pass);
  pinMode (MotorPin1, OUTPUT );
  pinMode (MotorPin2, OUTPUT);
  pinMode (ControlTemp, OUTPUT);
  pinMode (ControlH, OUTPUT);

  String e = "0";
  timer1.setInterval(3000L, timer);
  for (int i = 0; i < 33; ++i)
  {
    e += char(EEPROM.read(i));
    //    Serial.println("rread");
  }
  temp = e.toFloat();
  delay (2000);
  String dd = "";
  for (int i = 60; i < 80; ++i)
  {
    dd += char(EEPROM.read(i));
  }
  TimerController = dd.toInt();
  String ZZ = "";
  for (int i = 520; i < 530; ++i)
  {
    ZZ += char(EEPROM.read(i));

  } delay (2000);
  HH = ZZ.toFloat();
  String QQ = "";
  for (int i = 700 ; i < 710; ++i)
  {
    QQ += char(EEPROM.read(i));
  } delay (2000);
  h = QQ.toInt();
  Serial.println();
  //  Serial.print("temp: ");
  //  Serial.println(temp);


  //  Serial.print ("Time Controller = ");
  //  Serial.println (TimerController);
  //
  //  Serial.print ("h= ");
  //  Serial.println (h);

  lcd.init();                      // initialize the lcd
  lcd.backlight();
  pinMode (add, 1);
  pinMode (min, 1);
  pinMode (D9, 1);
  digitalWrite(add, 1);
  digitalWrite(min, 1);
  digitalWrite(ok, 1);
  while (WiFi.status() != WL_CONNECTED) {
    TimerMotor(TimerController);
    ControllerTemperatureMaf(temp, h);
    controlScreen();
    Serial.print(".");
  }

  Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
  Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
  while (Blynk.connect() == false) {
    // Wait until connected
    Serial.println ("false");
    TimerMotor(TimerController);
    ControllerTemperatureMaf(temp, h);
    controlScreen();
  }
  Serial.println("Connected to Blynk server");
  timer1.setInterval(500L, timer);
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, TimerController);


}
void timer() {
  TimerMotor(TimerController);
  ControllerTemperatureMaf(temp, h);
  Connected2Blynk = Blynk.connected();

  if (!Connected2Blynk) {
    Serial.println("Not connected to Blynk server");
    controlScreen();
    Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
  }
  else {
    //    Serial.println("Connected to Blynk server");
  }
  //  ControllerHad(TH, HH);
}

void loop() {
  Blynk.run() ;
  //      Serial.println ("kk");

  controlScreen();
  timer1.run();
}
void controlScreen() {
  delay(200);
  if (digitalRead(ok) == 0) {
    delay(200);
    if (control == 1 && number == 1) {
      char s[3];
      String a = dtostrf(temp, 1, 3, s);
      for (int i = 0; i < a.length(); ++i)
      {
        EEPROM.write(i, a[i]);
        Serial.print("Wrote: ");
        Serial.println(a[i]);
      }
      EEPROM.commit();
      Serial.print ("=================================");
      delay(1000);
    }
    if (control == 1 && number == 2) {
      String ass = String(h);
      for (int i = 700; i < ass.length() + 700; ++i)
      {
        EEPROM.write(i, ass[i - 700]);
        Serial.print("Wrote: ");
        Serial.println(ass[i - 700]);
      }
      EEPROM.commit();

      Serial.print ("H = ");
      Serial.println (ass);
      Serial.print ("==========================================");
      delay(1000);
    }
    if (control == 1 && number == 3) {
      String ass = String(TimerController);
      for (int i = 60; i < ass.length() + 60; ++i)
      {
        EEPROM.write(i, ass[i - 60]);
        Serial.print("Wrote: ");
        Serial.println(ass[i - 60]);
      }
      EEPROM.commit();
      Serial.print ("============================================");
      delay(1000);
    }
    control = !control;

  }
  if (control == 0) {
    if (digitalRead(add) == 0) {
      delay(200);
      if (number  < 4) {
        number ++;
      }
      if (number == 4)
      {
        number = 0;
      }
    }

    if (digitalRead(min) == 0) {
      delay(200);
      if (number > 0) {
        number --;
      }
      else {
        number = 3;
      }
    }
  }
  if (control == 1) {
    if ( number == 0) {
      lcd.setCursor(8, 0);
      lcd.print ("    ");
    }
    else if ( number == 1) {
      lcd.setCursor(8, 0);
      lcd.print ("*T* ");
    }
    else if ( number == 2) {
      lcd.setCursor(8, 0);
      lcd.print ("*H* ");
    }
    else if ( number == 3) {
      lcd.setCursor(8, 0);
      lcd.print ("*C* ");
    }
  }
  if (control == 0) {
    if ( number == 0) {
      lcd.setCursor(8, 0);
      lcd.print (" *  ");
    }
    else if ( number == 1) {
      lcd.setCursor(8, 0);
      lcd.print (" T  ");
    }
    else if ( number == 2) {
      lcd.setCursor(8, 0);
      lcd.print (" H  ");
    }
    else if ( number == 3) {
      lcd.setCursor(8, 0);
      lcd.print (" c  ");
    }
  }
  if (control == 1 && number == 1) {
    if (digitalRead(add) == 0) {
      if (temp <= 40) {
        temp = temp + 0.1;
      }
      delay(50);
    }
    if (digitalRead(min ) == 0) {
      if (temp > 0) {
        temp = temp - 0.1;
      }  delay(50);
    }
  }
  if (control == 1 && number == 2) {
    if (digitalRead(add) == 0) {
      if (h <= 99) {
        h++;
      }
      delay(200);
    }
    if (digitalRead(min ) == 0) {
      if (h > 40) {
        h--;
      }  delay(200);
    }
  }
  if (control == 1 && number == 3) {
    if (digitalRead(add) == 0) {
      if (TimerController <  60) {
        TimerController++;
      }
      delay(200);
    }
    if (digitalRead(min ) == 0) {
      if (TimerController > 0) {
        TimerController--;
      }  delay(200);
    }
  }
  lcd.setCursor(0, 1);
  lcd.print ("t=");
  lcd.print (temp);
  lcd.setCursor(8, 1);
  lcd.print ("h=");
  lcd.print (h);
  lcd.setCursor(14, 1);
  lcd .print (TimerController);
  if (TimerController < 10) {
    lcd .print (" ");
  }
}
void ControllerTemperatureMaf (float TController, int HController) {
  int HH = dhtMaf.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dhtMaf.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dhtMaf.readTemperature(true);
  float hif = dhtMaf.computeHeatIndex(f, h);
  float hic = dhtMaf.computeHeatIndex(t, h, false);
  if (t < TController - 5) {
    Blynk.notify("الحراره ممنخفضه");
  }
  if (h < HH - 15) {
    Blynk.notify("الرطوبه منخفضه");
  }

  if (isnan(HH) || isnan(t)) {
    HH = 0;
    t = 0;
    if (boolMon == 0) {
      Blynk.notify("حساس المفقس لايعمل");
      boolMon = 1;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print ("T=");
  lcd.print (t);
  lcd.setCursor(11, 0);
  lcd.print (" H=");
  lcd .print (HH);
  if (HH > 0 || t > 0 ) {
    boolMon = 0;
  }
  Blynk.virtualWrite(V4, HH);
  Blynk.virtualWrite(V3, t);

  if (TController <= t) {
    digitalWrite(ControlTemp, LOW);
    //    Serial.println ("LOW");
    //    Serial.println (digitalRead(ControlTemp));
  }
  if (TController - 1.5 > t && t > 0) {
    digitalWrite(ControlTemp, HIGH);
    //    Serial.print ("HIGH     t= ");
    //    Serial.println (t);
  }
  if (HController <= HH) {
    digitalWrite(ControlH, LOW);
  }
  if (HController - 5 > HH && HH > 0) {
    digitalWrite(ControlH, HIGH);
  }
}





void TimerMotor (int time ) {
  int m = 0;
  Serial.print ("i= ");
  Serial.print (i);
  Serial.print ("    111 -=== ");
  Serial.print (digitalRead(MotorPin1));
  Serial.print ("     222 -=== ");
  Serial.print (digitalRead(MotorPin2));
  Serial.print ("      timer = ");
  m = TimerController * 60 * 1000;
  if (m > 0) {
    time2 = millis ();
    Serial.print(m * i - time2);
    Serial.print ("     m = ");
    Serial.print (m);
    if ( m * i  < time2 ) {
      if (time2 < 3000) {
        i = 1;
      }
      i++;
      x = !x;
    }
    if (x == 0) {
      digitalWrite(MotorPin1, LOW);
      if (boolMotor1 == 0) {
        boolMotor2 = 0;
        boolMotor1 = 1;
      }
      digitalWrite(MotorPin2, HIGH);
    }
    else if (x == 1) {
      digitalWrite(MotorPin2, LOW);
      if (boolMotor2 == 0) {
        boolMotor1 = 0;
        boolMotor2 = 1;
      }
      digitalWrite(MotorPin1, HIGH);
    }
  }
  if (i % 2 == 0) {
    Blynk.virtualWrite(V5, "قرش");
  } else {
    Blynk.virtualWrite(V5, "برغ");
  }
  int first = m*i-time2;
  int secand =(first/1000);
  int ende=(secand/60)+1;
  
  Serial.print ("   ende = ");
  Serial.println (ende);
  Blynk.virtualWrite(V6, ende);
}