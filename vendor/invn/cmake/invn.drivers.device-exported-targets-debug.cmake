#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EmbUtils" for configuration "Debug"
set_property(TARGET EmbUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EmbUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/EmbUtils.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS EmbUtils )
list(APPEND _IMPORT_CHECK_FILES_FOR_EmbUtils "${_IMPORT_PREFIX}/debug/lib/EmbUtils.lib" )

# Import target "DynamicProtocol" for configuration "Debug"
set_property(TARGET DynamicProtocol APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(DynamicProtocol PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/DynamicProtocol.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS DynamicProtocol )
list(APPEND _IMPORT_CHECK_FILES_FOR_DynamicProtocol "${_IMPORT_PREFIX}/debug/lib/DynamicProtocol.lib" )

# Import target "IDDHostUtils" for configuration "Debug"
set_property(TARGET IDDHostUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(IDDHostUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/IDDHostUtils.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS IDDHostUtils )
list(APPEND _IMPORT_CHECK_FILES_FOR_IDDHostUtils "${_IMPORT_PREFIX}/debug/lib/IDDHostUtils.lib" )

# Import target "IDDHostAdapter" for configuration "Debug"
set_property(TARGET IDDHostAdapter APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(IDDHostAdapter PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/IDDHostAdapter.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS IDDHostAdapter )
list(APPEND _IMPORT_CHECK_FILES_FOR_IDDHostAdapter "${_IMPORT_PREFIX}/debug/lib/IDDHostAdapter.lib" )

# Import target "IDD" for configuration "Debug"
set_property(TARGET IDD APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(IDD PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/IDD.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS IDD )
list(APPEND _IMPORT_CHECK_FILES_FOR_IDD "${_IMPORT_PREFIX}/debug/lib/IDD.lib" )

# Import target "IDDDeviceEmdWrapIcm20xxx" for configuration "Debug"
set_property(TARGET IDDDeviceEmdWrapIcm20xxx APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(IDDDeviceEmdWrapIcm20xxx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/IDDDeviceEmdWrapIcm20xxx.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS IDDDeviceEmdWrapIcm20xxx )
list(APPEND _IMPORT_CHECK_FILES_FOR_IDDDeviceEmdWrapIcm20xxx "${_IMPORT_PREFIX}/debug/lib/IDDDeviceEmdWrapIcm20xxx.lib" )

# Import target "IDDClient" for configuration "Debug"
set_property(TARGET IDDClient APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(IDDClient PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/IDDClient.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS IDDClient )
list(APPEND _IMPORT_CHECK_FILES_FOR_IDDClient "${_IMPORT_PREFIX}/debug/lib/IDDClient.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
