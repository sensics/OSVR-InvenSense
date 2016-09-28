# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "InvChreDriver" for configuration "Debug"
set_property(TARGET InvChreDriver APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(InvChreDriver PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/InvChreDriver.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS InvChreDriver )
list(APPEND _IMPORT_CHECK_FILES_FOR_InvChreDriver "${_IMPORT_PREFIX}/external/lib/debug/InvChreDriver.lib" )

# Import target "libAAR" for configuration "Debug"
set_property(TARGET libAAR APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libAAR PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/libAAR.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libAAR )
list(APPEND _IMPORT_CHECK_FILES_FOR_libAAR "${_IMPORT_PREFIX}/external/lib/debug/libAAR.lib" )

# Import target "libCalibrationFxp" for configuration "Debug"
set_property(TARGET libCalibrationFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libCalibrationFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/libCalibrationFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libCalibrationFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libCalibrationFxp "${_IMPORT_PREFIX}/external/lib/debug/libCalibrationFxp.lib" )

# Import target "libGestureFxp" for configuration "Debug"
set_property(TARGET libGestureFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libGestureFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/libGestureFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libGestureFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libGestureFxp "${_IMPORT_PREFIX}/external/lib/debug/libGestureFxp.lib" )

# Import target "libOrientationFxp" for configuration "Debug"
set_property(TARGET libOrientationFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libOrientationFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/libOrientationFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libOrientationFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libOrientationFxp "${_IMPORT_PREFIX}/external/lib/debug/libOrientationFxp.lib" )

# Import target "PocoFoundation_s" for configuration "Debug"
set_property(TARGET PocoFoundation_s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PocoFoundation_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/PocoFoundation_sd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoFoundation_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoFoundation_s "${_IMPORT_PREFIX}/external/lib/debug/PocoFoundation_sd.lib" )

# Import target "PocoJSON_s" for configuration "Debug"
set_property(TARGET PocoJSON_s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PocoJSON_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/PocoJSON_sd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoJSON_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoJSON_s "${_IMPORT_PREFIX}/external/lib/debug/PocoJSON_sd.lib" )

# Import target "PocoNet_s" for configuration "Debug"
set_property(TARGET PocoNet_s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PocoNet_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/PocoNet_sd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoNet_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoNet_s "${_IMPORT_PREFIX}/external/lib/debug/PocoNet_sd.lib" )

# Import target "PocoUtil_s" for configuration "Debug"
set_property(TARGET PocoUtil_s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PocoUtil_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/PocoUtil_sd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoUtil_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoUtil_s "${_IMPORT_PREFIX}/external/lib/debug/PocoUtil_sd.lib" )

# Import target "PocoXML_s" for configuration "Debug"
set_property(TARGET PocoXML_s APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PocoXML_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/external/lib/debug/PocoXML_sd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoXML_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoXML_s "${_IMPORT_PREFIX}/external/lib/debug/PocoXML_sd.lib" )