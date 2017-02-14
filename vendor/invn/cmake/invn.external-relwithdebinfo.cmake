# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PocoFoundationmd" for configuration "RelWithDebInfo"
set_property(TARGET PocoFoundationmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PocoFoundationmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/external/lib/release/PocoFoundationmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoFoundationmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoFoundationmd "${_IMPORT_PREFIX}/external/lib/release/PocoFoundationmd.lib" )

# Import target "PocoJSONmd" for configuration "RelWithDebInfo"
set_property(TARGET PocoJSONmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PocoJSONmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/external/lib/release/PocoJSONmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoJSONmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoJSONmd "${_IMPORT_PREFIX}/external/lib/release/PocoJSONmd.lib" )

# Import target "PocoNetmd" for configuration "RelWithDebInfo"
set_property(TARGET PocoNetmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PocoNetmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/external/lib/release/PocoNetmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoNetmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoNetmd "${_IMPORT_PREFIX}/external/lib/release/PocoNetmd.lib" )

# Import target "PocoUtilmd" for configuration "RelWithDebInfo"
set_property(TARGET PocoUtilmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PocoUtilmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/external/lib/release/PocoUtilmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoUtilmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoUtilmd "${_IMPORT_PREFIX}/external/lib/release/PocoUtilmd.lib" )

# Import target "PocoXMLmd" for configuration "RelWithDebInfo"
set_property(TARGET PocoXMLmd APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(PocoXMLmd PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/external/lib/release/PocoXMLmd.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS PocoXMLmd )
list(APPEND _IMPORT_CHECK_FILES_FOR_PocoXMLmd "${_IMPORT_PREFIX}/external/lib/release/PocoXMLmd.lib" )