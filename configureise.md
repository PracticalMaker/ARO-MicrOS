---
layout: page
title: Command configureise 
tagline: 
group: 
---
{% include JB/setup %}

## Command configureise

The configureise command is used to configure ion specific electrodes (ISE). You know these as pH, ORP, DO etc. probes. This command basically takes measurements of 2 different solutions and stores them in EEPROM. The readise command assembles the data and then calculates the value. Note that this currently does not support temperature compensation.

### Usage

First, make sure you open the ARO_MicrOS_Serial example sketch.

configureise/Configuration_Number/PIN/Value1/Value2/Type


**Configuration_Number**: ARO_MicrOS stores configurations by number starting from 0. By default there is room for 2 configurations, but this can be changed. Please see ARO_MicrOS Configurations

**PIN**: Which analog pin you want to configure

**Value1**: Value of the first solution **Input as a decimal ie. 7.00 NOT 7**

**Value2**: Value of the second solution **Input as a decimal ie. 10.00 NOT 10**

**Type**: Type of measurement (pH, ORP etc.)

Once you've typed the command in (example in the picture below) it's simply a matter of following the on screen instructions.

{:.fullwideimage}
![](https://s3.amazonaws.com/practicalmaker/Images/ARO_MicrOS/configureISE.png)
