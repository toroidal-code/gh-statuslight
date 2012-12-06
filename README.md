#Attention!
This is now broken and deprecated due to status.github.com no longer using JSON. I will attempt to rewrite the program that it will work with the updated system.For now there is a dummy file being loaded, so the program still works. But it does not reflect the real status of Github
###Happy Hubbing!

Github Traffic Light
==============

An Arduino program to control a traffic light that mirrors the status of status.github.com

###What is this?

This is a little program I wrote that utilises an Arduino (ATMega328p) and Ethernet hardware (a WizNet WIZ820io) to control three lights that mirror the color of the banner on status.github.com. So now you know, without even opening a new webpage, whether the reason you can't push/pull to/from GitHub is your fault or not.

###Here be Dragons

Be warned, this is extremely janky code, and does some very wrong things inorder to run on a microcontroller. For example:
We assume that the status.github.com/status.json file will always exist
AND
We assume that the first element in the json object is the status key
AND
We assume that because of this, we can disregard everything past the first comma, thus saving us a lot of RAM

###Licensing

This is released under the MIT license.