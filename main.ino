#include <WiFi.h>
#include <Espalexa.h>

#define POWER_SWITCH_PIN 2 // Pin to control the power switch
#define POWER_LED_PIN 4    // Pin connected to the PC's power LED
#define POWER_STATUS_PIN 18 // Pin connected to the optocoupler's collector to read power status


const char* ssid = "The force";
const char* password = "hefternettwix"; 


Espalexa espalexa;

// Event handler for successful WiFi connection
void ConnectedToAP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Connected To The WiFi Network");
}

// Event handler for receiving an IP address from DHCP
void GotIP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

// Event handler for WiFi disconnection event
void WiFi_Disconnected_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Disconnected From WiFi Network");
  // Attempt Re-Connection
  WiFi.begin(ssid, password);
}

void setup() {
  // Set pin modes for optocoupler pins
  pinMode(POWER_SWITCH_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(POWER_STATUS_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  // Start serial communication at 115200 baud rate
  Serial.begin(115200);

  // Set up WiFi connection
  WiFi.mode(WIFI_STA); // Set WiFi mode to station (client)
  WiFi.onEvent(ConnectedToAP_Handler, ARDUINO_EVENT_WIFI_STA_CONNECTED); // Register event handler for connected event
  WiFi.onEvent(GotIP_Handler, ARDUINO_EVENT_WIFI_STA_GOT_IP); // Register event handler for got IP event
  WiFi.onEvent(WiFi_Disconnected_Handler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED); // Register event handler for disconnected event
  WiFi.begin(ssid, password); // Begin connection to WiFi network
  Serial.println("Connecting to WiFi Network ..");

  // Add Alexa device and define callback function for control commands
  espalexa.addDevice("PC", powerControlCallback);

  // Start Espalexa service to listen for Alexa commands
  espalexa.begin();
}

void loop() {
  // Handle any incoming Alexa commands
  espalexa.loop();
}

// Callback function to control power state via Alexa command
void powerControlCallback(EspalexaDevice* device) {
  bool deviceState = device->getValue(); // Get desired state from Alexa command
  bool powerStatus = !digitalRead(POWER_STATUS_PIN); // Read current power status (inverted logic)
  // If Alexa command is ON and PC is currently OFF, toggle power switch to turn ON PC
  if (deviceState && !powerStatus) {
    digitalWrite(POWER_SWITCH_PIN, HIGH);
    delay(100); // Simulate button press for 100ms
    digitalWrite(POWER_SWITCH_PIN, LOW);
    digitalWrite(POWER_LED_PIN, HIGH); // Turn on the LED indicator

  }
  
   // If Alexa command is OFF and PC is currently ON, toggle power switch to turn OFF PC
   else if (!deviceState && powerStatus) {
    digitalWrite(POWER_SWITCH_PIN, HIGH);
    delay(100); // Simulate button press for 100ms
    digitalWrite(POWER_SWITCH_PIN, LOW);
    digitalWrite(POWER_LED_PIN, LOW); // Turn off the LED indicator
   }
}
