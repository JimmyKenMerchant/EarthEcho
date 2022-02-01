#!/bin/bash
# LLVM Clang is preferred for portability of projects, e.g., on ARM processors.
# Read https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md
# Additional Dependencies: lv2-dev clang
PLUGINS=''
REPO=''
CONFIG='Release'
# Search All Arguments
if [[ $@ == *'newjuce'* ]]; then
  if [ -e "modules/JUCE" ]; then
    echo 'Deleting Existing JUCE Framework'
    rm -r modules/JUCE
  fi
fi
if [[ $@ == *'vst3'* ]]; then
  echo 'Building VST3 Plugin'
  PLUGINS+='-DPLUGIN_VST3=ON'
fi
if [[ $@ == *'lv2'* ]]; then
  echo 'Building LV2 Plugin'
  PLUGINS+=' -DPLUGIN_LV2=ON'
  # Fork of LV2 Porting Project
  REPO+='--branch lv2 https://github.com/lv2-porting-project/JUCE'
else
  # Official Repository
  REPO+='https://github.com/juce-framework/JUCE'
fi
if [[ $@ == *'debug'* ]]; then
  echo 'Building for Debugging'
  CONFIG='Debug'
fi
echo 'Executing git clone '${REPO}
if [ ! -d "modules" ]; then
  mkdir modules
fi
git clone ${REPO} modules/JUCE
cmake -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ${PLUGINS} .
cmake --build build --config ${CONFIG}
