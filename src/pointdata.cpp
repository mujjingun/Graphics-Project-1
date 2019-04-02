#include "pointdata.h"

ou::PointData::PointData(int size)
{
    vbo.reserve(size * sizeof(PointAttrib), GL_STATIC_DRAW);

    auto vertexBinding = vao.getBinding(0);
    vertexBinding.bindVertexBuffer(vbo, 0, sizeof(PointAttrib));

    auto vertexAttr = vao.enableVertexAttrib(0);
    vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, offsetof(PointAttrib, pos));
    vertexAttr.setBinding(vertexBinding);

    auto sizeAttr = vao.enableVertexAttrib(1);
    sizeAttr.setFormat(1, GL_FLOAT, GL_FALSE, offsetof(PointAttrib, size));
    sizeAttr.setBinding(vertexBinding);

    auto colorAttr = vao.enableVertexAttrib(2);
    colorAttr.setFormat(3, GL_FLOAT, GL_FALSE, offsetof(PointAttrib, color));
    colorAttr.setBinding(vertexBinding);
}
