#!/bin/bash
PLUGIN_NAME='EarthEcho'
PREFIX_PATH='/usr/local'
if [[ $@ == *'uninstall'* ]]; then
  if [[ $@ == *'standalone'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling Standalone..."
    rm ${PREFIX_PATH}/bin/${PLUGIN_NAME}
  fi
  if [[ $@ == *'vst3'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling VST3 Plugin..."
    rm -r ${PREFIX_PATH}/lib/vst3/${PLUGIN_NAME}.vst3
  fi
  if [[ $@ == *'lv2'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling LV2 Plugin..."
    rm -r ${PREFIX_PATH}/lib/lv2/${PLUGIN_NAME}.lv2
  fi
else
  if [[ $@ == *'standalone'* ]]; then
    echo "${PLUGIN_NAME}: Installing Standalone..."
    cp build/${PLUGIN_NAME}_artefacts/Standalone/${PLUGIN_NAME} ${PREFIX_PATH}/bin/
  fi
  if [[ $@ == *'vst3'* ]]; then
    echo "${PLUGIN_NAME}: Installing VST3 Plugin..."
    if [ ! -d "${PREFIX_PATH}/lib/vst3" ]; then
      mkdir ${PREFIX_PATH}/lib/vst3
    fi
    cp -r build/${PLUGIN_NAME}_artefacts/VST3/${PLUGIN_NAME}.vst3 ${PREFIX_PATH}/lib/vst3/
  fi
  if [[ $@ == *'lv2'* ]]; then
    echo "${PLUGIN_NAME}: Installing LV2 Plugin..."
    if [ ! -d "${PREFIX_PATH}/lib/lv2" ]; then
      mkdir ${PREFIX_PATH}/lib/lv2
    fi
    cp -r build/${PLUGIN_NAME}_artefacts/LV2/${PLUGIN_NAME}.lv2 ${PREFIX_PATH}/lib/lv2/
  fi
fi
