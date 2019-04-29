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

![Photon and sensor setup](https://lh3.googleusercontent.com/aOGESy7z6LJA1MKZu-8_F3fkbDrixD-gwzMaNBt7uHLJF7u5kGIUPZhc11ZhNm5iKyU5AHEJxhWmQ0Yare6ZJ6_lYLUpm8x0EEkuIkBTB6KbuJaiTfiCkJ5mYJV0eIuBLlg-1RyUdhsqEQj0LCHLe93M5WbnXG83nW60em87j75s8URPEjmsXH7lJrFfbncIkBRvqP3VYPitc2sKJkzjBCM1Ph3w2GGX0-iO0QTnZTfV2BH8OjBiIyXQJmRwvmrGlxudz7B7O_ZuOtm-95AnhL6dRNFFNNzTciIqNq0LrG-vwZHnIYlYTuxO_HrYWC3qCke3LV8YYm1dn5VXJqzvBH_Wzu_WNi9_BX2viZC844OXv8fOQTAzvDOm_xPwb9q-K129_Wajwx-f37K5fus7__sft8AoGQnjfv49CbP6yCElRDAsUDQn1DsEYgTbCMHdCVKWfQFXxj1TWvAiws-FL7otCGhfbUq29MT34pd65Kxs9ZBmWJYD4JxCN9s3u2j4wHPhnA67eRpbPk6WjDtPoO5Qe18EAiE31xllOCdmw_B_JSopsZRHzUCBnM5T3MFLjKUtW-UrvM02JG-q6ouxx00ZdzsONfLvQzGJMCPo8N6hTiUOa7stIscY9C4VtusaEwv1u9L44C5AANGK_3OhZLGRIBa33-ig=w1246-h829-no)
