---
layout: page
title: Command pinmode 
tagline: 
group: 
---
{% include JB/setup %}

## Command pinmode 

This command allows you to set the pinmode to either input or output. Note that pins 0 - 25 states (INPUT or OUTPUT) are automatically saved to EEPROM (in an Arduino) so you only need to set the mode once and on restart it will automatically reset the modes for you.

### Usage

First, make sure you open the ARO_MicrOS_Serial example sketch.

pinmode/PIN/MODE

**PIN**: Which digital pin you want to configure

**MODE**: 0 for INPUT, 1 for OUTPUT
