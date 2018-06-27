Import processing.serial.*;
Serial myPort;
Table dataTable;

int numReadings = 500;
int readingCounter = 0;

/*Swtich Code*/
int inPin = 8;         //switch output pin 8
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // Debouncing constant

/*PWM initializations*/
int retract = 5;       // RPWM is on Arduino PWM output pin 5
int extend = 6;        // LPWM is on Arduino PWM output pin 6
float duty = 0.8;      // Duty cycle as a decimal 80%

int time_delay = 15000; // 5 second delay

void setup() 
{
  String portName = Serial.list()[7];
  myPort = new Serial(this, port7, 9600);
  table.addColumn("id");

  table.addColumn("Pressure");
  table.addColumn("Force");

  table.addColumn("sensor1");
  table.addColumn("sensor2");
  
  /*Switch code*/
  pinMode(inPin, INPUT); //button press pin 8
  
  Serial.begin(9600);

  /*Force Transducer*/
  pinMode(A0, INPUT);
  //pinMode(A1, INPUT);

  /*Pressure Transducer*/
  pinMode(A2,INPUT);
  
  /*PWM setup*/
  pinMode(retract, OUTPUT);
  analogWrite(retract, 0);
  pinMode(extend, OUTPUT); 
  analogWrite(extend, 0);
}

void loop() 
{//start loop
  reading = digitalRead(inPin);

  if (reading == HIGH && previous == LOW && millis() - time > debounce) //interesting way of debouncing
  {//start if
    if (state == HIGH)
    {
      state = LOW;
      PWM_ADC_Cycle();
    }
    else
      state = HIGH;

    time = millis();    
  }//end if

  previous = reading;
  
}//end loop

void PWM_ADC_Cycle()
{
  analogWrite(extend, duty*255);//start PWM
  
  int startTime = millis();
  int endTime = startTime;
  while ((endTime - startTime) <= 7000)//5 second delay for now
  {
    Pressure_Sensor();
    Force_Sensor();
  }

  print.myPort();//print csv file
 
  analogWrite(extend, 0);//end PWM

  delay(2000);
  /*Retract actuator back to starting position*/
  analogWrite(retract, duty*255);
  delay(36000);
  analogWrite(retract, 0);
  Serial.print(F("\n"));
}
void Pressure_Sensor()
{
  float pVal = analogRead(A2)*5.000/1023.000;
  Serial.print(F(""));
  Serial.print(F("Pressure:\t"));
  Serial.println(pVal);
  Serial.print(F("\n"));
}

void Force_Sensor()
{
  float fVal1 = analogRead(A0)*5.000/1023.000;     // read the force sensor positive input
  //float fVal2 = analogRead(A1)*5.000/1023.000;     // read the force sensor negative input
  Serial.print(F("Force:\t"));
  Serial.println(abs(fVal1));
  Serial.println('\n');
  Serial.println('\n');
}

void serialEvent(Serial myPort)
{
  val = myPort.readStringUntil('\n');
  if (val!=null){
    val = trim(val);
    println(val);
    float sensorVals[]=floast(split(val, ','));

    TableRow newRow = dataTable.addRow();
    newRow.setInt("id", table.lastRowIndex());

    newRow.setInt("Pressure", pressure());
    newRow.setInt("Force", force());

    newRow.setFloat("sensor1",sensorVals[1]);
    newrow.setFloat("sensor2",sensorVals[1]);

    readingCounter++;
  }
  
}

