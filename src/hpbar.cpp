#include "hpbar.h"
#include "hpbarshader.h"
#include "vertexarray.h"
#include "vertexbuffer.h"

namespace ou {

namespace {

    struct HpBarData {
        VertexBuffer vbo;
        VertexArray vao;

        static HpBarData* self()
        {
            static HpBarData sself;
            return &sself;
        }

    private:
        HpBarData()
        {
            std::vector<glm::vec2> vertices = {
                { -1, -1 },
                { -1, 1 },
                { 1, 1 },
                { 1, -1 },
            };

            vbo.setData(vertices, GL_STATIC_DRAW);

            auto vertexBinding = vao.getBinding(0);
            vertexBinding.bindVertexBuffer(vbo, 0, sizeof(glm::vec2));

            auto vertexAttr = vao.enableVertexAttrib(0);
            vertexAttr.setFormat(2, GL_FLOAT, GL_FALSE, 0);
            vertexAttr.setBinding(vertexBinding);
        }
    };
}

HpBar::HpBar()
{
}

void HpBar::render(glm::mat4 mat, float value)
{
    HpBarShader::self()->setUniform(0, mat);
    HpBarShader::self()->setUniform(1, value);

    HpBarData::self()->vao.use();
    HpBarShader::self()->use();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
}
