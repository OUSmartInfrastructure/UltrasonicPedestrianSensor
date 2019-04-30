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


![Photon and single sensor setup](https://lh3.googleusercontent.com/Xe4HW000d6frL4SwBG6PeyBP30EypJroX1ozOxZB6zl8bjAcGcsZv0R-cBKnqeoTU3qr8NUdTPAgyovB5dloO-lwOUSiirQ0f7-kT-cumumsk962d3hlrsLM1_3Y89zqpyN-vqOTM29hoD39WUmFjZvz4s899M63Vdtvv5Z9PsKh6pj_zxusA8S2KVXcsJeF81z-AsxplFnFMH50VAJlQQrPcphIYqOs57tcJqSHLADX_KqoQGiVk-kbc4CThfwlqPd5x96ENSpdzhFTfjb85zFf0TA92CZtM5hjUehDYAYhh-KG5nnDqYMlXxp4E1xKhm0zhnJbchBYozZsFjmXBnEnubLjIEnpOhAb-l0AMS77Ew1lGRxdgt3BpUvv6DLKAsKVZ0QkXQPlzeoeIhBJ4nKFwwaYB4vdFmvuhF44nasD6NuF2g58R0UOVFMrWl2TQ5sxyuEifxNVgRa1qaOMOfwbvqj-ECGH46PHXIk5urcHQECS1q1ZwOaVJTceBMTSgyN5FLr9NMoh5IEh0WQJzaJGzZ3RJRmsSH7UZ9cuw9oM2f9baSiecIYFSqiKcM64fXQwOKk0PQ-2zHdhM2QE3Yo59HjgEYJP8P7bebDjru8HrgPPWxqvAVRBNFa8_FVOrwh09c_x_26DJvXVwrvBCnbUsH93eQoe=w1290-h604-no)

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

![Photon and sensor setup](https://lh3.googleusercontent.com/mf_g8sRSLoaWhfDBYqEbg-dRGGpE-W3MlzlIgxlmZ5Hic7qnGjRUIKQMepZ2aLzIh772NVG6v_-Cz_pWPTNctCLFOO2ACTMGz5izfRg6wi3EqWesQdnkuuWYv3atFqh897MJTtOM7ZH1i1L-pcFSNHgmmLq6PssFRowYe43VES-IwUs-KfdiVZgbYdJaiVxdOJGZq86wss6LqUcKKo5SUGrqdrMFl66KlT-_ncWdDe1bQKY9fvnrVDqw5QIcY6dmCXde2hH5hGb3uoxbQ-vE8baIgLpo_E8LSb8i1LN3gMcovijrSBa2_iaTmbeiSW0B-A54dgdiweQMq3ae8Xe3OtROjxZwz4QJVyr0tukjco8J8Tx-Et0AH4M-SHznI8ceUv_8ESbhQxxyjku4xzbqhS1AOdwStFUJZXYZaVZJWDXY_jMHo2MmCw6gW4Svbkmal6ELuR8eG94nEEbRboqS4cPVvdb000g1Axe2fpo2Bj9Odunr_RFlYn2xBsTe7aNgsr8yFly6_WIlLZzgU70EtsSwfkSABilOiEh9nPJ6bKXeiTXvrU3kKpFdE6DZHp1BOTgzG_4Pd9qGiFKKBAFzwL0k5Q4cQJEaIM3UnWYP1IB4EijqD0WEPGyxRxaF_SNtWzVJJdjfRjKpcLVfrUpiDVVmztNA733e=w1241-h825-no)

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
Establishing state changes is one step towards identifying the possibility of a pedestrian crossing but to establish the direction, these states need to be stored and analyzed. In C programming, serial data is usually managed using arrays or vectors but arrays require a predetermined size and vectors are a lot harder to manipulate so the storage mechanism of choice was the LinkedList data structure. Fortunately a good one has been implemented by Ivan Seidel Gomes and can be obtained from [GitHub](https://github.com/ivanseidel/LinkedList/)

