#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define DATA_PIN 13
#define CS_PIN 12
#define CLK_PIN 14

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

const char* cryptocurrencies[] = {"cardano", "bitcoin", "ethereum", "litecoin", "ripple", "bitcoin-cash"};
const int numCryptocurrencies = 6;

const char* ssid = "php";
const char* password = "1234567890";

void scrollText(const char* text, int speed) {
  myDisplay.displayText(text, PA_CENTER, 0, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (!myDisplay.displayAnimate()) {
    delay(speed);
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connexion au WiFi en cours...");
  }
  Serial.println("Connecté au WiFi");

  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 0; i < numCryptocurrencies; i++) {
      HTTPClient http;
      String API = "https://api.coingecko.com/api/v3/simple/price?ids=" + String(cryptocurrencies[i]) + "&vs_currencies=usd&";
      http.begin(API);

      int httpCode = http.GET();

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println("Prix actuel du " + String(cryptocurrencies[i]) + " : " + extractBitcoinPrice(payload));

          // Affiche le nom de la cryptomonnaie avec défilement
          scrollText(cryptocurrencies[i],90);
          delay(900);
          myDisplay.print(extractBitcoinPrice(payload));

          // Ajoutez une petite pause pour éviter le problème du buffer
          delay(500);

          // Affiche le prix pendant 8 secondes
          delay(8000);
          myDisplay.displayClear();
        }
      } else {
        Serial.println("Erreur lors de la requête HTTP pour" + String(cryptocurrencies[i]));
      }

      http.end();
    }
  }
}

String extractBitcoinPrice(String json) {
  int start = json.indexOf("\"usd\":") + 6;  // Recherche la position du début de la sous-chaîne "\"usd\":"
  int end = json.indexOf("}", start);       // Recherche la position de la fin de la sous-chaîne
  String price = json.substring(start, end); // Extrait la partie de la chaîne entre start et end
  price.replace(".", ",");                   // Supprime les guillemets

  // Limite le prix aux 5 premiers chiffres
  int commaIndex = price.indexOf(",");
  if(commaIndex != -1) {
     price = price.substring(0, 6);
  } else {
     price = price.substring(0, 5);
  }
  return price;
}
