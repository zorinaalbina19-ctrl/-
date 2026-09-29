enum State { S_GREEN, S_YELLOW, S_RED, S_RED_YELLOW, S_WARNING };
State currentState = S_GREEN;
unsigned long stateStartMillis = 0;
unsigned long stateDuration    = 0;
bool pedRequest = false;
bool emergency  = false;

int lastBtn3 = HIGH, stableBtn3 = HIGH; unsigned long tBtn3 = 0;
int lastBtn2 = HIGH, stableBtn2 = HIGH; unsigned long tBtn2 = 0;

unsigned long lastBlink = 0;
bool blinkOn = false;

void setOutputsForState(State s) {
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);

  switch (s) {
    case S_GREEN:      digitalWrite(11, HIGH); break;
    case S_YELLOW:     digitalWrite(12, HIGH); break;
    case S_RED:        digitalWrite(13, HIGH); break;
    case S_RED_YELLOW: digitalWrite(13, HIGH);
                       digitalWrite(12, HIGH); break;
    case S_WARNING:    blinkOn = false; lastBlink = millis(); break;
  }
}

void handleEventsInState(State s) {
  (void)s;

  int r3 = digitalRead(3);
  if (r3 != lastBtn3) { tBtn3 = millis(); lastBtn3 = r3; }
  if ((millis() - tBtn3) > 50 && r3 != stableBtn3) {
    stableBtn3 = r3;
    if (stableBtn3 == LOW) {
      emergency = !emergency;
    }
  }

  int r2 = digitalRead(2);
  if (r2 != lastBtn2) { tBtn2 = millis(); lastBtn2 = r2; }
  if ((millis() - tBtn2) > 50 && r2 != stableBtn2) {
    stableBtn2 = r2;
    if (stableBtn2 == LOW) {
      pedRequest = true;
    }
  }
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  currentState = S_GREEN;
  stateStartMillis = millis();
  stateDuration = 10000;
  setOutputsForState(S_GREEN);
}

void loop() {
  handleEventsInState(currentState);

  if (emergency) {
    if (currentState != S_WARNING) {
      currentState = S_WARNING;
      stateStartMillis = millis();
      stateDuration = 500;
      setOutputsForState(S_WARNING);
    }
  } else {
    if (currentState == S_WARNING) {
      currentState = S_GREEN;
      stateStartMillis = millis();
      stateDuration = 10000;
      setOutputsForState(S_GREEN);
    }
  }

  switch (currentState) {

    case S_GREEN:
      if (millis() - stateStartMillis >= stateDuration) {
        currentState = S_YELLOW;
        stateStartMillis = millis();
        stateDuration = 3000;
        setOutputsForState(S_YELLOW);
      }
      break;

    case S_YELLOW:
      if (millis() - stateStartMillis >= stateDuration) {
        currentState = S_RED;
        stateStartMillis = millis();
        if (pedRequest) {
          pedRequest = false;
          stateDuration = 15000;
        } else {
          stateDuration = 10000;
        }
        setOutputsForState(S_RED);
      }
      break;

    case S_RED:
      if (millis() - stateStartMillis >= stateDuration) {
        currentState = S_RED_YELLOW;
        stateStartMillis = millis();
        stateDuration = 2000;
        setOutputsForState(S_RED_YELLOW);
      }
      break;

    case S_RED_YELLOW:
      if (millis() - stateStartMillis >= stateDuration) {
        currentState = S_GREEN;
        stateStartMillis = millis();
        stateDuration = pedRequest ? 5000 : 10000;
        setOutputsForState(S_GREEN);
      }
      break;

    case S_WARNING:
      if (millis() - lastBlink >= 500) {
        lastBlink = millis();
        blinkOn = !blinkOn;
        digitalWrite(12, blinkOn ? HIGH : LOW);
      }
      break;
  }
}
