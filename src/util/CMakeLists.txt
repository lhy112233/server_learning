aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} FILES)

add_library(UTIL_LIB INTERFACE)

target_sources(UTIL_LIB PUBLIC ${FILES})

target_include_directories(UTIL_LIB INTERFACE ${CMAKE_SOURCE_DIR}/include/util)