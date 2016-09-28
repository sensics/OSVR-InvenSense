# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "InvChreDriver" for configuration "Release"
set_property(TARGET InvChreDriver APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(InvChreDriver PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/InvChreDriver.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS InvChreDriver )
list(APPEND _IMPORT_CHECK_FILES_FOR_InvChreDriver "${_IMPORT_PREFIX}/external/lib/release/InvChreDriver.lib" )

# Import target "libAAR" for configuration "Release"
set_property(TARGET libAAR APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libAAR PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/libAAR.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libAAR )
list(APPEND _IMPORT_CHECK_FILES_FOR_libAAR "${_IMPORT_PREFIX}/external/lib/release/libAAR.lib" )

# Import target "libCalibrationFxp" for configuration "Release"
set_property(TARGET libCalibrationFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libCalibrationFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/libCalibrationFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libCalibrationFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libCalibrationFxp "${_IMPORT_PREFIX}/external/lib/release/libCalibrationFxp.lib" )

# Import target "libGestureFxp" for configuration "Release"
set_property(TARGET libGestureFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libGestureFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/libGestureFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libGestureFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libGestureFxp "${_IMPORT_PREFIX}/external/lib/release/libGestureFxp.lib" )

# Import target "libOrientationFxp" for configuration "Release"
set_property(TARGET libOrientationFxp APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libOrientationFxp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/libOrientationFxp.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS libOrientationFxp )
list(APPEND _IMPORT_CHECK_FILES_FOR_libOrientationFxp "${_IMPORT_PREFIX}/external/lib/release/libOrientationFxp.lib" )

# Import target "PocoFoundation_s" for configuration "Release"
set_property(TARGET PocoFoundation_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoFoundation_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoFoundation_s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoFoundation_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoFoundation_s "${_IMPORT_PREFIX}/external/lib/release/PocoFoundation_s.lib" )

# Import target "PocoJSON_s" for configuration "Release"
set_property(TARGET PocoJSON_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoJSON_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoJSON_s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoJSON_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoJSON_s "${_IMPORT_PREFIX}/external/lib/release/PocoJSON_s.lib" )

# Import target "PocoNet_s" for configuration "Release"
set_property(TARGET PocoNet_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoNet_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoNet_s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoNet_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoNet_s "${_IMPORT_PREFIX}/external/lib/release/PocoNet_s.lib" )

# Import target "PocoUtil_s" for configuration "Release"
set_property(TARGET PocoUtil_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoUtil_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoUtil_s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoUtil_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoUtil_s "${_IMPORT_PREFIX}/external/lib/release/PocoUtil_s.lib" )

# Import target "PocoXML_s" for configuration "Release"
set_property(TARGET PocoXML_s APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoXML_s PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoXML_s.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoXML_s )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoXML_s "${_IMPORT_PREFIX}/external/lib/release/PocoXML_s.lib" )