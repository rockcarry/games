aux_source_directory(. DIR_LIB_SRCS)

include_directories(../inc)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(game ${DIR_LIB_SRCS})

message(STATUS "static library complete!" ${PROJECT_BINARY_DIR}/add)