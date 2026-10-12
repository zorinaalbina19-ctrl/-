#define BUTTON_PIN 2
#define LED_PIN 13

volatile bool interruptOccurred = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  attachInterrupt(0, buttonISR, FALLING);
}

void loop() {
  if (interruptOccurred) {
    interruptOccurred = false;
    
    Serial.println("=== Прерывание произошло! ===");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

void buttonISR() {
  interruptOccurred = true;
}