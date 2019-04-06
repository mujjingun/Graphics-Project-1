#ifndef POINTDATA_H
#define POINTDATA_H

#include <glm/glm.hpp>

#include "graphics/vertexarray.h"
#include "graphics/vertexbuffer.h"

namespace ou {

struct PointAttrib {
    glm::vec2 pos;
    float size;
    glm::vec3 color;
};

struct PointData {
    VertexBuffer vbo;
    VertexArray vao;

    PointData(int size);
};
}

#endif // POINTDATA_H
