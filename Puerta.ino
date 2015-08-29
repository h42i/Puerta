#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Espanol.h>

#define STATE_PIN 12

char* ssid     = "ssid";
char* password = "passphrase";
char* broker   = "broker_adress/hostname";
int port       = 1883;

char* apiKey = "internal_api_key";
char* httpHost = "http_requests_host";

Espanol denada(ssid, password, broker, port, callback);

int state = 0;

void callback(char* topic, byte* payload, unsigned int length)
{
}

void setup()
{
    Serial.begin(115200);

    pinMode(STATE_PIN, INPUT_PULLUP);

    delay(50);

    state = digitalRead(STATE_PIN);
}

void loop()
{
    int newState = digitalRead(STATE_PIN);

    if (newState != state)
    {
        state = newState;

        if (!state)
        {
            denada.publish("hasi/state", "open");
        }
        else
        {
            denada.publish("hasi/state", "closed");
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

    denada.loop();
}

