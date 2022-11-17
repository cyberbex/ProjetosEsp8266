#define RL 9.5  //The value of resistor RL is 47K

#define m -0.263 //Enter calculated Slope 

#define b 0.42 //Enter calculated intercept

#define Ro 14.5 //Enter found Ro value

#define MQ_sensor 32 //Sensor is connected to A4



void setup() {

  Serial.begin(115200);
  pinMode(MQ_sensor,INPUT);

}


void loop() {


  float VRL; //Voltage drop across the MQ sensor

  float Rs; //Sensor resistance at gas concentration 

  float ratio; //Define variable for ratio

   

  VRL = analogRead(MQ_sensor)*(3.3/4096.0); //Measure the voltage drop and convert to 0-3.1V

  Rs = ((4.7*RL)/VRL)-RL; //Use formula to get Rs value

  ratio = Rs/Ro;  // find ratio Rs/Ro

 

  float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm


  Serial.print("NH3 (ppm) = "); //Display a ammonia in ppm

  Serial.println(ppm);


  Serial.print("Voltage = "); //Display a intro message 

  Serial.print(VRL);


   delay(1000);

}
