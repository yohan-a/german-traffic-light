/**
 * @author Yohan Amaratunga, Anirudh Gopishankar
 * @brief Final project for Mechatronics Lab. A web-controlled German-style traffic light.
 * @date August 8, 2025
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FS.h" // Used for reading files from the ESP8266's flash memory

// --- Configuration Constants ---
const char* WIFI_SSID = "Y";
const char* WIFI_PASSWORD = "yoyo54321";

const int PIN_LED_RED = D1;
const int PIN_LED_YELLOW = D2;
const int PIN_LED_GREEN = D3;

// --- State   & Global Variables ---
enum LightState {
  STATE_RED,
  STATE_GREEN,
  STATE_YELLOW_SOLID,
  STATE_YELLOW_BLINK,
  STATE_RED_YELLOW_TRANSITION
};

LightState currentLightState = STATE_RED; 

// Used for the non-blocking timer logic, which prevents the use of delay()
unsigned long previousMillis = 0;
bool blinkLedState = false;

// Durations for each timed light phase
const long DURATION_YELLOW_SOLID = 2000;
const long DURATION_RED_YELLOW = 1500;
const long DURATION_BLINK_INTERVAL = 500;

// --- Web Server ---
ESP8266WebServer server(80); // Listen on the standard HTTP port 80

// --- Helper Functions ---

/**
 * @brief Sets the physical state of the three LEDs.
 * A helper function to keep the main loop clean.
 */
void setPhysicalLeds(bool red, bool yellow, bool green) {
  digitalWrite(PIN_LED_RED, red);
  digitalWrite(PIN_LED_YELLOW, yellow);
  digitalWrite(PIN_LED_GREEN, green);
}

// --- Web Server Handlers ---

/**
 * @brief Handles requests to the root URL ("/") by sending the main webpage from SPIFFS.
 */
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    server.send(404, "text/plain", "File Not Found");
    return;
  }
  server.streamFile(file, "text/html");
  file.close();
}

/**
 * @brief Handles requests to "/state" and reports the current light state to the webpage.
 */
void handleState() {
  String stateString = "";
  switch (currentLightState) {
    case STATE_RED:                   stateString = "red"; break;
    case STATE_GREEN:                 stateString = "green"; break;
    case STATE_YELLOW_SOLID:          stateString = "yellow"; break;
    case STATE_RED_YELLOW_TRANSITION: stateString = "red,yellow"; break;
    case STATE_YELLOW_BLINK:
      if (blinkLedState) { stateString = "yellow"; } else { stateString = ""; }
      break;
  }
  server.send(200, "text/plain", stateString);
}

/**
 * @brief Handles POST requests to "/set" to manually change the light state from the webpage.
 */
void handleSet() {
  if (!server.hasArg("light")) {
    server.send(400, "text/plain", "Bad Request");
    return;
  }
  String lightColor = server.arg("light");

  if (lightColor == "red" && (currentLightState == STATE_GREEN || currentLightState == STATE_YELLOW_BLINK)) {
    currentLightState = STATE_YELLOW_SOLID;
    previousMillis = millis();
  }
  else if (lightColor == "green" && (currentLightState == STATE_RED || currentLightState == STATE_YELLOW_BLINK)) {
    currentLightState = STATE_RED_YELLOW_TRANSITION;
    previousMillis = millis();
  }
  else if (lightColor == "yellow" && currentLightState != STATE_YELLOW_BLINK) {
    currentLightState = STATE_YELLOW_BLINK;
    previousMillis = millis();
    blinkLedState = false;
  }
  server.send(200, "text/plain", "OK");
}

// --- Main Program ---

/**
 * @brief Main setup function, runs once on boot.
 */
void setup() {
  Serial.begin(115200);

  // Initialize the SPIFFS file system
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialize hardware
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  setPhysicalLeds(true, false, false); // Start with red on

  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize Web Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/state", HTTP_GET, handleState);
  server.on("/set", HTTP_POST, handleSet);
  server.begin();
  Serial.println("HTTP server started");
}

/**
 * @brief Main program loop, runs continuously.
 */
void loop() {
  // This must be called in every loop to handle client requests
  server.handleClient();

  unsigned long currentMillis = millis();

  // The main state machine logic
  switch (currentLightState) {
    case STATE_RED:
      setPhysicalLeds(true, false, false);
      break;

    case STATE_GREEN:
      setPhysicalLeds(false, false, true);
      break;

    case STATE_YELLOW_SOLID: // A timed transition state
      setPhysicalLeds(false, true, false);
      if (currentMillis - previousMillis >= DURATION_YELLOW_SOLID) {
        currentLightState = STATE_RED; // After yellow, go to red
      }
      break;

    case STATE_RED_YELLOW_TRANSITION: // A timed transition state
      setPhysicalLeds(true, true, false);
      if (currentMillis - previousMillis >= DURA=TION_RED_YELLOW) {
        currentLightState = STATE_GREEN; // After red+yellow, go to green
      }
      break;

    case STATE_YELLOW_BLINK: // A continuous state until interrupted by user
      if (currentMillis - previousMillis >= DURATION_BLINK_INTERVAL) {
        previousMillis = currentMillis;
        blinkLedState = !blinkLedState; // Invert the blink state
        setPhysicalLeds(false, blinkLedState, false);
      }
      break;
  }
}
