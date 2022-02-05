#!/bin/bash
PLUGIN_NAME='EarthEcho'
PREFIX_PATH='/usr/local'
ICON_PATH='../resources/icons'
ICON_NAME='icon'
ICON_SMALL_SIZE='128'
ICON_BIG_SIZE='256'
ICON_EXTENSION='png'
ICON_EXTENSION_SCALABLE='svg'
PLUGIN_NAME_LOWERCASE=$(echo ${PLUGIN_NAME} | tr '[:upper:]' '[:lower:]')
if [[ $@ == *'uninstall'* ]]; then
  if [[ $@ == *'standalone'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling Standalone..."
    rm ${PREFIX_PATH}/bin/${PLUGIN_NAME_LOWERCASE}
  fi
  if [[ $@ == *'icons'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling Icons..."
    rm ${PREFIX_PATH}/share/icons/hicolor/${ICON_SMALL_SIZE}x${ICON_SMALL_SIZE}/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION}
    rm ${PREFIX_PATH}/share/icons/hicolor/${ICON_BIG_SIZE}x${ICON_BIG_SIZE}/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION}
    rm ${PREFIX_PATH}/share/icons/hicolor/scalable/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION_SCALABLE}
    rm ${PREFIX_PATH}/share/applications/${PLUGIN_NAME_LOWERCASE}.desktop
  fi
  if [[ $@ == *'vst3'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling VST3 Plugin..."
    rm -r ${PREFIX_PATH}/lib/vst3/${PLUGIN_NAME_LOWERCASE}.vst3
  fi
  if [[ $@ == *'lv2'* ]]; then
    echo "${PLUGIN_NAME}: Uninstalling LV2 Plugin..."
    rm -r ${PREFIX_PATH}/lib/lv2/${PLUGIN_NAME_LOWERCASE}.lv2
  fi
else
  if [[ $@ == *'standalone'* ]]; then
    echo "${PLUGIN_NAME}: Installing Standalone..."
    cp build/${PLUGIN_NAME}_artefacts/Standalone/${PLUGIN_NAME_LOWERCASE} ${PREFIX_PATH}/bin/
  fi
  if [[ $@ == *'icons'* ]]; then
    echo "${PLUGIN_NAME}: Installing Icons..."
    cp ${ICON_PATH}/${ICON_NAME}${ICON_SMALL_SIZE}.${ICON_EXTENSION} ${PREFIX_PATH}/share/icons/hicolor/${ICON_SMALL_SIZE}x${ICON_SMALL_SIZE}/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION}
    cp ${ICON_PATH}/${ICON_NAME}${ICON_BIG_SIZE}.${ICON_EXTENSION} ${PREFIX_PATH}/share/icons/hicolor/${ICON_BIG_SIZE}x${ICON_BIG_SIZE}/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION}
    cp ${ICON_PATH}/${ICON_NAME}.${ICON_EXTENSION_SCALABLE} ${PREFIX_PATH}/share/icons/hicolor/scalable/apps/${PLUGIN_NAME_LOWERCASE}.${ICON_EXTENSION_SCALABLE}
    cp ${PLUGIN_NAME_LOWERCASE}.desktop ${PREFIX_PATH}/share/applications
  fi
  if [[ $@ == *'vst3'* ]]; then
    echo "${PLUGIN_NAME}: Installing VST3 Plugin..."
    if [ ! -d "${PREFIX_PATH}/lib/vst3" ]; then
      mkdir ${PREFIX_PATH}/lib/vst3
    fi
    cp -r build/${PLUGIN_NAME}_artefacts/VST3/${PLUGIN_NAME_LOWERCASE}.vst3 ${PREFIX_PATH}/lib/vst3/
  fi
  if [[ $@ == *'lv2'* ]]; then
    echo "${PLUGIN_NAME}: Installing LV2 Plugin..."
    if [ ! -d "${PREFIX_PATH}/lib/lv2" ]; then
      mkdir ${PREFIX_PATH}/lib/lv2
    fi
    cp -r build/${PLUGIN_NAME}_artefacts/LV2/${PLUGIN_NAME_LOWERCASE}.lv2 ${PREFIX_PATH}/lib/lv2/
  fi
fi
