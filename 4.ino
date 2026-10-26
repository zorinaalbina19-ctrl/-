const byte R[4] = {2,3,4,5}, C[4] = {8,9,10,11};
char K[4][4] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};
char last = 0; 
unsigned long t = 0;


void setup() {
  Serial.begin(9600);

  for (byte i=0;i<4;i++){ 
    pinMode(R[i],INPUT); 
    digitalWrite(R[i],LOW); 
  }

  for (byte j=0;j<4;j++) { 
    pinMode(C[j],INPUT_PULLUP);
  }
  pinMode(7,OUTPUT);
}

char scan() {

  for (byte i=0;i<4;i++){
    pinMode(R[i],OUTPUT); 
    digitalWrite(R[i],LOW); 
    delayMicroseconds(50);

    for (byte j=0;j<4;j++) if(!digitalRead(C[j])) { 
      pinMode(R[i],INPUT); 
      return K[i][j]; 
    }

    pinMode(R[i],INPUT);
  }
  return 0;
}


void loop() {
  char k = scan();
  if (k && k != last) { 
    last = k; 
    t = millis(); 
    Serial.println(k); }
  if (!k && last && millis()-t > 80) {
    last = 0;
  }
}