#include <M5Core2.h>
#include <TinyGPS++.h>
#include <WiFi.h>

TinyGPSPlus gps;
HardwareSerial GPSModule(1);

const int numLines = 7; // Number of data lines (excluding the "Created by" line)
char displayBuffer[numLines][100];
char nextBuffer[numLines][100];

void setup() {
    M5.begin();
    Serial.begin(115200);
    GPSModule.begin(9600, SERIAL_8N1, 13, 14); // RX, TX pins for GPS

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);

    WiFi.mode(WIFI_STA); // Set WiFi to station mode for scanning
    WiFi.disconnect();   // Ensure WiFi is not connected to any network

    memset(displayBuffer, 0, sizeof(displayBuffer)); // Initialize buffer
    memset(nextBuffer, 0, sizeof(nextBuffer)); // Initialize next buffer
}

void loop() {
    while (GPSModule.available() > 0) {
        if (gps.encode(GPSModule.read())) {
            displayGPSInfo();
        }
    }

    scanForWiFiNetworks(); // Scan for WiFi Networks
    updateDisplay(); // Update display from buffer
    displayCreatorInfo(); // Display creator info
    M5.update();
}

void displayGPSInfo() {
    // GPS info update logic
    sprintf(nextBuffer[0], "Latitude: %.6f", gps.location.lat());
    sprintf(nextBuffer[1], "Longitude: %.6f", gps.location.lng());
    sprintf(nextBuffer[2], "Altitude: %.2f ft", gps.altitude.meters() * 3.281); // Convert to feet
    sprintf(nextBuffer[3], "Satellites: %d", gps.satellites.value());
    sprintf(nextBuffer[4], "Speed: %.2f mph", gps.speed.kmph() * 0.621371); // Convert to mph

    if (gps.date.isValid() && gps.time.isValid()) {
        int hour = gps.time.hour() - 5; // Adjust for EST
        if (hour < 0) hour += 24;
        bool isPM = hour >= 12;
        if (hour > 12) hour -= 12;
        if (hour == 0) hour = 12;
        sprintf(nextBuffer[5], "%02d/%02d/%04d %02d:%02d:%02d %s",
                gps.date.month(), gps.date.day(), gps.date.year(),
                hour, gps.time.minute(), gps.time.second(),
                isPM ? "PM" : "AM");
    }
}

void scanForWiFiNetworks() {
    int n = WiFi.scanNetworks();
    sprintf(nextBuffer[6], "WiFi Networks: %d", n);
    WiFi.scanDelete();
}

void updateDisplay() {
    for (int i = 0; i < numLines; i++) {
        if (strcmp(displayBuffer[i], nextBuffer[i]) != 0) {
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setCursor(0, i * 20);
            M5.Lcd.fillRect(0, i * 20, 320, 20, BLACK); // Clear the line
            M5.Lcd.print(nextBuffer[i]);
            strcpy(displayBuffer[i], nextBuffer[i]); // Copy to current buffer
        }
    }
}

void displayCreatorInfo() {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setCursor(0, M5.Lcd.height() - 20);
    M5.Lcd.fillRect(0, M5.Lcd.height() - 20, 320, 20, BLACK); // Clear the line
    M5.Lcd.print("Created by @Ringmast4r");
}
