#include <WiFi.h>
#include <HTTPClient.h>
#include <base64.h>
#include <Keypad.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>
 
#define I2C_ADDR 0x3F
#define BACKLIGHT_PIN 3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
SoftwareSerial rfid (4, 2);
const char* ssid = "Fahri";
const char* password = "12345678";
String authUsername = "U01";
//String authPassword = "";

// inisialisasi Keypad
String pad;
const byte ROWS = 4; //4 Row
const byte COLS = 4; //4 kolom
char keys[ROWS][COLS] = {
  
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 25, 18, 19}; //connect to the column pinouts of the keypad 
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//Variable RFID
int readVal = 0; // individual karakter baca dari serial
unsigned int readData[10]; // baca data dari serial
int counter = -1; // counter to keep position in the buffer
char tagId[11]; 

void setup()
{
  Serial.begin(115200); 
  rfid.begin(9600);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Line1Shirt");
  }
 
  Serial.println("Connected to Line1Shirt ");
  //Serial.begin(9600);
  Serial.println();
  //lcd.init(); // initialize the lcd
  lcd.begin(21, 22);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home (); 
  for(int i=0; i<3; i++){
      lcd.backlight();
      delay(200);
      lcd.noBacklight();
      delay(200);
    }
  lcd.backlight();   
  lcd.setCursor(1, 0);
  lcd.print("***PROMOSYS***");
  delay(2000);
  lcd.setCursor(1, 1);
  lcd.print("==============");
  delay(2000);
  lcd.setCursor(1, 1);
  lcd.print("BUSANA APPAREL");
  delay(2000);
  lcd.clear();

}
void parseTag() {
  int j = 0;
  for (int i = 0; i < 10; ++i) {
    if (i > 3){
      tagId[j] = readData[i];
      j = j + 1;     
    }   
  }
} 

void processTag() {
  // convert id to a string
  parseTag();
  
  // print it
  printTag();
  
}

void printTag() {
  //Serial.print("Tag value: ");
  long result = strtol(tagId,NULL,16);
  Serial.printf("000%d",result);
  delay(20);  
   
}

void clearSerial() {
  while (rfid.read() >= 0) {
    ; 
  }
}

 
void loop()
{    
  //Inisialisasi LCD
  Serial.available();
    lcd.write(Serial.read());
    lcd.setCursor(0, 0);
    lcd.print("Emp.ID:");
    lcd.setCursor(0, 1);
    lcd.print("Line: 00");
    lcd.setCursor(13, 1);
    lcd.print("--");
    lcd.setCursor(0, 2);
    lcd.print("TR:0");
    lcd.setCursor(5, 2);
    lcd.print("Av:");
    lcd.setCursor(10, 2);
    lcd.print("OP:");
    
    lcd.setCursor(0, 3);
    lcd.print("==> Please Login:");
    

  keypress();
  lcd.setCursor(7, 0);
  lcd.print(pad);
  delay(50);

  //Koneksi Rest
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin("http://10.10.60.19/dev03/rest/svc/RSTRPC/getDataCounter?data=U01");
    Serial.print("[HTTP] GET...\n");
    String auth = base64::encode(authUsername);
    http.addHeader("Authorization", "Basic " + auth);
 
    int httpCode = http.GET(); 
 
    if (httpCode > 0) { //Check for the returning code
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {    
              String payload = http.getString();
              Serial.println(httpCode);
              Serial.println(payload);
      }
    }
 
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n");

    }
 
    http.end();
  }
  delay(10000);
  
  //Baca RFID
  if (rfid.available() > 0) {
    // read the incoming byte:
    readVal = rfid.read();
    
    if (readVal == 2) {
      counter = 0; // start baca
    } 
    else if (readVal == 3) {
      processTag();
      
      clearSerial();
      counter = -1;
    }
    else if (counter >= 0) {
      readData[counter] = readVal;     
      ++counter;
    } 
  }
}
//Keypad
void keypress(){
  char keypressed = myKeypad.getKey();
  String konv = String(keypressed);
  pad+=konv;

}
  
