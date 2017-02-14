# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PocoFoundationmd" for configuration "Release"
set_property(TARGET PocoFoundationmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoFoundationmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoFoundationmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoFoundationmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoFoundationmd "${_IMPORT_PREFIX}/external/lib/release/PocoFoundationmd.lib" )

# Import target "PocoJSONmd" for configuration "Release"
set_property(TARGET PocoJSONmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoJSONmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoJSONmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoJSONmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoJSONmd "${_IMPORT_PREFIX}/external/lib/release/PocoJSONmd.lib" )

# Import target "PocoNetmd" for configuration "Release"
set_property(TARGET PocoNetmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoNetmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoNetmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoNetmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoNetmd "${_IMPORT_PREFIX}/external/lib/release/PocoNetmd.lib" )

# Import target "PocoUtilmd" for configuration "Release"
set_property(TARGET PocoUtilmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PocoUtilmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/external/lib/release/PocoUtilmd.lib"
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