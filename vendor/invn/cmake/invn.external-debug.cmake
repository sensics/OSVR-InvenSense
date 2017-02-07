# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

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