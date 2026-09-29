enum State { S_GREEN, S_YELLOW, S_RED, S_RED_YELLOW, S_WARNING, NUM_STATES };
enum Event { E_NONE, E_TIMER, E_EMERGENCY_ON, E_EMERGENCY_OFF, NUM_EVENTS };

State currentState = S_GREEN;
unsigned long stateStart = 0;
unsigned long stateDur   = 0;
bool pedRequest = false;
bool emergency  = false;

int lastBtn3 = HIGH, stableBtn3 = HIGH; unsigned long tBtn3 = 0;
int lastBtn2 = HIGH, stableBtn2 = HIGH; unsigned long tBtn2 = 0;

unsigned long lastBlink = 0;
bool blinkOn = false;

typedef void (*Handler)(void);
Handler fsmTable[NUM_STATES][NUM_EVENTS];

void setOutputsForState(State s) {
  digitalWrite(13, LOW); digitalWrite(12, LOW); digitalWrite(11, LOW);
  switch (s) {
    case S_GREEN:      digitalWrite(11, HIGH); break;
    case S_YELLOW:     digitalWrite(12, HIGH); break;
    case S_RED:        digitalWrite(13, HIGH); break;
    case S_RED_YELLOW: digitalWrite(13, HIGH); digitalWrite(12, HIGH); break;
    case S_WARNING:    blinkOn = false; lastBlink = millis(); break;
    default: break;
  }
}

void goTo(State s, unsigned long dur, const char* reason) {
  currentState = s;
  stateStart = millis();
  stateDur = dur;
  setOutputsForState(s);
  Serial.print("[STATE] "); Serial.print(s);
  Serial.print(" | t="); Serial.print(stateStart);
  Serial.print(" | "); Serial.println(reason);
}

void onGreenTimer()   { goTo(S_YELLOW, 3000, pedRequest ? "green done (ped)" : "green done"); }
void onYellowTimer()  {
  if (pedRequest) { pedRequest = false; goTo(S_RED, 20000, "yellow done -> long red"); }
  else            { goTo(S_RED, 10000, "yellow done"); }
}
void onRedTimer()     { goTo(S_RED_YELLOW, 2000, "red done"); }
void onRedYellowTimer(){ goTo(S_GREEN, pedRequest ? 5000 : 10000, "red+yellow done"); }
void onEmergencyOn()  { emergency = true;  goTo(S_WARNING, 500, "emergency on"); }
void onEmergencyOff() { emergency = false; goTo(S_GREEN,   10000, "emergency off"); }

void setupTable() {
  for (int s = 0; s < NUM_STATES; ++s)
    for (int e = 0; e < NUM_EVENTS; ++e) fsmTable[s][e] = NULL;

  fsmTable[S_GREEN][E_TIMER]      = onGreenTimer;
  fsmTable[S_YELLOW][E_TIMER]     = onYellowTimer;
  fsmTable[S_RED][E_TIMER]        = onRedTimer;
  fsmTable[S_RED_YELLOW][E_TIMER] = onRedYellowTimer;

  for (int s = 0; s < NUM_STATES; ++s) {
    fsmTable[s][E_EMERGENCY_ON]  = onEmergencyOn;
    fsmTable[s][E_EMERGENCY_OFF] = onEmergencyOff;
  }
}

Event pollEvent() {
  int r3 = digitalRead(3);
  if (r3 != lastBtn3) { tBtn3 = millis(); lastBtn3 = r3; }
  if ((millis() - tBtn3) > 50 && r3 != stableBtn3) {
    stableBtn3 = r3;
    if (stableBtn3 == LOW) return emergency ? E_EMERGENCY_OFF : E_EMERGENCY_ON;
  }
  int r2 = digitalRead(2);
  if (r2 != lastBtn2) { tBtn2 = millis(); lastBtn2 = r2; }
  if ((millis() - tBtn2) > 50 && r2 != stableBtn2) {
    stableBtn2 = r2;
    if (stableBtn2 == LOW) { pedRequest = true; Serial.print("[EVENT] ped | t="); Serial.println(millis()); }
  }
  if (millis() - stateStart >= stateDur) return E_TIMER;
  return E_NONE;
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT); pinMode(12, OUTPUT); pinMode(11, OUTPUT);
  pinMode(3, INPUT_PULLUP); pinMode(2, INPUT_PULLUP);
  setupTable();
  goTo(S_GREEN, 10000, "init");
}

void loop() {
  Event ev = pollEvent();
  if (ev != E_NONE) {
    Handler h = fsmTable[currentState][ev];
    if (h) h();
  }
  if (currentState == S_WARNING && millis() - lastBlink >= 500) {
    lastBlink = millis();
    blinkOn = !blinkOn;
    digitalWrite(12, blinkOn ? HIGH : LOW);
  }
}
