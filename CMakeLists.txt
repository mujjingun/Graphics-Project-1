cmake_minimum_required(VERSION 3.7)
project(graphics1)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/scene.cpp
    src/components.cpp
    src/simpleshader.cpp
    src/pointshader.cpp
    src/pointdata.cpp
    src/hpbar.cpp
    src/hpbarshader.cpp

    src/ecs/entity.cpp
    src/ecs/ecsengine.cpp

    src/entitysystems/backgroundsystem.cpp
    src/entitysystems/rendersystem.cpp
    src/entitysystems/physicssystem.cpp
    src/entitysystems/enemysystem.cpp
    src/entitysystems/scenesystem.cpp
    src/entitysystems/projectilesystem.cpp
    src/entitysystems/shrapnelsystem.cpp
    src/entitysystems/playersystem.cpp

    src/graphics/texture.cpp
    src/graphics/framebuffer.cpp
    src/graphics/shader.cpp
    src/graphics/vertexarray.cpp
    src/graphics/vertexbuffer.cpp
    src/graphics/rawbufferview.cpp
    src/graphics/renderbuffer.cpp
)
target_link_libraries(${PROJECT_NAME}
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLM_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)
target_compile_definitions(${PROJECT_NAME} PUBLIC)

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
