# Using Particle's Photon and three low-cost Ultrasonic Sensors (HC-SR04) to implement a finite-state machine that detects pedestrians crossing a relatively busy sidewalk 

Particle's [Photon](https://www.particle.io/products/hardware/photon-wifi/) is small and powerful wifi-connected micontroller capable of basic to advaced telemetry when interfaced with sensor units. To demonstrate the use of Photon as a pedestrian sensor, the microcontroller was interfaced with three low-cost Ultrasonic sensor units, _HC-SRO4_. 

The scope of this project comprised setting up the microcontroller to report telemetry data in the form of the count and direction of crossings as determined by the state-machine to the [AT&T M2X](https://m2x.att.com/) service. AT&T M2X provides time-series data storage, device management, message brokering, event triggering, alarming, geo-fencing, and data visualization for industrial Internet of Things (IOT) products and services.
