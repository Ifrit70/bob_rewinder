# bob_rewinder
My personal adaptation of Deku's Bob Rewinder for Arduino pro mini

I made my own rewinder with an arduino pro mini laying around (it was for a one-time use, and I already had all parts at hand).
I started with @Briadark code and ... things escalated a little 😅 
Long time without coding, so it was a fun exercise, I didn't see the hours passing.
Here is my participation if others want to duplicate:

Hardware: 
- 1x 3.3V arduino pro mini
- 1x 2x4 edge slot connector
- 2x Dipswitch
- Power supply from the cheap usb/serial converter, but it could work without a pc, only require a 3.3V regulator/battery or a 5V supply (with double check of the connexions to use the arduino's regulators)

The arduino internal led give a quick status of the cassette value:
Off = Not full
On = Full (from previous write or just brand new if in read-only mode)
Blinking = Writing failed
