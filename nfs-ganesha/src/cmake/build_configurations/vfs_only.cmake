# Only build VFS fsal and other useful options

set(USE_FSAL_PROXY_V4  OFF)
set(USE_FSAL_CEPH OFF)
set(USE_FSAL_GPFS OFF)
set(_MSPAC_SUPPORT OFF)
set(USE_9P OFF)
set(USE_DBUS ON)

message(STATUS "Building vfs_only configuration")
