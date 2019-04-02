#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <memory>

namespace ou {

class Scene;
struct BackgroundStates;

class Background {
    Scene* m_scene;

    std::unique_ptr<BackgroundStates> m_s;

public:
    Background(Scene* scene);
    ~Background();
    Background(Background&&);
    Background& operator=(Background&&);

    void render();
};
}

#endif // BACKGROUND_H
