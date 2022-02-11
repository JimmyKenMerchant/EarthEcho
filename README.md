# EarthEcho

![Graphical User Interface of EarthEcho in Development](/resources/images/earthecho_gui.png "EarthEcho_GUI")

## What's EarthEcho?

* EarthEcho is an audio plugin made with [JUCE framework](https://github.com/juce-framework/JUCE).

* EarthEcho makes an echo effect on each channel (left = L or right = R) with various sliders to emulate nature.

* EarthEcho has a retro computing style graphic user interface.

## Usage

* There are 6 sliders for each channel (L or R).
  * 'Wet Gain': Changes the volume of the wet (effected sound).
  * 'Dry/Wet': Changes the mix rate of the dry (non-effected sound) and the wet.
  * 'Delay Time': Changes the delay time up to 1 second.
  * 'Repeat Rate': Changes the repeating time of the echo (feedbacking sound).
  * 'LP Filter': Changes frequency of the low-pass filter effected on the echo.
  * 'HP Filter': Changes frequency of the high-pass filter effected on the echo.

* There are 2 buttons.
  * 'BGCOLOR': Changes the background color of the GUI.
  * 'L/R': Changes displaying parameters for each channel.

## Installation

* I tested this plugin on Linux so far. I'm using [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md) to build EarthEcho.

* I made a plugin with standalone, VST3 format, and LV2 format. Note that [a JUCE fork of the LV2 porting project will be automatically downloaded to make LV2 plugins on my Bash script](https://github.com/lv2-porting-project/JUCE/tree/lv2). If you don't select to make a LV2 plugin, the official JUCE will be downloaded.

```bash
cd ~/Desktop
git clone -b main https://github.com/JimmyKenMerchant/EarthEcho.git
cd EarthEcho/builder_linux
# For more detail, read "builder_linux/README.md".
./builder_linux.sh vst3 lv2
```

* I haven't made it with AU format. I leave the XML file for Projucer, and you can customize settings to make an AU plugin on Projucer.

## License

* Code: [GNU GENERAL PUBLIC LICENSE Version 3](https://www.gnu.org/licenses/gpl-3.0.en.html)

* Icons and Images: Copyright © 2022 Kenta Ishii. All Rights Reserved.
