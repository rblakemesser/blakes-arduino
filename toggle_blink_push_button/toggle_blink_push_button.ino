/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int myswitch = 2; 
int lit;
int reading;
int last;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(myswitch, INPUT);
  pinMode(led, OUTPUT);   
  lit = LOW;  
}

// the loop routine runs over and over again forever:
void loop() {
  reading = digitalRead(myswitch);
  if (reading != last) {
    if (reading == HIGH) {
      Serial.println("CHANGE PLACES");
      lit = !lit;
      delay(50);
      digitalWrite(led, lit);    // turn the LED off by making the voltage LOW
    }
    last = reading;
  }
}
