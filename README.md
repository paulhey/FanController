# FanController

## Hardware

[MSP430 LaunchPad](https://www.ti.com/tool/MSP-EXP430G2ET)

## Notes

This is firmware for an MSP430G2553 microcontroller that generates a PWM signal based on the current temperature. Control is performed via the UART over USB on the LaunchPad board. Temperature is measured using the internal ADC temperature sensor. The entire process is interrupt driven.

## Changelog

> FanController - Current Version [0.4]
>
> __[2014 - MAY - 15] [0.4]__
>
> - System has a state machine. There are several modes (Auto, Debug-Auto,
> Manual, Setup) The Automatic mode is based on the following:
>
>   - The PWM duty cycle is proportional to the temperature above a
>  minimum threshold.
>   - At and below the minimum threshold, the duty cycle is ~10% (21
>  on, 179 off)
>   - The max duty cycle is at around 100 deg C.
>  The Manual mode allows the user to select one of 10 fixed PWM levels (1-10,
>  selected as {1234567890}) The Debug-Auto mode displays the TemperatureString
>  (displays current system information) continuously. Otherwise, pressing 't'
>  will do the same manually.
>
> - The Setup mode will allow the user to specifiy the minimum threshold
> temperature in degrees C. Press 's' to get into setup mode and press 'l' to
> enter the degrees (00-99) and press enter to confirm.
>
> - TODO: [ ] Port to MSP430F5529!
>
> __[2014 - Date?] [0.3.2]__
>
> - TODO: [X] Lower PWM frequency to 25KHz. Small fan is too noisy, even
> at 50KHz.
> - TODO: [ ] Read Temp. Data every ms? (Setup Timer1_A0 for this, Toggle
> LED2)
> - TODO: [ ] Print Temp. Data out every 1s over UART
>
> __[2014 - APR - 13] [0.3.1]__
>
> - [?] Added ADC10 initialization code, as well as added
> "universal" sample code (sample.c/.h)
> This is untested code.
>
> - TODO: [?] Do 8 or 4 point rolling average (is setup for 8 point)
>
> __[2014 - APR - 12] [0.3]__
>
> - [u] Respond to single character commands over UART
> - Responds to 'u' with "Hello World"
>
> __[2014 - APR - 10] [0.2, 0.1]__
>
> - [W] PWM's LED1 and MOSFET 50% Duty Cycle @100KHz
> - [W] Echoes back on UART @ 128000 BAUD.
