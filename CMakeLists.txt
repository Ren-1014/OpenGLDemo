cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE "LEGACY")
set(CMAKE_VERBOSE_MAKEFILE:BOOL "ON")
#SET(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" CACHE STRING "" FORCE)

project(OpenGLdemo)
add_definitions(-w)

aux_source_directory(./src main_srcs)


file(GLOB static_libraries  "${CMAKE_SOURCE_DIR}/depend/lib/*.lib")
# file(GLOB shared_libraries  "${CMAKE_SOURCE_DIR}/depend/lib/*.dll")
include_directories(${CMAKE_SOURCE_DIR}/depend/include)

# add_executable(main ${main_srcs})
add_executable(OpenGLdemo 
src/main.cpp
# src/Utils.hpp
src/Config.hpp
src/Shader.hpp
src/OBJ_Loader.hpp)

target_link_libraries(OpenGLdemo PRIVATE ${static_libraries})

# target_link_libraries(main PRIVATE ${shared_libraries} )


# set_target_properties(OpenGLdemo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/resource")

# 拷贝动态库到可执行目录下
add_custom_command(TARGET OpenGLdemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/depend/dll/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:OpenGLdemo>)

# add_custom_command(
# 	TARGET main POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/patch" $<TARGET_FILE_DIR:main>
# )
#add_custom_command(
#		TARGET main POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy_directory "${shared_libraries}" $<TARGET_FILE_DIR:main>
#)


