#  matio -------------------------------------------------------------------
find_library(MATIO_LIB matio)
find_library(MATIO_DLL libmatio.dll)

find_path(MATIO_INCLUDE_DIRS NAMES matio.h)
find_path(MATIO_INCLUDE_CONFIG_DIRS NAMES matio_pubconf.h)

set(MATIO_INCLUDE_DIRS_ALL ${MATIO_INCLUDE_DIRS})
list(APPEND MATIO_INCLUDE_DIRS_ALL ${MATIO_INCLUDE_CONFIG_DIRS})

add_library(matio SHARED IMPORTED GLOBAL)

set_property(TARGET matio APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  "${MATIO_INCLUDE_DIRS_ALL}"
  )

set_property(TARGET matio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(matio PROPERTIES
  IMPORTED_IMPLIB_DEBUG ${MATIO_LIB}
  IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG ""
  IMPORTED_LOCATION_DEBUG ${MATIO_DLL}
  )
set_property(TARGET matio APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(matio PROPERTIES
  IMPORTED_IMPLIB_RELEASE ${MATIO_LIB}
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE ""
  IMPORTED_LOCATION_RELEASE ${MATIO_DLL}
  )
