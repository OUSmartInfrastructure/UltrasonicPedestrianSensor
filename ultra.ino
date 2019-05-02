#include "application.h"
#include "src/constants.h"
#include "src/LinkedList.h"


// LED pins
pin_t led1 = D6;
pin_t led2 = A5;
pin_t led3 = D7;

// TRIG pins
pin_t t1 = D0;
pin_t t2 = D4;
pin_t t3 = D5;

// ECHO pins
pin_t e1 = D1;
pin_t e2 = D2;
pin_t e3 = D3;

// ULTRASONIC SENSOR struct
typedef struct {
    pin_t trig_pin; //echo pin
    pin_t echo_pin; //trig pin
    pin_t led_pin; //led pin
    uint32_t wait; //trigger interval based on TRIGGERINTERVAL
    uint16_t sensor; //ID of the sensor
    bool loginfo; //whether to log any info of the sensor
    unsigned long duration; //value of last pulse duration from the interrupt
    unsigned long startMicros; //time marker for when the trigger was activated. used to calculate pulse length
    long last; //last time in milliseconds the trigger was activated
    bool checking; //blocks repeat evaluations of sensor state until state evaluation is completed.
    uint32_t dirCntBuf; //smoothing buffer to even out sensor errors. Current sensor operation does not require it, depends on SAMPLECOUNT
    long dirBufTimer; //time marker for smoothing buffer to even out sensor errors. Current sensor operation does not require it, depends on MAXTIMEBTNSAMPLES.
    bool lastStatus; //last recorded status of the sensor
} ULTRASONIC_SENSOR;

// Defines sensor parameters using the STRUCT structure
ULTRASONIC_SENSOR uSensor[3] = {
    {t1, e1, led1, TRIGGERINTERVAL, 1, true, 0, 0, 0, false, 0, 0, false},
    {t2, e2, led2, TRIGGERINTERVAL, 2, true, 0, 0, 0, false, 0, 0, false},
    {t3, e3, led3, TRIGGERINTERVAL, 3, true, 0, 0, 0, false, 0, 0, false},
};

// Status stack implemented in a LinkedList
LinkedList<int> sStack = LinkedList<int>();

// Cloud update temporary variables for timers and count
unsigned long cloudUpdateTimeStamp = 0;
uint16_t cloudLastLeft = 0;
uint16_t cloudLastRight = 0;

// Crossing counters
int leftXings, rightXings;


/*
 * Function:  isBetween
 * --------------------
 * Checks if an integer is between a certain minimum and maximum
 *
 *  a: the minimum
 *  b: the number to check
 *  c: the maximum
 *
 *  returns: boolean
 *
 */
bool isBetween(double a, double b, double c) {
    if((a < b) && (b < c))
      return true;
    else
      return false;
}


/*
 * Function:  rangeISR
 * --------------------
 * Does the actual argument passing for the ISR for the called sensor
 * by computing and saving the duration of the pulse with respect to
 * when the ISR was called.
 *
 *  i: the id of the sensor
 *
 *  returns: void
 *
 */
void rangeISR(pin_t i){
    uSensor[i].duration = micros() - uSensor[i].startMicros;  // sets duration to current time - start time of respective sensor
    if(uSensor[i].loginfo){if (DBGLVL3) Serial.printlnf("[L3][uSensor[%2d].duration] - > %2d", i, uSensor[i].duration);}
}


/*
 * Function:  echoIntS1
 * --------------------
 * ISR for sensor 1 that calls the actual ISR. This is necessary:
 * due to the inability to pass arguments directly to this function
 *
 *  returns: void
 *
 */
void echoIntS1(){
    rangeISR(0);
    if(uSensor[0].loginfo){if (DBGLVL1) Serial.printlnf("[L1][rangeISR(0)] - > Sensor 1 ISR called");}
}


/*
 * Function:  echoIntS2
 * --------------------
 * ISR for sensor 2 that calls the actual ISR. This is necessary:
 * due to the inability to pass arguments directly to this function
 *
 *  returns: void
 *
 */
void echoIntS2(){
    rangeISR(1);
    if(uSensor[1].loginfo){if (DBGLVL1) Serial.printlnf("[L1][rangeISR(1)] - > Sensor 2 ISR called");}
}


/*
 * Function:  echoIntS3
 * --------------------
 * ISR for sensor 3 that calls the actual ISR. This is necessary:
 * due to the inability to pass arguments directly to this function
 *
 *  returns: void
 *
 */
void echoIntS3(){
    rangeISR(2);
    if(uSensor[2].loginfo){if (DBGLVL1) Serial.printlnf("[L1][rangeISR(2)] - > Sensor 3 ISR called");}
}



/*
 * Function:  controlLED
 * --------------------
 * turns an LED on or off based on the boolean value passed :
 *
 *  led: the numerical id of the sensor and by extension the related LED
 *  action: boolean action to take on the LED
 *
 *  returns: void
 *
 */
void controlLED(int led, bool action){
    (action) ? digitalWrite(led, HIGH) :digitalWrite(led, LOW);
}


/*
 * Function:  evalState
 * --------------------
 * Evaluates the state of the sensor based on the reported distance over at least 2 cycles :
 *
 *  sensor: the numerical id of the sensor
 *  inches: the computed distance the sensor is away from the object
 *
 *  returns: void
 *
 */
void evalState(int sensor, uint32_t inches){
    int i = sensor - 1; //change from sensor ID to index
    uSensor[i].dirCntBuf += 1; //assume false positive #hacky, needs proper fix

    if(isBetween(MININCHES, inches, MAXINCHES)){ //check if reported distance to object is within range
        if(uSensor[i].dirCntBuf < SAMPLECOUNT){ //check if smoothening criteria has been met
            if(uSensor[i].loginfo){if (DBGLVL2) Serial.printlnf("[L2] - > Distance and Buffer criteria met");}
            if((micros() - uSensor[i].dirBufTimer) < MAXTIMEBTNSAMPLES){ //check is MAX samples has been achieved in time
                uSensor[i].dirCntBuf += 1;
            }
            else{
                uSensor[i].dirCntBuf = 0;
            }
            uSensor[i].dirBufTimer = micros();
        }
        else{ //smoothening criteria has been met
            if(!uSensor[i].lastStatus){
                uSensor[i].lastStatus = true; //set last known status
                controlLED(uSensor[i].led_pin, true); //light up the LED
                if(uSensor[i].loginfo){if (DBGLVL1) Serial.printlnf("[L1][LED index %2d] - > HIGH", i);}
            }
        }
    }
		else { //no object in valid range
        uSensor[i].dirCntBuf = 0;
        uSensor[i].dirBufTimer = 0;
        uSensor[i].lastStatus = false;
        controlLED(uSensor[i].led_pin, false);
        if(uSensor[i].loginfo){if (DBGLVL1) Serial.printlnf("[L1][LED index %2d] - > LOW", i);}
		}
}


/*
 * Function:  getSystemStatus
 * --------------------
 * Computes the decimal state of the system from the binaries from their last status update :
 *
 *
 *
 *  returns: int, deci: the integer value of the state of the system
 *
 */
int getSystemStatus(){
    int deci = 0;
    String input_binary_stringz =
        String(uSensor[1].lastStatus == true ? 1 : 0) +
        String(uSensor[2].lastStatus == true ? 1 : 0) +
        String(uSensor[0].lastStatus == true ? 1 : 0); //construct instantaneous binary state

    deci = strtol(input_binary_stringz, NULL, 2); //convert binary to decimal

    return deci;
}


/*
 * Function:  updateStatus
 * --------------------
 * Adds a new status to the stack by first eliminating the possibility of repeatitions :
 *
 *  status (int): the integer status to add to the stack
 *
 *  returns: void
 *
 */
void updateStatus(int status){
    if (sStack.size() == 0){ //check if this is the first item, implying no duplicate possibility
        sStack.add(status);
    }
    else{
        if((sStack.get(sStack.size() - 1) != status) && (sStack.get(sStack.size() - 2) != status)){ //check for duplicity
            sStack.add(status); //add to stack
            if (DBGLVL1) Serial.printlnf("[L1][Add to Stack] - > %2d", status);
        }
    }
}


/*
 * Function:  countCrossings
 * --------------------
 * Counts the total left and right crossings and assigns them to the variables passed by reference
 * The count is implemented using basic algebra and a set of assumptions described at this LinkedList
 * https://github.com/OUSmartInfrastructure/UltrasonicPedestrianSensor:
 *
 *  & leftXings (int): total left crossings
 *  & rightXings (int): total right crossings
 *
 *  returns: void
 *
 */
void countCrossings(int & leftXings, int & rightXings){ //arguments passed by reference
    leftXings = 0; //reset count
    rightXings = 0; //reset count

    int sSum = 0; //temporary pattern sum reset
    int marker = sStack.size(); //marker to keep track of which part of stack to start looking at patterns from

    for (unsigned i = sStack.size(); i-- > 0 ; ){
        if(i > marker){
            if (DBGLVL3) Serial.printlnf("[L3][XINGCOUNT][Pre Marker Point L1]");
            continue; //if the marker is set to a latter point, move on
        }
        else{ //if this point is at or after the marker point, look for pattern
            if (DBGLVL3) Serial.printlnf("[L3][XINGCOUNT][Post Marker Point L1]");
            if(sStack.get(i) == 1 || sStack.get(i) == 4){ //if the STATE at this point is 1 or 4, it possibly indicates an end to a left or right crossing
                sSum = 0;
                for (unsigned j = i+1; j-- > 0 ; ){
                    if (DBGLVL3) Serial.printlnf("[L3][XINGCOUNT][Post Marker Point L2] j<>sSum -> %2d:%2d  ", j, sSum);
                    if((sSum - 7 == 0)  || ((sSum - 7) % 3 == 0)){
                        if (DBGLVL3) Serial.printlnf("[L3][XINGCOUNT][Post Marker Point L3] sStack.get(j+1) -> %2d  ", sStack.get(j+1));
                        if(sStack.get(i) == 4 && sStack.get(j+1) == 1){//left crossing start and end criteria met
                            leftXings += 1;
                            marker = j+1;
                            break;
                        }
                        else if(sStack.get(i) == 1 && sStack.get(j+1) == 4){//right crossing start and end criteria met
                            rightXings += 1;
                            marker = j+1;
                            break;
                        }
                        else{
                            sSum += sStack.get(j); //continue summing
                        }
                    }
                    else if(sSum < 17){
                        sSum += sStack.get(j);//continue summing
                    }
                    else{
                        break; //quit loop, invalid state/sum encountered
                    }
                }
            }
        }
    }
}


/*
 * Function:  syncWithCloud
 * --------------------
 * Updates the ATT M2X cloud service at regular intervals with the number of crossings
 * Interval is based on CLOUDUPDATEINTERVAL :
 *
 *
 *  returns: void
 *
 */
void syncWithCloud(){
    if((unsigned long) micros() - cloudUpdateTimeStamp > CLOUDUPDATEINTERVAL){

        uint16_t lefts = leftXings;
        uint16_t rights = rightXings;

        if (DBGLVL1) Serial.printlnf("[L1][CLOUD] Cloud sync in progress ...");
        if (DBGLVL1) Serial.printlnf("[L1][CLOUD] L<>R PREV -> %2d:%2d", cloudLastLeft,cloudLastRight);
        if (DBGLVL1) Serial.printlnf("[L1][CLOUD] L<>R CURR -> %2d:%2d", lefts, rights);

        String cloudTime = Time.format(Time.now(), TIME_FORMAT_ISO8601_FULL);


        String dataToPublish1 = String::format("{ \"ts1\": \"%s\", \"an1\": %d }", cloudTime.c_str(), lefts - cloudLastLeft);
        if (DBGLVL1) Serial.print("[L1][CLOUD] dataToPublish1 -> ");
        if (DBGLVL1) Serial.println(dataToPublish1);
        Particle.publish(WEBHOOKID1, dataToPublish1, PRIVATE);

        String dataToPublish2 = String::format("{ \"ts1\": \"%s\", \"an1\": %d }", cloudTime.c_str(), rights - cloudLastRight);
        if (DBGLVL1) Serial.print("[L1][CLOUD] dataToPublish2 -> ");
        if (DBGLVL1) Serial.println(dataToPublish2);
        Particle.publish(WEBHOOKID2, dataToPublish2, PRIVATE);

        cloudUpdateTimeStamp = micros(); //save timestamp as reference marker
        cloudLastLeft = lefts; // save last left count
        cloudLastRight = rights; // save last right count

    }
}


/*
 * Function:  setup
 * --------------------
 * The setup() function is called when a sketch starts.
 * Use it to initialize variables, pin modes, start using libraries, etc.
 * The setup() function will only run once, after each powerup or reset of the Photon board.:
 *
 *
 *  returns: void
 *
 */
void setup() {
    Serial.begin(SERIALTXRATE);

    if (DBGLVL1 == TRUE) Serial.printlnf("[L1][SETUP] Serial port configured for %d bps.", SERIALTXRATE);
    if (DBGLVL1 == TRUE) Serial.printlnf("[L1][SETUP] Starting setup...");
    if (DBGLVL1 == TRUE) Serial.printlnf("[L1][SETUP] Setting time zone to: %d ...", int(TZONE));
    Time.zone(TZONE);

    if (DBGLVL1 == TRUE) Serial.printlnf("[L1][SETUP] Turning on Daylight Savings Time");
    Time.beginDST();

    if (DBGLVL1 == TRUE) Serial.printlnf("[L1][SETUP] Current time and date are: %s", Time.timeStr().c_str());

    // Set pin mode
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);

    // Perform startup animation
    controlLED(led1, true);
    controlLED(led2, true);
    controlLED(led3, true);
    delay(2000);
    controlLED(led1, false);
    controlLED(led2, false);
    controlLED(led3, false);

    // Set trig pins
    pinMode(t1, OUTPUT);
    pinMode(t2, OUTPUT);
    pinMode(t3, OUTPUT);

    // clear any intrinsic or legacy writes
    digitalWriteFast(t1, LOW);
    digitalWriteFast(t2, LOW);
    digitalWriteFast(t3, LOW);

    // Set echo pins
    pinMode(e1, INPUT);
    pinMode(e2, INPUT);
    pinMode(e3, INPUT);

    // set and activate interrupt on falling edge
    attachInterrupt(e1, echoIntS1, FALLING, 0);
    attachInterrupt(e2, echoIntS2, FALLING, 0);
    attachInterrupt(e3, echoIntS3, FALLING, 0);

    // enable interrupts
    interrupts();


    //Initialize crossing stack with phantom state #hacky
    sStack.add(0);
}


/*
 * Function:  setup
 * --------------------
 * loop() After creating a setup() function, which initializes and sets the initial values,
 * the loop() function does precisely what its name suggests, and loops consecutively,
 * allowing your program to change and respond.
 * Use it to actively control the Photon board.:
 *
 *
 *  returns: void
 *
 */
void loop() {
    for(unsigned int i = 0; i < (sizeof uSensor / sizeof uSensor[0]); i++){
        if ( uSensor[i].checking == false && millis() - uSensor[i].last > uSensor[i].wait ){  // once per 1/10 second if not doing anything
            uSensor[i].checking = true; // set a flag so it doesn't get confused
            uSensor[i].duration = 0; //reset duration
            uSensor[i].last = millis();  // get time now for timer

            digitalWriteFast(uSensor[i].trig_pin, HIGH); // activate trigger
            delayMicroseconds(TRIGGERPAUSE);
            digitalWriteFast(uSensor[i].trig_pin, LOW); // de-activate trigger

            uSensor[i].startMicros = micros();  // get time now for start
        }

        if ( uSensor[i].duration ) { // if it has been set by ISR
            uint32_t inches = uSensor[i].duration / 74 / 2; //converting to inches, depends on sensor

            evalState(uSensor[i].sensor, inches); //evaluate the state of the sensor from the calculated dsitance in inches


            uSensor[i].checking = false;            // reset the flag to signal a new start
            uSensor[i].duration = 0;                   // reset this as well
            //delayMicroseconds(uSensor[i].wait);                      // even circuits need a break
        }
    }

    //get system state
    updateStatus(getSystemStatus());

    //obtain crossing counts
    countCrossings(leftXings, rightXings);

    //cloud sync
    syncWithCloud();


    if (DBGLVL2) Serial.printlnf("[L2][STATE] T<>L<>R -> %2d:%2d:%2d", sStack.size(), leftXings, rightXings);
}
