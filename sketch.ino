#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Wokwi's simulated Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// AWS IoT Core details (Replace with your actual endpoint)
const char* aws_iot_endpoint = "a2ju9lqgnvmeel-ats.iot.us-east-1.amazonaws.com";
const int aws_iot_mqtt_port = 8883; // Standard MQTT port for AWS IoT Core

// Your AWS IoT Core Certificates and Private Key (as string literals)
const char AWS_CERT_CA[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
"MAkGA1UEBhMCVVMxDzANBgNVQQoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
"rqXRfboQnoZsG4q5WTP468SQvvG5\n"
"-----END CERTIFICATE-----\n";

const char AWS_CERT_CRT[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
"MIIDWTCCAkGgAwIBAgIUfuNTM2ihbH3pc2y49KtaCCiqgeowDQYJKoZIhvcNAQEL\n"
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDUzMDE1MTUy\n"
"MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANkCS7c43IWZgFDMYmCu\n"
"7rdrueOzXoVvUMGLTVBIIXWBY32JUL8f/LsAE5q3UndxvflXyyAaHKUYRLw9VZ++\n"
"QkhbOgZslmSzCIjsUY8kIbd1WkoMtx1KIQaciC10vtxfluHcVY1TgavnUTIlIUkd\n"
"V5POj/wkWFvqrBhkRUIMbr0SupVMxfzzpHk+maPI7nLb7WRO0dzDS+M91nryjJ3W\n"
"lJcVBKy+gy2iDPiCg8NWr16Z9NAknejsZ0vhEzVZG0O9S3Ld/0CiR2slvJym+DKP\n"
"u1PRo4+LHIMvO763hhfiCOh/ZezVuOQ26ArWRTrLbiQOt5RmeiaJov9ss6RvDkIR\n"
"Cs8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUlNJo2XQLuMdj3cH7kZqe/8pP3jwwHQYD\n"
"VR0OBBYEFDT4dz+Y/VQsPfGCtldb+IVIm2qyMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAlRtVzTGU71/6zRXQQn1aTZtCR\n"
"xeJMzZgUbdz37DN0zNz6GYxQc8SYGeHdTNe1a5fpYo5AicdUTkRcOLoQVGnrFWQ4\n"
"MaxWxa0mSRF9B+CEPDQtdGu2jQEJg4aTGq3IBip959GPIZZ+dPMLfPahtCyL9oDl\n"
"ATqElmwOKId4Olcf/5PWPk/tihxC9Ukflfkwyr5n7zY5CiJ5VChROPt/aLpXKk8z\n"
"gCPB+EMWzbjm+BuCczXdtq83bJ/2DfvU9NB1acp7QbOl3SOsgUjXFty6Rn9+aiES\n"
"27k58PVWfKPwQNVVaJdfdjRQmPaXu7yLnnDV7kZ1oXlpcGICdx4TiKcibDuP\n"
"-----END CERTIFICATE-----\n";

const char AWS_CERT_PRIVATE[] PROGMEM = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpQIBAAKCAQEA2QJLtzjchZmAUMxiYK7ut2u547NehW9QwYtNUEghdYFjfYlQ\n"
"vx/8uwATmrdSd3G9+VfLIBocpRhEvD1Vn75CSFs6BmyWZLMIiOxRjyQht3VaSgy3\n"
"HUohBpyILXS+3F+W4dxVjVOBq+dRMiUhSR1Xk86P/CRYW+qsGGRFQgxuvRK6lUzF\n"
"/POkeT6Zo8juctvtZE7R3MNL4z3WevKMndaUlxUErL6DLaIM+IKDw1avXpn00CSd\n"
"6OxnS+ETNVkbQ71Lct3/QKJHayW8nKb4Mo+7U9Gjj4scgy87vreGF+II6H9l7NW4\n"
"5DboCtZFOstuJA63lGZ6Jomi/2yzpG8OQhEKzwIDAQABAoIBAQDI1xUYN6nwg8Z8\n"
"AQyzinMokqzyDhnWyNUmqzQcrwU4aU2Y67Mtj/8pWsLKGAxcu4ok8Jnv1frY2QMU\n"
"bHeIZl+V88haFbmC7SdjAcPiBICn+BZeHjKDCw5RviFSYJAcCOPtmSwcpkKGxIkj\n"
"LbYGov9grnYX19NnDmPEF1j57eO3W5/ByEldj41BpczmcsMzuMyx/2gAGulUHZqR\n"
"CriyzvLctEJZKc58vgdFXdZNpWcntzsCgQOqqIKqwrWweAyFxVOWgWY3Xextr7Yy\n"
"lZcEXY2yi2J5OhT7+jwTnDv1Io6EvkY5/MTXlapEpxOaLnoZyFj1+K93E/zbJKK7\n"
"fqj/mrwxAoGBAO3+FbyvEC0RfUTZhhOxwZPY/RRmcABlyV9tkSCb0bAPFSWVSMuv\n"
"arWULQ27VEGE9ngqbVa1Du8KH2G0RB3C9RwEtvaSC7XQr9APyNGVKAuh6KdJKrh1\n"
"zjsGuYt0W8ZAgfJS6FTtDyAxM0x5APY6ifKracXHJ7c4LQOL4oc5EEJ1AoGBAOlt\n"
"wmxOAuPYjVjkOmOrp00df0xEISFn3dXP1FWw5kTbTVu9GEXWNNNn7Nc36kzoSod4\n"
"YKK7teh6mDxzraOR6PldRAKKupSPOYKNI4ghFC0QqM1BEIj3Rj5boDb0ZgqTNEPy\n"
"64NU+Fl4VhQgfD3RYgwAZ6RCVGxx0yILdYGDb+ezAoGBAMYa38s9SlDJoyXhj+rq\n"
"54EDeb82efrKv7JQySKEsipEk8zCFGOAdGoLmuk4Sy1y2ZszbepkMrcmDPNOk/jW\n"
"JSQahJaBAYBp4z9lNDvJVhlAwCIoFIiRf4bA1UhRJmfDoKeu/bhb0GBytAnvG67b\n"
"EaNPrm80lUm9WDMAiSMYjAFVAoGBALCJTezS4uSmNd/wWyhd/V0Y3dkz/UpiXycA\n"
"0rkr3DpfEJY84EqQwcrQYhIyO4SGeTcTFyJ7jAuSC+hMJBn8k5Fjs7eoa4tG9luB\n"
"mkmtmrCqVL9GYdb0C6/FY9mr5DBDO9fXs73TWS9xDsiR+ORXoispcziK4YZSXzb0\n"
"K54TZWJ5AoGAOhps8j6GZByMqsMQkXx1vZ9cqMNDn2axNnwFjd27HL8mQH2jLSy7\n"
"tIXst7rgJd1JtcaG6rWy6K7tQTcQhjlkjwdGt7kj9taHDugcPQ1CgHoxmTcwXiHC\n"
"F35K656oA1fgI4AbUrLJLB4xMoUnqk43A8PCvYiwsntLr93Wc1TXJ1I=\n"
"-----END RSA PRIVATE KEY-----\n";

// MQTT topics
const char* pub_topic = "smartIrrigation/data";
const char* sub_topic = "smartIrrigation/commands";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// Pin definitions - MAKE SURE THIS MATCHES YOUR WOKWI SETUP
const int SOIL_MOISTURE_PIN = 36; // Analog pin for NTC
const int RELAY_PIN = 2;          // Digital pin for relay (pump)
const int PUMP_INDICATOR_LED = 4; // LED to show pump status

long lastMsg = 0;
char msgBuffer[50];

void setup_wifi() {
  delay(10);
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to AWS IoT Core...");

    // Set up TLS/SSL client certificates
    espClient.setCACert(AWS_CERT_CA);
    espClient.setCertificate(AWS_CERT_CRT);
    espClient.setPrivateKey(AWS_CERT_PRIVATE);
    client.setClient(espClient); // Tell PubSubClient to use the secure client

    // Connect with your AWS IoT Thing Name as the client ID
    if (client.connect("MySmartIrrigationDevice")) { // Replace with your Thing Name
      Serial.println("connected to AWS IoT Core");
      client.publish(pub_topic, "ESP32 connected to AWS IoT Core");
      client.subscribe(sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  if (String(topic) == sub_topic) {
    if (message.indexOf("ON") >= 0) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(PUMP_INDICATOR_LED, HIGH);
      Serial.println("Pump ON command received!");
    } else if (message.indexOf("OFF") >= 0) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(PUMP_INDICATOR_LED, LOW);
      Serial.println("Pump OFF command received!");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PUMP_INDICATOR_LED, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(PUMP_INDICATOR_LED, LOW);

  setup_wifi();
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) { // Publish every 5 seconds
    lastMsg = now;

    // Read NTC value - Wokwi simulates sensor data
    int sensorValue = analogRead(SOIL_MOISTURE_PIN);
    int moisturePercentage = map(sensorValue, 0, 4095, 0, 100); // Adjust mapping as needed

    snprintf(msgBuffer, 50, "{\"moisture\": %d}", moisturePercentage);
    Serial.print("Publishing moisture: ");
    Serial.println(msgBuffer);
    client.publish(pub_topic, msgBuffer);

    // Basic auto-irrigation logic (optional)
    if (moisturePercentage > 60) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(PUMP_INDICATOR_LED, HIGH);
      client.publish(pub_topic, "{\"pump_status\": \"ON\"}");
    } else if (moisturePercentage < 30) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(PUMP_INDICATOR_LED, LOW);
      client.publish(pub_topic, "{\"pump_status\": \"OFF\"}");
    }
  }
  delay(10); // Small delay to prevent busy-waiting
}