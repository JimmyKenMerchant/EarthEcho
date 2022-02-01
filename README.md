# EarthEcho

![Graphical User Interface of EarthEcho](/resources/images/earthecho_gui.png "EarthEcho_GUI")

## What's EarthEcho?

* EarthEcho is an audio plugin made with [JUCE framework](https://github.com/juce-framework/JUCE).

* EarthEcho makes an echo effect on each channel (e.g., left, right, etc.) with various sliders.

* EarthEcho has a retro computing style graphic user interface.

## Usage

* There are 6 sliders.
  1. 'Wet Gain': Changes the volume of the wet (effected sound).
  2. 'Dry/Wet': Changes the mix rate of the dry (non-effected sound) and the wet.
  3. 'Delay Time': Changes the delay time up to 1 second.
  4. 'Repeat Rate': Changes the repeating time of the echo (feedbacking sound).
  5. 'LP Filter': Changes frequency of the low-pass filter effected on the echo.
  6. 'HP Filter': Changes frequency of the high-pass filter effected on the echo.

## Installation

* I tested this plugin on Linux so far. I'm using [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md) to build EarthEcho.

* I made a plugin with standalone, VST3 format, and LV2 format. Note that [a JUCE fork of the LV2 porting project will be automatically downloaded to make LV2 plugins on my Bash script](https://github.com/lv2-porting-project/JUCE/tree/lv2). If you don't select to make a LV2 plugin, the official JUCE will be downloaded.

```bash
git clone -b main https://github.com/JimmyKenMerchant/EarthEcho.git
cd EarthEcho/builder_linux
# For more detail, read "builder_linux/README.md".
./builder_linux.sh vst3 lv2
```

* I haven't made it with AU format. I leave the XML file for Projucer, and you can customize settings to make an AU plugin on Projucer.

## License

* Code: [GNU GENERAL PUBLIC LICENSE Version 3](https://www.gnu.org/licenses/gpl-3.0.en.html)

* Icons and Images: Copyright © 2022 Kenta Ishii. All Rights Reserved.
