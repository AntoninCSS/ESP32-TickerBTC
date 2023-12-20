#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
// Penentuan pin serial
#define MAX_DEVICES 4
#define DATA_PIN 13
#define CS_PIN 12
#define CLK_PIN 14

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);


const char* ssid = "php";
const char* password = "1234567890";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi en cours...");
  }

  Serial.println("Connecté au WiFi");


    // Memulai object
    myDisplay.begin();
    // Mengatur intensitas (0-15)
    myDisplay.setIntensity(0);
    // Membersihkan tampilan
    myDisplay.displayClear();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Remplacez l'URL de l'API par celle que vous souhaitez utiliser
    http.begin("https://api.coindesk.com/v1/bpi/currentprice/BTC.json");

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Prix actuel du Bitcoin : " + extractBitcoinPrice(payload));
        myDisplay.print(extractBitcoinPrice(payload));
      }
    } else {
      Serial.println("Erreur lors de la requête HTTP");
    }

    http.end();
  }

  delay(60000); // Attendez 1 minute avant de récupérer à nouveau le prix
}

String extractBitcoinPrice(String json) {
  int start = json.indexOf("\"rate\":\"") + 8;
  int end = json.indexOf(".", start);
  String price = json.substring(start, end);
  price.replace(",", "");
  return price;
}
