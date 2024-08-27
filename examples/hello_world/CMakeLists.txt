find_package(OpenGL REQUIRED)

set(HelloWorld_SRCS
"main.cpp"
)

add_executable(Example_HelloWorld ${HelloWorld_SRCS})
target_include_directories(Example_HelloWorld PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Example_HelloWorld Cacti::Cacti SDL2::SDL2main)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(Example_HelloWorld OpenGL::GL)
endif()