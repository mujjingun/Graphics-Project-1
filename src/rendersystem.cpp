#include "rendersystem.h"
#include "components.h"
#include "ecsengine.h"
#include "entity.h"
#include "pointshader.h"

namespace ou {

RenderSystem::RenderSystem()
    : m_starBuf(300)
{
}

void RenderSystem::update(ECSEngine& engine, float deltaTime)
{
    SceneComponent const& scene = engine.getOne<SceneComponent>();
    Entity const& player = engine.getOneEnt<PlayerComponent>();

    std::vector<PointAttrib> attribs;
    for (Entity& ent : engine.components<StarComponent>()) {
        StarComponent star = ent.get<StarComponent>();
        PosComponent pos = ent.get<PosComponent>();

        PointAttrib attrib;
        attrib.pos = pos.pos;
        attrib.pos -= player.get<PosComponent>().pos * float(star.size) * 0.1f;
        attrib.color = star.color;
        attrib.size = star.size;
        attribs.push_back(attrib);
    }

    m_starBuf.vbo.updateData(attribs);

    PointShader::self()->setUniform(0, scene.viewProjMat);

    m_starBuf.vao.use();
    PointShader::self()->use();
    glDrawArrays(GL_POINTS, 0, int(attribs.size()));
}
}
