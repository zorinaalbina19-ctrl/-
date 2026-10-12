#define BUTTON_PIN 2
#define LED_PIN 13

volatile bool buttonPressed = false;

bool idleLedState = false;
unsigned long idleLastToggle = 0;
const unsigned long IDLE_INTERVAL = 200;

bool fastLedState = false;
unsigned long fastLastToggle = 0;
int blinkCount = 0;
const unsigned long FAST_INTERVAL = 500;

unsigned long pauseStartTime = 0; //пауза
const unsigned long PAUSE_DURATION = 5000;

byte state = 0; //состояние, где 0 это постоянное мигание, 1 это 1гцх5, 2 пауза 5сек

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(0, buttonISR, FALLING);

  idleLastToggle = millis();
  idleLedState = true;
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  unsigned long currentTime = millis();

  if (buttonPressed) {
    buttonPressed = false;

    state = 1;
    fastLastToggle = currentTime;
    blinkCount = 0;
    fastLedState = true;
    digitalWrite(LED_PIN, HIGH);
  }

  if (state == 0) {
    if (currentTime - idleLastToggle >= IDLE_INTERVAL) {
      idleLastToggle = currentTime;
      idleLedState = !idleLedState;
      digitalWrite(LED_PIN, idleLedState);
    }
  }
  else if (state == 1) {
    if (currentTime - fastLastToggle >= FAST_INTERVAL) {
      fastLastToggle = currentTime;
      fastLedState = !fastLedState;
      digitalWrite(LED_PIN, fastLedState);

      if (!fastLedState) {
        blinkCount++;
        if (blinkCount >= 5) {
          state = 2;
          pauseStartTime = currentTime;
          digitalWrite(LED_PIN, LOW);
        }
      }
    }
  }
  else if (state == 2) {
    if (currentTime - pauseStartTime >= PAUSE_DURATION) {
      state = 0;
      idleLastToggle = currentTime;
      idleLedState = true;
      digitalWrite(LED_PIN, HIGH);
    }
  }
}

void buttonISR() {
  buttonPressed = true;
}