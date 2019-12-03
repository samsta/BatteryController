find_path(LibGpiod_INCLUDE_DIR NAMES gpiod.h)
mark_as_advanced(LibGpiod_INCLUDE_DIR)

find_library(LibGpiod_LIBRARY NAMES gpiod)
mark_as_advanced(LibGpiod_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibGpiod
    REQUIRED_VARS LibGpiod_INCLUDE_DIR LibGpiod_LIBRARY)
