#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Espanol.h>

#define STATE_PIN 12

// WiFi-Config
const String ssid     = "HaSi-Kein-Internet-Legacy";
const String password = "";
const String broker   = "mqtt.hasi";
int port       = 1883;

// Service-Config
const String apiKey = "";
const String httpHost = "status.hasi.it";

// Variables
bool state = false;

void callback(char* topic, byte* payload, unsigned int length)
{
}

void setup()
{
    Serial.begin(115200);

    pinMode(STATE_PIN, INPUT_PULLUP);

    delay(50);

    state = digitalRead(STATE_PIN) == HIGH;

    Espanol.setDebug(true);
    Espanol.begin(ssid, password, "raumstatus", broker, port);
}

void loop()
{
    // Integrate debouncing
    bool stateRead1 = digitalRead(STATE_PIN) == HIGH;
    delay(20);
    bool stateRead2 = digitalRead(STATE_PIN) == HIGH;
    bool newState = stateRead1 && stateRead2;

    if (newState != state)
    {
        state = newState;

        if (!state)
        {
            Espanol.publish("hasi/state", "open");
        }
        else
        {
            Espanol.publish("hasi/state", "closed");
        }

        WiFiClient client;
        int httpConnected = client.connect(httpHost, 80);

        String httpPayload = String("{ \"api_key\": \"") + apiKey + "\", \"value\": " + (!state ? 1 : 0) + " }";

        String httpPostRequest = String("POST /api/sensors/open HTTP/1.1\n") +
                                 "Host: " + httpHost + "\n" +
                                 "Content-Type: application/json\n" +
                                 "Content-Length: " + httpPayload.length() + "\n" +
                                 "Connection: close\n\n" +
                                 httpPayload;

        if (httpConnected)
        {
            client.print(httpPostRequest);
        }
    }

    delay(0);

    Espanol.loop();
}
