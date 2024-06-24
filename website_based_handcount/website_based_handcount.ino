#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR SSID";
const char* password = "********";
const int buttonPin1 = D1;  // Pin connected to the first push-button
const int buttonPin2 = D5;  // Pin connected to the second push-button
int buttonState1 = 0;       // Current state of the first button
int buttonState2 = 0;       // Current state of the second button
int lastButtonState1 = 0;   // Previous state of the first button
int lastButtonState2 = 0;   // Previous state of the second button
int pressCount1 = 0;        // Counter for presses on the first button
int pressCount2 = 0;        // Counter for presses on the second button
unsigned long lastDebounceTime1 = 0;  // Last time the button 1 state was toggled
unsigned long lastDebounceTime2 = 0;  // Last time the button 2 state was toggled
unsigned long debounceDelay = 50;     // Debounce delay time in milliseconds
ESP8266WebServer server(80);

void handleRoot() {
  String content = "<html><head><meta http-equiv=\"refresh\" content=\"5\">";
  content += "<style>";
  content += ".box:nth-child(odd) {";
  content += "  background-color: #f0f0f0;"; // Light gray background for odd numbered boxes
  content += "}";
  content += ".box:nth-child(even) {";
  content += "  background-color: #e0e0e0;"; // Lighter gray background for even numbered boxes
  content += "}";
  content += ".box {";
  content += "  border: 1px solid #032;";
  content += "  padding: 20px;";
  content += "  width: 200px;";
  content += "  margin: 10px auto;";
  content += "  text-align: center;";
  content += "}";
  content += "</style>";
  content += "</head><body>";
  content += "<h1>Button Press Counts</h1>";
  content += "<div class=\"box\"><p>Left button press count:</p><span id=\"pressCount1\">" + String(pressCount1) + "</span></div>";
  content += "<div class=\"box\"><p>Right button press count:</p><span id=\"pressCount2\">" + String(pressCount2) + "</span></div>";
  content += "<script>function updateCounts() {";
  content += "var xhttp = new XMLHttpRequest();";
  content += "xhttp.onreadystatechange = function() {";
  content += "if (this.readyState == 4 && this.status == 200) {";
  content += "var counts = JSON.parse(this.responseText);";
  content += "document.getElementById(\"pressCount1\").innerText = counts.pressCount1;";
  content += "document.getElementById(\"pressCount2\").innerText = counts.pressCount2;";
  content += "}";
  content += "};";
  content += "xhttp.open(\"GET\", \"/counts\", true);";
  content += "xhttp.send();";
  content += "}";
  content += "setInterval(updateCounts, 10);</script>";
  content += "</body></html>";
  server.send(200, "text/html", content);
}



void handleCounts() {
  String countsJson = "{\"pressCount1\":" + String(pressCount1) + ",\"pressCount2\":" + String(pressCount2) + "}";
  server.send(200, "application/json", countsJson);
}

void setup() {
  pinMode(4,OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);  // Set the first button pin as input with internal pull-up resistor
  pinMode(buttonPin2, INPUT_PULLUP);  // Set the second button pin as input with internal pull-up resistor
  Serial.begin(9600);

  // Connect to Wi-Fi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print ESP8266 IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the web server
  server.on("/", handleRoot);
  server.on("/counts", HTTP_GET, handleCounts); // Endpoint for fetching counts
  server.begin();
}

void loop() {
  server.handleClient();

  unsigned long currentMillis = millis(); // Get the current time

  // Read the state of the first button
  int reading1 = digitalRead(buttonPin1);
  // Check for a change in the state of the first button
  if (reading1 != lastButtonState1) {
    // Reset the debounce timer
    lastDebounceTime1 = currentMillis;
  }
  if ((currentMillis - lastDebounceTime1) > debounceDelay) {
    // Button state has stabilized
    if (reading1 != buttonState1) {
      buttonState1 = reading1;
      // Check if the button 1 state has changed
      if (buttonState1 == LOW) {
            digitalWrite(4,HIGH);
    delay(300);
        pressCount1++;
        Serial.print("Left button pressed! Press count: ");
        Serial.println(pressCount1);
    digitalWrite(4,LOW);
        // Button 1 is pressed

      }
    }
  }
  // Update lastButtonState1
  lastButtonState1 = reading1;

  // Read the state of the second button
  int reading2 = digitalRead(buttonPin2);
  // Check for a change in the state of the second button
  if (reading2 != lastButtonState2) {

    lastDebounceTime2 = currentMillis;

  }
  if ((currentMillis - lastDebounceTime2) > debounceDelay) {
    // Button state has stabilized
    if (reading2 != buttonState2) {
      buttonState2 = reading2;
      // Check if the button 2 state has changed
      if (buttonState2 == LOW) {
        // Button 2 is pressed
            digitalWrite(4,HIGH);
    delay(300);
        pressCount2++;
        Serial.print("Right button pressed! Press count: ");
        Serial.println(pressCount2);
    digitalWrite(4,LOW);
    delay(10);

      }
    }
  }
  // Update lastButtonState2
  lastButtonState2 = reading2;
}
