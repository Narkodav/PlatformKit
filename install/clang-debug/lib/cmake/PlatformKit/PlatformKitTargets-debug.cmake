#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PlatformKit::PlatformKit" for configuration "Debug"
set_property(TARGET PlatformKit::PlatformKit APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(PlatformKit::PlatformKit PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libPlatformKit.a"
  )

list(APPEND _cmake_import_check_targets PlatformKit::PlatformKit )
list(APPEND _cmake_import_check_files_for_PlatformKit::PlatformKit "${_IMPORT_PREFIX}/lib/libPlatformKit.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
