cmake_minimum_required(VERSION 2.6.4)
project(GPS_DATALOGGER C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${gps_reader_MISSION_DIR}/fsw/platform_inc)
include_directories(${gps_kalman_MISSION_DIR}/fsw/platform_inc)
include_directories(${gps_kalman_MISSION_DIR}/fsw/src)
include_directories(${libnmea_MISSION_DIR}/include)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(gps_datalogger ${APP_SRC_FILES})
