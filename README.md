# EarthEcho

![Graphical User Interface of EarthEcho](/resources/images/earthecho_gui.png "EarthEcho_GUI")

## What's EarthEcho?

* EarthEcho is an audio plugin made with [JUCE framework](https://github.com/juce-framework/JUCE).

* EarthEcho makes an echo effect on each channel (left = L or right = R) with various sliders to emulate nature.

* EarthEcho has a retro computing style graphic user interface.

## Usage

* There are 6 sliders for each channel (L or R) to change parameter values.
  * 'Wet Gain': Changes the volume of the wet (effected sound).
  * 'Dry/Wet': Changes the mix rate of the dry (non-effected sound) and the wet.
  * 'Delay Time': Changes the delay time up to 1 second.
  * 'Repeat Rate': Changes the repeating time of the echo (feedbacking sound).
  * 'LP Filter': Changes frequency of the low-pass filter effected on the echo.
  * 'HP Filter': Changes frequency of the high-pass filter effected on the echo.

* Parameter values can be changed through MIDI messages (Standalone Plugin).
  * Received control change / continuous controller (CC) number is masked to enable only least 5 bits, i.e., CC number modulo 32 will be used to determine the number.
  * Parameters are assigned to CC# 0-5 for L channel, and CC# 6-11 for R channel. The order is the same as the description above.
  * Resolution is 0-100. The value 101-127 will be saturated to 100.
  * There is no channel selection so far because of considering multiple MIDI through ports in your system. To make multiple MIDI through ports in Linux, use 'snd-seq-dummy (ALSA sequencer MIDI-through client)'.
  * When you use a plugin host, but not this standalone plugin, the plugin host would have a function to map MIDI channels and CCs to change parameter values. In this case, you need to change a parameter value between 0 to 1.0 with a received CC value, 0-127. In this plugin, I set interval to snap the decimal for each parameters, and it helps the precise control.

* There are 3 buttons.
  * 'ABOUT': Shows the product information of this plugin.
  * 'L/R': Alternates displaying sliders for each channel.
  * 'BGCOLOR': Changes the background color of the GUI.

* In the convention of the digital audio world, the first audio input/output called as "input/output 1" is for the left channel of a stereo system, and the second one called as "input/output 2" is for the right channel. This plugin follows this convention, and you'll watch numbered inputs and outputs for this plugin in your audio plugin host.

* I'm also offering this plugin as an application, so called "standalone plugin".
  * In POSIX, the configuration like input and output connections is stored automatically in '~/.config/EarthEcho.settings' which also retrieves parameter values on the previous session.
  * ALSA/JACK. JACK is preferred. Select JACK in "Audio device type" in the "Audio/MIDI Settings" menu popped up from the "Options" menu. QjackCtl is useful to manage connections. I also use [aj-snapshot](https://aj-snapshot.sourceforge.io/) to save and load connections.
  * Parameter values can be saved using "Save current state..." in the "Options" menu.
  * In the command line, EarthEcho can load a parameter file for multiple processes, e.g., `earthecho param1.earthecho`.

## Installation

* I tested this plugin on Linux so far. I'm using [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md) to build EarthEcho.

* I made a plugin with standalone, VST3 format, and LV2 format. Note that [a JUCE fork of the LV2 porting project will be automatically downloaded to make LV2 plugins on my Bash script](https://github.com/lv2-porting-project/JUCE/tree/lv2). If you don't select to make a LV2 plugin, the official JUCE will be downloaded.

```bash
cd ~/Desktop
git clone -b main https://github.com/JimmyKenMerchant/EarthEcho.git
cd EarthEcho/builder_linux
# For more detail, read "builder_linux/README.md".
./builder_linux.sh vst3 lv2
sudo ./installer_linux.sh vst3 lv2 standalone icons
```

* I haven't made it with AU format. I leave the XML file for Projucer, and you can customize settings to make an AU plugin on Projucer.

## Technical Notes

* 02/15/2022: ~6.1.5-lv2 seems to make no sound on LV2 plugin format. Checkout to 6.1.4-lv2.~ [lv2 has already become OK.](https://github.com/lv2-porting-project/JUCE/pull/21)

* 03/13/2022 (6.1.5): The standalone plugin accepts MIDI messages even if you don't set `pluginCharacteristicsValue="pluginWantsMidiIn"` in 'EarthEcho.jucer' or `NEEDS_MIDI_INPUT TRUE` in 'builder_linux/CMakeLists.txt'. The wrapper for standalone plugins in 'JUCE/modules/juce_audio_plugin_client/Standalone' doesn't use a definition, `JucePlugin_WantsMidiInput`, to check the necessity of MIDI messages. However, the standalone plugin needs to select one of "Active MIDI inputs" on "Audio/MIDI Settings".

## License

* Code: [GNU GENERAL PUBLIC LICENSE Version 3](https://www.gnu.org/licenses/gpl-3.0.en.html)

* Icons and Images: Copyright © 2022 Kenta Ishii. All Rights Reserved.
