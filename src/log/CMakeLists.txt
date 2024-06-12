aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILES)

add_library(LOG_LIB STATIC)

target_sources(LOG_LIB PUBLIC ${FILES})

target_include_directories(LOG_LIB PUBLIC
${CMAKE_SOURCE_DIR}/include/ 
${CMAKE_SOURCE_DIR}/include/log
${CMAKE_SOURCE_DIR}/include/util)