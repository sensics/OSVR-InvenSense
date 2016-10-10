#
# InvenSense plugin for OSVR
#
cmake_minimum_required(VERSION 3.0.0)
project(com_sensics_InvenSense)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(osvr REQUIRED)
find_package(invn REQUIRED)
find_package(jsoncpp REQUIRED)

# Generate our JSON header file
osvr_convert_json(com_sensics_InvenSense_json
    com_sensics_InvenSense.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_sensics_InvenSense_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/invn/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/invn/sources")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/invn/external/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/vendor/invn/external/sources")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/quat")

add_subdirectory(quat)

# Build and install the plugin
osvr_add_plugin(com_sensics_InvenSense
    com_sensics_InvenSense.cpp
    InvenSenseController.h
    InvenSenseController.cpp
    DeviceInstance.h
    DeviceInstance.cpp
    DefaultOutputStream.h
    DefaultOutputStream.cpp
    SensorEventsTimestampStats.h
    SensorEventsPrinter.h
    SensorEventsPrinter.cpp
    CommandSensor.h
    CommandSensor.cpp
    Command.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_sensics_InvenSense_json.h")

target_link_libraries(com_sensics_InvenSense
    osvr::osvrPluginKitCpp
    osvr::osvrUtil
    IDDClient
    IDD
    IDDHostAdapter
    VSensorFwk
    EmbUtils
    DynamicProtocol
    VSensorImplCModel
    VSensorImplEISGyr
    MLMath
    IDDDeviceIcm20602
    IDDIcm20602
    IDDDeviceIcm20690
    IDDDeviceIcm20648
    IDDIcm20690
    IDDDeviceSmartMotion
    IDDDeviceOther
    IDDDeviceIcm30xxx
    IDDIcm30xxx
    IDDFifoProtocol
    IDDDeviceEmdWrapIcm20xxx
    IDDDeviceGsh
    IDDHostUtils
    IDDIcm20648
    AlgoInvn
    PocoNet_s
    PocoUtil_s
    PocoXML_s
    PocoFoundation_s
    PocoJSON_s
    ws2_32
    iphlpapi
    jsoncpp_lib
    quat)

# Install docs, license, sample config
install(FILES
    osvr_server_config.InvenSense.sample.json
    DESTINATION .)