---
layout: page
title: Command setpin 
tagline: 
group: 
---
{% include JB/setup %}

## Command setpin 

This command allows you to change pin status using digitalWrite or analogWrite. It's a 'smart' command in that if it's not 0/on or 1/off it uses analogWrite for PWM output. If you find the pin isn't responding how you think it should make sure you've run the command pinmode first and set it to an output.

### Usage

First, make sure you open the ARO_MicrOS_Serial example sketch.

setpin/PIN/Status

**PIN**: Which digital pin you want to configure

**Status**: What status to change the pin to
  - 0 = Off
  - 1 = On
  - 2 - 255 = PWM output
