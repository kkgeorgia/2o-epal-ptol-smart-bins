#include <WiFiS3.h>
#include <ThingSpeak.h>

char ssid[] = "WIFI_SSID";       
char pass[] = "WIFI_PASSWORD";

WiFiClient client;

unsigned long channelID = [Channel ID];  
const char* writeAPIKey = "Write API Key";

#define TRIG_PIN 9
#define ECHO_PIN 10
#define TILT_PIN 2  
#define DHTPIN 5     
#define DHTTYPE DHT22  
#include <DHT.h> 

DHT dht(DHTPIN, DHTTYPE);
void setup() {
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(TILT_PIN, INPUT);
    Serial.println("DHTxx test!");
    dht.begin();

    WiFi.begin(ssid, pass);
    Serial.print("Σύνδεση στο WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nΣυνδέθηκε στο WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Ενεργοποίηση ThingSpeak
    ThingSpeak.begin(client);
}

void loop() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;  // Υπολογισμός απόστασης σε cm
    
    Serial.print("Απόσταση: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance<4){
      Serial.println("Ο κάδος είναι γεμάτος!");
      }

    int state = digitalRead(TILT_PIN);  

    if (state == HIGH) {
        Serial.println("Κλίση ανιχνεύτηκε! Πιθανή ανατροπή κάδου...");
    } else {
        Serial.println("Χωρίς κλίση.");
    }

    float t = dht.readTemperature();
    Serial.print("Θερμοκρασία: ");
    Serial.print(t);
    Serial.print(" *C \n");
    
    if (t>70){
      Serial.print("Προσοχή!! κίνδυνος πυρκαγιάς...");
      }

      // Αποστολή στο ThingSpeak
    ThingSpeak.setField(1, distance);   // πληρότητα κάδου
    ThingSpeak.setField(2, state);   // κλίση
    ThingSpeak.setField(3, t);  // θερμοκρασία
    
    int x = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (x == 200) {
        Serial.println("Επιτυχής αποστολή στο ThingSpeak.");
    } else {
        Serial.print("Σφάλμα αποστολής. Κωδικός: ");
        Serial.println(x);
    }
   
    delay(20000);  // Αναμονή 20 δευτερόλεπτα  

}
