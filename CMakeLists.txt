cmake_minimum_required(VERSION 3.7)
project(graphics1)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/scene.cpp
    src/shader.cpp
    src/vertexarray.cpp
    src/vertexbuffer.cpp
    src/rawbufferview.cpp
    src/framebuffer.cpp
    src/texture.cpp
    src/renderbuffer.cpp
    src/terrain.cpp
    src/airplane.cpp
    src/enemy.cpp
    src/house.cpp
    src/background.cpp
    src/simpleshader.cpp
    src/pointshader.cpp
    src/pointdata.cpp
)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLM_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})

# Copy shaders
configure_file(shaders/simple.vert.glsl shaders/simple.vert.glsl COPYONLY)
configure_file(shaders/simple.frag.glsl shaders/simple.frag.glsl COPYONLY)
configure_file(shaders/star.vert.glsl shaders/star.vert.glsl COPYONLY)
configure_file(shaders/star.frag.glsl shaders/star.frag.glsl COPYONLY)
configure_file(shaders/hdr.vert.glsl shaders/hdr.vert.glsl COPYONLY)
configure_file(shaders/hdr.frag.glsl shaders/hdr.frag.glsl COPYONLY)

# Testing
#enable_testing()
#find_package(GTest REQUIRED)
#
#add_executable(runUnitTests
#    tests/unittests.cpp
#)
#
#target_link_libraries(runUnitTests gtest gtest_main)
#
#add_test(
#    NAME runUnitTests
#    COMMAND runUnitTests
#)
