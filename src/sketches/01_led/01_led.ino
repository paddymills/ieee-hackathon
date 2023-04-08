int ledPinRed = 2;
int ledPinYellow = 4;
int timer = 1;
int direction = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinYellow, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinYellow, HIGH);
  delay(timer);
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinYellow, LOW);
  delay(timer);

  timer = timer + direction;
  if ( timer == 50 || timer == 1 ) {
    direction = direction * -1;
  }
}
