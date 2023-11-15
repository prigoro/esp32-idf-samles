# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/ihor/esp/esp-idf/components/bootloader/subproject"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/tmp"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/src/bootloader-stamp"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/src"
  "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Volumes/Project/Workspace/esp32-idf-samles/ESP32-RGB-GT911-LVGL_sample/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
