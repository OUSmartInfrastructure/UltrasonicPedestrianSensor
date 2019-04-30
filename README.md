# Using Particle's Photon and three low-cost Ultrasonic Sensors (HC-SR04) to implement a finite-state machine that detects pedestrians crossing a relatively busy sidewalk 

Particle's [Photon](https://www.particle.io/products/hardware/photon-wifi/) is small and powerful wifi-connected micontroller capable of basic to advaced telemetry when interfaced with sensor units. To demonstrate the use of Photon as a pedestrian sensor, the microcontroller was interfaced with three low-cost ultrasonic sensor units, _HC-SRO4_. 

The scope of this project comprised setting up the microcontroller to report telemetry data in the form of the count and direction of crossings as determined by the state-machine to the [AT&T M2X](https://m2x.att.com/) service. AT&T M2X provides time-series data storage, device management, message brokering, event triggering, alarming, geo-fencing, and data visualization for industrial Internet of Things (IOT) products and services.


# Getting Started
To get things started, it was required to:
 - Obtain a [Photon](https://www.particle.io/products/hardware/photon-wifi/)
 - A breadboard and a couple of jumpers
 - Three [Ultrasonic](https://www.robotshop.com/en/hc-sr04-ultra01-ultrasonic-range-finder.html) sensor units
 - An AT&T M2X account
 
 
## Setting up the Photon with the sensor unit
Once the photon has been [setup](https://docs.particle.io/guide/getting-started/start/photon/) and the development environment is ready for use, you can go on to connect the ultrasonic sensors as described in the breadboard diagram below. Each sensor unit requires a 5V power supply, a connection to ground and digital I/O pins for the echo and trigger functionality.

The photos is a 3V3 device but the Ultrasonic sensors are 5V devices so for the purposes of this project, the Vcc was obtained from the Vin pin of the photon. The Vin pin extends the 5V from the external supply (USB in this case) for use by devices that require it.


![Photon and single sensor setup](https://i.imgur.com/GgHCLG9.png)

## A word about ultrasonic sensors
Our ultrasonic sensors, like many others, use a single transducer to send a pulse and to receive the echo.  The sensor determines the distance to a target by measuring time lapses between the sending and receiving of the ultrasonic pulse. [Read more here](https://www.maxbotix.com/articles/how-ultrasonic-sensors-work.htm) and [here](https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/)

## Operating a single ultrasonic sensor with the Photon
In order to generate the ultrasound you need to set the Trig on a High State for 10 µs. That will send out an 8 cycle sonic burst which will travel at the speed of sound and it will be received in the Echo pin. The Echo pin will output the time in microseconds the sound wave traveled.


     // defines pins numbers
     const int trigPin = D1;
     const int echoPin = D0;
     
     // defines variables
     long duration;
     int distance;
     
     void setup() {
         pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
         pinMode(echoPin, INPUT); // Sets the echoPin as an Input
         Serial.begin(9600); // Starts the serial communication
     }
     void loop() {
         // Clears the trigPin
         digitalWrite(trigPin, LOW);
         delayMicroseconds(2);
         
         // Sets the trigPin on HIGH state for 10 micro seconds
         digitalWrite(trigPin, HIGH);
         delayMicroseconds(10);
         digitalWrite(trigPin, LOW);
         
         // Reads the echoPin, returns the sound wave travel time in microseconds
         duration = pulseIn(echoPin, HIGH);
         
         // Calculating the distance
         distance= duration*0.034/2; //Depending on the sensor model, this may be calculated differently
         
         // Prints the distance on the Serial Monitor
         Serial.print("Distance: ");
         Serial.println(distance);
     }


## Operating three ultrasonic sensors concurrently
Adapting the code above to operate three of the ultrasonic sensors concurrently exposes a major issue with respect to how unoptimized the code above is. Several delay functions, including the pulseIn function block any further processing by the processor until they have completed their respective routines which causes lots of valuable time losses.

The sensor requires two of these delays for their proper function. Anything less than this results in a total malfunction which leads to unpredictive and erroneous results. They are:
1. The time interval between successive triggers (100 milliseconds). It's recommended to wait at least this interval before another trigger. 
2. The trigger length (10 miroseconds) which refers to the interval between when the trigger pin is set high and then low.

The LEDs were added to provide a visual cue to an observer about when the respective ultrasonic sensor has a lock on an object.

![Photon and sensor setup](https://i.imgur.com/dABqimX.png)

To deal with this issue of blocking, the succesful approach made use of interrupts. Interrupts is a mechanism by which an I/O or an instruction can suspend the normal execution of processor and gets itself serviced like it has higher priority. For example a processor doing a normal execution can be interrupted by some sensor to execute a particular process that is present in ISR (Interrupt Service Routine). After executing the ISR, the processor can again resume the normal execution of the program. [Read more](https://circuitdigest.com/microcontroller-projects/arduino-interrupt-tutorial-with-examples)

Interrupts can be of type hardware and software, but for the purposes of this project, a special kind of hardware interrupt which falls under external interrupts called the pin-change interrupt was used. Like Arduinos, Photons can have more interrupt pins enabled by using pin change interrupts. They can also be triggered using RISING or FALLING edges. On each FALLING edge, an Interrupt Service Routine (ISR) is called to perform a specific function in reaction to the interrupt.

Interrupts provided a mechanism to avoid the dreaded pulseIn function that blocked any further execution of the program causing the system to greatly underperform. 

## Implementing the state machine
When in operation, the state of each sensor is either LOW (0) or HIGH (1). In essence, the entire system at anytime can be in one of the following eight finite states shown in their binary and decimal notations.


     000 = 0
     001 = 1
     010 = 2
     011 = 3
     100 = 4
     101 = 5
     110 = 6
     111 = 7


In the code, a sensor with a LOW instantaneous state means no object has been detected within a distance of about 5 inches - 120 inches of that sensor. A HIGH state is the reverse of this statement. The system scans the status of these sensors constantly to establish one of the finite states listed above.

Also, because of the constant delays required for the proper function of the ultrasonic sensor, the maximum number of times a sensor can change it's state in a second is approximately 10.

## Storing state transitions
Establishing state changes is one step towards identifying the possibility of a pedestrian crossing but to establish the direction, these states need to be stored and analyzed. In C programming, serial data is usually managed using arrays or vectors but arrays require a predetermined size and vectors are a lot harder to manipulate so the storage mechanism of choice was the LinkedList data structure. Fortunately a good one has been implemented by Ivan Seidel Gomes and can be obtained from [GitHub](https://github.com/ivanseidel/LinkedList/).

The LinkedList was used to implement the state change stack which is then run through an algorithm to determine a pattern of transitions that amount to a crossing.


## Identifying a crossing in the stack
In a typical scenario, lets assume we are observers observing the unit at a sidewalk and a pedestrian is walking across from right to left. This crossing from right to left is referred to as a Left Crossing and it will be the basis of the explanation for how a crossing is identified from the stack.

![left crossing](https://i.imgur.com/VdE2kMS.png)

As the pedestrian crosses the unit, ideally we expect a number of transitions. Each transition will pe pushed down the stack as observed below:

    Stack level         Sensor State
    -----------         ------------ 
    0                   000 -> 0
    1                   001 -> 1
    2                   011 -> 3
    3                   010 -> 2
    4                   110 -> 6
    5                   100 -> 4
    6                   000 -> 0

It always starts with a state 1 and ends with a state 4. The stack will contain 100s and perhaps 1000s of such states so the real hurdle here has to do with identifying the number of this pattern of transitions within the stack.

The algorithm employed to achieve this employs a peek and look-ahead strategy, one for each direction. Starting from the top of the stack to the bottom, the algorithm iterates over the stack by "peeking" at the state at the top of the stack, and then the next and so on. 

Anytime it peeks, it checks if the state is 4, implying there is a chance that 4 terminates a left crossing pattern. Other than that, it continues till it reaches the bottom.

To summarize the transitions again, when a pedestrian crosses from the right to the left (left crossing) we expect the transitions to be as follows:

    000 -> 001 -> 011 -> 010 -> 110 -> 100 -> 000 

which is the same as:

    0 -> 1 -> 3 -> 2 -> 6 -> 4 -> 0

But the system is not perfect, and although some kind of machine learning algorithm could iron out the errors, the possibilities are few and can be catered to using simple algebra. The following transitions could also be valid left crossings assuming the system has imperfections.

    000(0)              000(0)               000(0)               000(0)
    001(1)              001(1)               001(1)               001(1)
    010(2)              011(3)               010(2)               011(3)             
    100(4)              010(2)               110(6)               010(2)
    000(0)              100(4)               100(4)               110(6)  
                        000(0)               000(0)               100(4)
                                                                  000(0)
    ------              ------               ------               ------
    7                   10                   13                   16
    
    
All the states are represented here except for 111 (7) which is a state which is not expected to ever happen due to the positioning of the sensors. According to the documentation, each sensor has a beam angle of about 30° of which 15° is typically effective. The beams are usually of the same frequency so it's important to separate the sensors to prevent any cross readings. [Read more](http://dyor.roboticafacil.es/en/sensor-distancia/)






