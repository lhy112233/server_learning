aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILES)

add_library(NET_LIB STATIC)

target_sources(NET_LIB PUBLIC ${FILES})

target_include_directories(NET_LIB PUBLIC 
${CMAKE_SOURCE_DIR}/include/net
${CMAKE_SOURCE_DIR}/include/util)