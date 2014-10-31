---
layout: page
title: Commands 
tagline: 
group: navigation
---
{% include JB/setup %}

##  Commands
1. [configureise](/configureise.html)
  - This command is used for configuring Ion Specific Electrodes (ISE). You would use this command to configure pH, Nitrate, ORP etc. probes
2. [readise](/readise.html)
  - This command uses the configuration data from configureise and returns a value. Use this to read pH, Nitrate, ORP etc. probes
3. [identify](/identify.html)
  - This command will return a unique ID for the device. This isn't really used on the device currently, but is useful for external communication routing
4. [pinmode](/pinmode.html)
  - This command allows you to set pins as inputs and outputs. Pins 0 - 25 state are automatically saved to EEPROM so on a restart it's saved.
5. [setpin](/setpin.html)
  - This command allows you to change the status of pins: either ON, OFF or a PWM output.
6. resetconfig
  - Running this command will erase ALL configurations you have saved. Use with caution because this is tantamount to formatting your hard drive.
