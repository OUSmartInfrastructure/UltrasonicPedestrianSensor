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
Once the photon has been [setup](https://docs.particle.io/guide/getting-started/start/photon/) and the development environment is ready for use, you can go on integrate the ultrasonic sensors. Each sensor unit requires a 5V power supply, a connection to ground and digital pins for the echo and trigger functionality.

![Photon and sensor setup](https://lh3.googleusercontent.com/TCk_xNIiJ_JGC0zZXulXLKgJJQP_RB0OIq0QRRNtOWTtSb2X17bn9YIvIbSZZzYKyhySkzzh4R_vSAX-vHgD_lu7LtXR9AldzJ9YpsqClJ1qEmZdYEN_FyHTIZuhF7v_cZ5XaEHc5BOthVijAeXEF2UCp1wJGjAhlryThdGaRHkvtQcIrf-L93gO8GMqlFufHrs7C3m_yUGoFn-RRpFlaY64naZ8TvlqA44u0oizG-zjK1H6MuQUi1SJR09_cOVJQrJII8AhKwxxOmzWB0wKWu5noStk-x74D6HtcC6hNWab-GA_M-Lt-JInyLckOKhzAat1eXp66uvM7n0PFpbDamBOV0mBORC09Yo5UjWKINDukFqWWGOT55WeOKMefXQBrFob7sfvm21n9hHyWAOFtVMEOtio16uihyejzZwp9hLXnZKhV8guQ-iy3fa-xIJvxruMjxh1XF4aHw4mi_8rX3JRv9naspHjshj1Cod6qzVQLB4QRm4VaW8lj4T_DyRY5zwZ-q2-1XBBjGQFZFkb5PtmexebbXOTvx_RmVj14ez4WFBLiMNW64USn_WTMMtVUBzlTDZC84rqnaNjArRVckBErT1A_6jZUaKa5xvs=w705-h436-no)
