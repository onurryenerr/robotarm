                                                                                                                                                                                                                                                                                              #include <Servo.h>

Servo alfa;
Servo beta;
Servo gamma;

void setup() {
  alfa.attach(2);
  beta.attach(4);
  gamma.attach(6);

  alfa.write(1455);
  beta.write(1455);
  gamma.write(50);

  Serial.begin(9600);  
  }

void loop() {
  if (Serial.available()) {
    int angle = Serial.parseInt();
    Serial.println(angle);
    beta.writeMicroseconds(angle);
    }    
 }
