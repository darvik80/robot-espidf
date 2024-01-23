Robot based on ESP32 devices
* [x] target: Linux for debugging
* [x] BLE/HID Joystick
* [ ] Servo motor
* [ ] L293 motor

## esp-idf

### update defaults
```shell
idf.py save-defconfig
```
### set-target=linux
```shell
idf.py --preview set-target linux
```
### problems with xcode-15: -warn_commons
$ENV{IDF_PATH}/CMakeLists.txt
```cmake
...
if(CONFIG_IDF_TARGET_LINUX AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND link_options "-Wl,-dead_strip")
    #list(APPEND link_options "-Wl,-warn_commons")
    else()
    list(APPEND link_options "-Wl,--gc-sections")
    list(APPEND link_options "-Wl,--warn-common")
endif()
...
```