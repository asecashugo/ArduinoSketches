#include <BlynkSimpleMKR1000.h>
#include <SimpleTimer.h>

char auth[] = "2662a762d3b344c9be0979268af1b4df";
char ssid[] = "HugoyMarieT";
char pass[] = "nomejodas";

WidgetLCD lcd(V1);
SimpleTimer timer;

void setup()
{
  pinMode(6, OUTPUT);

  digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
      digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
      digitalWrite(6, HIGH);
  delay(500);
  digitalWrite(6, LOW);
    delay(500);
    
  Serial.print("Connecting...");
  digitalWrite(6, LOW);
  Serial.println("Connecting ahora...");
  Blynk.begin(auth, ssid, pass, "rpihugo.ddns.net",8080);
//  while (Blynk.connect() == false) {}
  lcd.clear();
  digitalWrite(6, HIGH);
  lcd.print(0, 0, "Connected...");
  Serial.println("Connected...");
  timer.setInterval(1000L, sendText);
}

void sendText()
{
  lcd.print(0, 1, "Hello World");
  Serial.println("Connected...");
}

void loop()
{
  Blynk.run();
  timer.run();
}
