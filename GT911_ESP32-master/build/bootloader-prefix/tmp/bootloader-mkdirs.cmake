# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/ihor/esp/esp-idf/components/bootloader/subproject"
  "/Users/ihor/GT911_ESP32-master/build/bootloader"
  "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix"
  "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/tmp"
  "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/src"
  "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/ihor/GT911_ESP32-master/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
