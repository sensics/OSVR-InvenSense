#OSVR-InvenSense

## Build instructions

You will need the following tools:

1. CMake (3.0 or above)
2. OSVR-Core (32 bit) - Download complete SDK [here](http://access.osvr.com/binary/osvr-sdk-installer) or [OSVR-Core](http://access.osvr.com/binary/osvr-core) separately 

Set `CMAKE_INSTALL_PREFIX` to OSVR-Core directory such as `C:\osvr_builds\OSVR-Core-Snapshot-v0.6-1339-32bit` and when you compile the plugin, also build `INSTALL` job and it will automatically copy the plugin over to OSVR directory.

Copy `osvr_server_config.InvenSense.sample.json` config file to `OSVR-Core/bin/` directory and start OSVR Server by dragging config file on top of `osvr_server.exe` or from cmd line as `osvr_server.exe osvr_server_config.InvenSense.sample.json`

You can verify that the tracking works by running OSVR Tracker Viewer app - [link](http://access.osvr.com/binary/osvr-tracker-view) (included with SDK)

The provided CMake modules in `/vendor/invn/cmake` assume the following directory structure (similar to sensor-cli app directory structure):

Set `invn_DIR` in CMake to the folder above and it will auto-configure all necessary libraries 

`-vendor/external` 

&nbsp;&nbsp;&nbsp;&nbsp; `/include` (headers)

&nbsp;&nbsp;&nbsp;&nbsp; `/sources`
 
&nbsp;&nbsp;&nbsp;&nbsp; `/lib)` (Libraries)

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; `/debug`

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; `/release`

`-vendor/include/Invn/` (headers) 

`-vendor/sources/Invn/` (headers)

`-vendor/debug`

&nbsp;&nbsp;&nbsp;&nbsp; `/bin)` (Debug Libraries)

&nbsp;&nbsp;&nbsp;&nbsp; `/lib)` (Debug Libraries)

`-vendor/release` 

&nbsp;&nbsp;&nbsp;&nbsp; `/bin)` (Release Libraries)

&nbsp;&nbsp;&nbsp;&nbsp; `/lib)` (Release Libraries)


