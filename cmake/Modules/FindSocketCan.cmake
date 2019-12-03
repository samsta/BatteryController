find_path(SocketCan_INCLUDE_DIR NAMES linux/can.h)
mark_as_advanced(SocketCan_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SocketCan
    REQUIRED_VARS SocketCan_INCLUDE_DIR)