## e-Paper img

This code allows flashing an image to a [4.2 inch B](https://www.waveshare.com/wiki/4.2inch_e-Paper_Module_\(B\)\_Manual) waveshare e-ink display.

It makes assumptions that are specific to the environment I'm running in, and is not meant to really be generally usable code.

I specifically want the following properties:

1. Write images (and only images, I don't care about fonts or drawing or whatever)
2. Control 3 e-Ink displays from one board at once by wiring them to different GPIO pins and using SPI CS selection.
3. Runs on the NixOS image I'm using for this project (TODO: link to it once it's in a public repo somewhere)

The goal is for this to spit out a CLI tool that allows displaying an image with a given SPI+GPIO configuration at runtime.

The original example code this is based on could display an image, but configuration was hard-coded, and it supported a bunch of boards. I really don't care about the other boards, so we can simplify.

### Why C

I want to do this in rust, but the venerable epd-waveshare rust library simply did not work for my board and raspberry pi setup, and I eventually gave up and decided rust shelling out to a CLI tool for this part of it was _fine_.

If I get enough free time, I might finish debugging my rust issues I ran into.

## License / provenance

This code is derived from the RaspberryPi / C code in [this repository](https://github.com/waveshareteam/e-Paper/tree/master).

It retains the git history to show what commit it came from, and is under the original MIT license.

Modifications are made by me.
