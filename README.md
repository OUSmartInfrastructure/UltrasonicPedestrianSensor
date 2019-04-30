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
The LEDs were added to provide a visual cue to an observer about when the respective ultrasonic sensor has a lock on an object.

![Photon and sensor setup](https://lh3.googleusercontent.com/aOGESy7z6LJA1MKZu-8_F3fkbDrixD-gwzMaNBt7uHLJF7u5kGIUPZhc11ZhNm5iKyU5AHEJxhWmQ0Yare6ZJ6_lYLUpm8x0EEkuIkBTB6KbuJaiTfiCkJ5mYJV0eIuBLlg-1RyUdhsqEQj0LCHLe93M5WbnXG83nW60em87j75s8URPEjmsXH7lJrFfbncIkBRvqP3VYPitc2sKJkzjBCM1Ph3w2GGX0-iO0QTnZTfV2BH8OjBiIyXQJmRwvmrGlxudz7B7O_ZuOtm-95AnhL6dRNFFNNzTciIqNq0LrG-vwZHnIYlYTuxO_HrYWC3qCke3LV8YYm1dn5VXJqzvBH_Wzu_WNi9_BX2viZC844OXv8fOQTAzvDOm_xPwb9q-K129_Wajwx-f37K5fus7__sft8AoGQnjfv49CbP6yCElRDAsUDQn1DsEYgTbCMHdCVKWfQFXxj1TWvAiws-FL7otCGhfbUq29MT34pd65Kxs9ZBmWJYD4JxCN9s3u2j4wHPhnA67eRpbPk6WjDtPoO5Qe18EAiE31xllOCdmw_B_JSopsZRHzUCBnM5T3MFLjKUtW-UrvM02JG-q6ouxx00ZdzsONfLvQzGJMCPo8N6hTiUOa7stIscY9C4VtusaEwv1u9L44C5AANGK_3OhZLGRIBa33-ig=w1246-h829-no)
