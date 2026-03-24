# Measured Extraction

An interactive installation that reveals the hidden material weight of smartphones by translating invisible extraction processes into physical movement and balance.

## Overview

**Measured Extraction** is a media art installation that invites participants to reflect on the unseen infrastructures behind everyday technology. By placing a smartphone onto the tray, users trigger a physical response that represents the accumulated extraction of minerals embedded within the device.

The project translates mining industry, labour that were involoved, and material sourcing into a kinetic experience.

## Concept

Modern smartphones contain a complex mix of rare earth elements and mined materials. These resources are often extracted through environmentally and socially intensive processes that remain invisible to users.

This installation asks:

> *How much extraction do you carry in your pocket?*

Through physical balance and motion, the work makes these hidden weights perceptible.

## How It Works

- A user places their smartphone onto the installation.
- Sensors detect the presence and magnetic values of the device.
- A motorised system responds by shifting a weighted structure.
- The movement represents the “measured extraction” embedded in the phone.
- The system returns to its initial calibrated state after interaction.

## System Components

### Hardware
- Microcontroller (e.g., Arduino Mega)
- Geared stepper motor (with driver)
- Hall effect sensors
- Endstop switch (for trigger)
- Custom mechanical structure (scale / arm)
- Power supply

### Software
- Arduino (C/C++)
- Motor control via [AccelStepper](https://github.com/waspinator/AccelStepper) library
- Sensor data processing
- State-based interaction logic

## Setup & Installation

### 1. Hardware Assembly
- Assemble the mechanical structure
- Mount the stepper motor and the driver to the structure and conenct them
- 3D print the tray and the bucket wheel and post-process as needed
- Position Hall sensors and endstop switch at key detection points within the tray

### 2. Wiring
- Connect sensors to analog pins
- Connect endstop to digital pin
- Connect motor driver to microcontroller

### 3. Upload Code
- Open Arduino IDE
- Install required library [AccelStepper](https://github.com/waspinator/AccelStepper)
- Upload the main `.scale_controller.ino` file into the board

### 4. Calibration
- On startup, the system:
  1. Goes into Calibration mode
  2. Adjust the initial motor position by clicking left and right button
  3. Save the initial point by pressing confirm button
- This ensures consistent behaviour for each interaction

## Usage

1. Power on the system  
2. Wait for calibration to complete  
3. Place a smartphone on the designated area  
4. Observe the movement response  
5. Remove the device to reset the system  

## Known Issues

- Mechanical stability depends on accurate weight distribution  
- Heavy structures may require higher torque motors  
- Sensor placement must be precise for consistent readings  

## Future Improvements

- Add more hall effect sensors for effective magnetic value reading
- Improved structural design for durability and aesthetics  

