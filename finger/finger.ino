// Wifi
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;
HTTPClient http;

// URL WEB IOT
String simpan = "http://192.168.43.239/finger/Data/save?id=";
String ambil_id = "http://192.168.43.239/finger/Data/ambil_id";
String respon_akses = "http://192.168.43.239/finger/Data/akses";
String respon_nama = "http://192.168.43.239/finger/Data/nama?id=";
String kirim_akses = "http://192.168.43.239/finger/Data/edit_akses?akses=";

String respon, akses, nama;

#define relay_on LOW
#define relay_off HIGH

// selenoid
#define selenoid D0

// buzzer
#define buzzer D5

// Telegram
#include "CTBot.h"
CTBot myBot;
String ssid = "bulus";
String pass = "12345678";
String token = "1225155667:AAEdsUesCtXUY90lR-I2ZP8R_zIUFdyQI4U";
const int id_bot = 1171547212;

// lcd
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// SDA ---------------> D2
// SCL ---------------> D1
// VCC ---------------> VIN
// GND ---------------> GND

#include <Adafruit_Fingerprint.h>

#if defined(__AVR__) || defined(ESP8266)
// For UNO and others without hardware serial, we must use software serial...
// pin #D7 is IN from sensor (GREEN wire)
// pin #D8 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(13, 15); // pin D7 dan D8

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup()  
{
  Serial.begin(115200);

  Serial.println();

  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(false);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("bulus", "12345678"); // Sesuaikan SSID dan password ini

  Serial.println();
  
  for (int u = 1; u <= 5; u++)
  {
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      USE_SERIAL.println("Wifi konek");
      USE_SERIAL.flush();
      delay(1000);
    }
    else
    {
      Serial.println("Wifi belum konek");
      delay(1000);
    }
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 0);
  lcd.print("KEAMANAN");
  lcd.setCursor(2, 1);
  lcd.print("RUANG SERVER");

  Serial.println("Starting Telegram Bot...");
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  if (myBot.testConnection()) {
    Serial.println("Koneksi Bagus");
  } else {
    Serial.println("Koneksi Jelek");
  }

  pinMode(selenoid, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(selenoid, relay_off);
  digitalWrite(buzzer, relay_off);
  
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600); // 57600
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    http.begin( ambil_id );
    
    USE_SERIAL.print("[HTTP] Ambil id dari database ...\n");
    int httpCode = http.GET();

    if(httpCode > 0)
    {
      USE_SERIAL.printf("[HTTP] kode response GET : %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK)
      {
        respon = http.getString();
        USE_SERIAL.println("ID : " + respon);
        num = respon.toInt();
        delay(200);
      }
    }
    else
    {
      USE_SERIAL.printf("[HTTP] GET data gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  
  return num;
}

void loop()                     // run over and over again
{
  // baca_telegram();
  
  ambil_akses();
  delay(50);
  if (akses == "Daftar")
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("AKSES : DAFTAR");
    lcd.setCursor(1, 1);
    lcd.print("TEMPELKAN JARI");
    Serial.println("Silahkan daftarkan jari anda yes..");
    id = readnumber();
    if (id == 0) {// ID #0 not allowed, try again!
       return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    
    while (!  getFingerprintEnroll() );
  }
  else if (akses == "Masuk")
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("AKSES : MASUK");
    lcd.setCursor(1, 1);
    lcd.print("TEMPELKAN JARI");
    
    getFingerprintID();
  }

  delay(50);
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Tempelkan jari");
      delay(50);

      //baca_telegram();
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("AKSES : DAFTAR");
      lcd.setCursor(1, 1);
      lcd.print("TEMPELKAN JARI");
    
      ambil_akses();
      if (akses != "Daftar")
      {
        return true;
      }
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AKSES : DAFTAR");
  lcd.setCursor(1, 1);
  lcd.print("TEMPELKAN LAGI");
  
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(100);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Oke sip, tempelkan lagi");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Tempelkan jari lagi yes..");
      delay(100);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      delay(500);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    delay(500);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    delay(500);

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("AKSES : DAFTAR");
    lcd.setCursor(1, 1);
    lcd.print("OKE SUKSES");
    
    save_database();

    digitalWrite(buzzer, relay_on);
    delay(200);
    digitalWrite(buzzer, relay_off);
    delay(100);
    digitalWrite(buzzer, relay_on);
    delay(200);
    digitalWrite(buzzer, relay_off);
    delay(100);
    digitalWrite(buzzer, relay_on);
    delay(200);
    digitalWrite(buzzer, relay_off);

    // KIRIM NOTIF TELE
    myBot.sendMessage(id_bot, "Sidik jari telah ditambahkan");
    Serial.print("Pesan Terkirim\n\n");
  
    delay(500);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  } 

  return true;
}

void ambil_akses()
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    http.begin( respon_akses );
    
    int httpCode = http.GET();

    if(httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        akses = http.getString();
        delay(100);
      }
    }
    else
    {
      USE_SERIAL.printf("[HTTP] GET data gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void baca_telegram()
{
  TBMessage msg;
  if (myBot.getNewMessage(msg)) {
    Serial.println(msg.text);
  }

  if (msg.text == "Masuk" || msg.text == "Daftar")
  {
    if ((WiFiMulti.run() == WL_CONNECTED))
    {
      http.begin( kirim_akses + (String) msg.text );
      
      USE_SERIAL.print("[HTTP] Edit akses ke database ...\n");
      int httpCode = http.GET();
  
      if(httpCode > 0)
      {
        if (httpCode == HTTP_CODE_OK)
        {
          respon = http.getString();
          USE_SERIAL.println("RESPON : " + respon);
        }
      }
      else
      {
        USE_SERIAL.printf("[HTTP] GET data gagal, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
  }
}

void save_database()
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    http.begin( simpan + (String) id );
    
    USE_SERIAL.print("[HTTP] Simpan id ke database ...\n");
    int httpCode = http.GET();

    if(httpCode > 0)
    {
      USE_SERIAL.printf("[HTTP] kode response GET : %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK)
      {
        respon = http.getString();
        USE_SERIAL.println("RESPON : " + respon);
        delay(200);
      }
    }
    else
    {
      USE_SERIAL.printf("[HTTP] GET data gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void cek_nama()
{
  if ((WiFiMulti.run() == WL_CONNECTED))
  {
    http.begin( respon_nama + (String) finger.fingerID );
    
    USE_SERIAL.print("[HTTP] Cek id ke database ...\n");
    int httpCode = http.GET();

    if(httpCode > 0)
    {
      USE_SERIAL.printf("[HTTP] kode response GET : %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK)
      {
        nama = http.getString();
        USE_SERIAL.println("Nama : " + nama);
        delay(200);
      }
    }
    else
    {
      USE_SERIAL.printf("[HTTP] GET data gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

// dibawah ini coding untuk fingerprint masuk

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Sidik jari tidak dikenali");

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("AKSES : MASUK");
    lcd.setCursor(1, 1);
    lcd.print("TIDAK DIKENALI");

    digitalWrite(buzzer, relay_on);
    delay(1000);
    digitalWrite(buzzer, relay_off);
    delay(300);
    digitalWrite(buzzer, relay_on);
    delay(1000);
    digitalWrite(buzzer, relay_off);

    // KIRIM NOTIF TELE
    myBot.sendMessage(id_bot, "Seseorang mencoba membuka pintu !");
    Serial.print("Pesan Terkirim\n\n");
  
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  cek_nama();

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AKSES : MASUK");
  lcd.setCursor(1, 1);
  lcd.print("OKE");
  lcd.setCursor(5, 1);
  lcd.print(nama);
  
  digitalWrite(selenoid, relay_on);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(100);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(100);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(4000);
  digitalWrite(selenoid, relay_off);

  // KIRIM NOTIF TELE
  myBot.sendMessage(id_bot, "Pintu telah dibuka oleh " + (String) nama);
  Serial.print("Pesan Terkirim\n\n");

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  cek_nama();

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AKSES : MASUK");
  lcd.setCursor(1, 1);
  lcd.print("OKE");
  lcd.setCursor(5, 1);
  lcd.print(nama);
  
  digitalWrite(selenoid, relay_on);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(100);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(100);
  digitalWrite(buzzer, relay_on);
  delay(200);
  digitalWrite(buzzer, relay_off);
  delay(4000);
  digitalWrite(selenoid, relay_off);

  // KIRIM NOTIF TELE
  myBot.sendMessage(id_bot, "Pintu telah dibuka oleh " + (String) nama);
  Serial.print("Pesan Terkirim\n\n");
  
  return finger.fingerID; 
}
