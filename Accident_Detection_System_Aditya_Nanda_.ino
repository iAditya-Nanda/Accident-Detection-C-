//Accident Detection System made by Aditya Nanda
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int xInput = A0;
const int yInput = A1;
const int zInput = A2;

LiquidCrystal lcd(7, 6, 8, 9, 10, 11);

static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial Serial1(2,3);

// initialize minimum and maximum Raw Ranges for each axis
int RawMin = 0;
int RawMax = 1023;

// Take multiple samples to reduce noise
const int sampleSize = 10;

int xsample=0;
int ysample=0;
int zsample=0;

#define samples 10
#define minVal 0
#define MaxVal 1023

void setup() 
{
  analogReference(EXTERNAL);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Accident Alert  ");
  lcd.setCursor(0,1);
  lcd.print("     System     ");
  delay(2000);
  lcd.clear();
  lcd.print("     Made by   ");
  lcd.setCursor(0,1);
  lcd.print("   Aditya Nanda     ");
  delay(5000);
  lcd.clear();
  lcd.print("Initializing");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  delay(3000);
  lcd.clear();
  lcd.print("Callibrating ");
  lcd.setCursor(0,1);
  lcd.print("Accelerometer");
  for(int i=0;i<samples;i++)
  {
    xsample+=analogRead(xInput);
    ysample+=analogRead(yInput);
    zsample+=analogRead(xInput);
  }

  xsample/=samples;
  ysample/=samples;
  zsample/=samples;

  lcd.print(xsample);
  lcd.print(ysample);
  lcd.print(zsample);
  delay(3000);
  lcd.clear();
  lcd.print("Waiting For GPS");
  lcd.setCursor(0,1);
  lcd.print("     Signal    ");
  delay(2000);
  ss.begin(GPSBaud);
  delay(2000);
  lcd.clear();
  lcd.print("GPS is Ready");
  delay(3000);
  lcd.clear();
  lcd.print("System Ready");
  Serial.println("System Ready..");
  delay(2500);
  lcd.clear();
  
}

void loop() 
{
  //Read raw values
  int xRaw = ReadAxis(xInput);
  int yRaw = ReadAxis(yInput);
  int zRaw = ReadAxis(zInput);

  // Convert raw values to 'milli-Gs"
  long xScaled = map(xRaw, RawMin, RawMax, -3000, 3000);
  long yScaled = map(yRaw, RawMin, RawMax, -3000, 3000);
  long zScaled = map(zRaw, RawMin, RawMax, -3000, 3000);

  // re-scale to fractional Gs
  float xAccel = xScaled / 1000.0;
  float yAccel = yScaled / 1000.0;
  float zAccel = zScaled / 1000.0;

  Serial.print("X, Y, Z  :: ");
  Serial.print(xRaw);
  Serial.print(", ");
  Serial.print(yRaw);
  Serial.print(", ");
  Serial.print(zRaw);
  Serial.print(" :: ");
  Serial.print(xAccel,0);
  Serial.print("G, ");
  Serial.print(yAccel,0);
  Serial.print("G, ");
  Serial.print(zAccel,0);
  Serial.println("G");

  delay(200);

  
   // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.println();
  Serial.println();
  delay(1000);
}

  if(xRaw < minVal || xRaw > MaxVal  || yRaw < minVal || yRaw > MaxVal  || zRaw < minVal || zRaw > MaxVal)
    {
      lcd.print("Accident");
      lcd.setCursor(0,1);
      lcd.print("   Detected!!!");
      lcd.clear();
      lcd.print("Sending SMS ");
      Serial.println("Sending SMS");
      Send();
      Serial.println("SMS Sent");
      delay(2000);
      lcd.clear();
      lcd.print("System Ready");
    }
}
// Take samples and return the average
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
  reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}
void Send()
{ 
   Serial1.println("AT");
   delay(500);
   serialPrint();
   Serial1.println("AT+CMGF=1");
   delay(500);
   serialPrint();
   Serial1.print("AT+CMGS=");
   Serial1.print('"');
   Serial1.print("9418428011");    //mobile no. for SMS alert
   Serial1.println('"');
   delay(500);
   serialPrint();
   Serial1.print("Latitude:");
   Serial1.println(gps.location.lat(), 6);
   delay(500);
   serialPrint();
   Serial1.print(" longitude:");
   Serial1.println(gps.location.lng(), 6);
   delay(500);
   serialPrint();
   Serial1.print(" Speed:");
   Serial1.print(gps.speed.kmph());
   Serial1.println("KM/H");
   delay(500);
   serialPrint();
   Serial1.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
   Serial1.print(gps.location.lat(), 6);
   Serial1.print("+");              //28.612953, 77.231545   //28.612953,77.2293563
   Serial1.print(gps.location.lng(), 6);
   Serial1.write(26);
   delay(2000);
   serialPrint();
}
void serialPrint()
{
  while(Serial1.available()>0)
  {
    Serial.print(Serial1.read());
  }
}
