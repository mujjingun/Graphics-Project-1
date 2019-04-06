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
    src/playersystem.cpp
    src/components.cpp
    src/simpleshader.cpp
    src/pointshader.cpp
    src/pointdata.cpp
    src/hpbar.cpp
    src/hpbarshader.cpp
    src/entity.cpp
    src/ecsengine.cpp
    src/backgroundsystem.cpp
    src/rendersystem.cpp
    src/physicssystem.cpp
    src/enemysystem.cpp
    src/scenesystem.cpp
    src/projectilesystem.cpp
    src/shrapnelsystem.cpp
)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLM_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_ENABLE_EXPERIMENTAL)

# Copy shaders
set(SHADER_FILES
    shaders/simple.vert.glsl
    shaders/simple.frag.glsl
    shaders/star.vert.glsl
    shaders/star.frag.glsl
    shaders/hdr.vert.glsl
    shaders/hdr.frag.glsl
    shaders/hpbar.vert.glsl
    shaders/hpbar.frag.glsl
)

foreach(SHADER_FILE ${SHADER_FILES})
    configure_file(${SHADER_FILE} ${SHADER_FILE} COPYONLY)
endforeach()

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
