project(test LANGUAGES CXX)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test.cc)

target_include_directories(${PROJECT_NAME} PUBLIC 
${CMAKE_SOURCE_DIR}/include/util
${CMAKE_SOURCE_DIR}/include/net)