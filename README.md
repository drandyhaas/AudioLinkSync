
# AudioLinkSync
Code and design files etc for a small device that delays digital audio.

Audio comes in on Toslink Spdif digital fiber, gets converted to electrical by DLR1150, then processed (delayed)
by a Teensy 4.0, then converted back to optical by a DLT1150, and back out on fiber.

The Teensy runs Arduino code with an audio library that can delay the music by up to 750 ms, in 1 ms steps.

A button and knob are used to adjust the delay. Hold the button down a few seconds until the LED comes on - you can then adjust the delay using the knob. Then hold the button down a few seconds again until the LED goes off - the delay is then locked.

An ssd1360 screen shows the delay and stats in real time.

PCB:

![Screenshot_20230627-143739](https://github.com/drandyhaas/AudioLinkSync/assets/13686048/cc174014-3fa5-45bc-a1f0-77dc1720f8c1)


![teensy_toslink_v1 2](https://github.com/drandyhaas/AudioLinkSync/assets/13686048/8b2f0c58-a93f-4113-b62e-3a8bb50ae1ae)


Product:

![PXL_20230706_005849022~2](https://github.com/drandyhaas/AudioLinkSync/assets/13686048/256f95ee-8c1b-4877-980d-8ba2e631c779)

