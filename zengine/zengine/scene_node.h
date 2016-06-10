#ifndef Z_SCENE_NODE_H
#define Z_SCENE_NODE_H

#include "node.h"

#include "glm/glm.hpp"

namespace Z {

class SceneNode : public Node {
    friend class Engine;
    public:
        SceneNode();
        ~SceneNode();

        glm::vec4 backgroundColor;
    private:
    protected:
        void draw(SpriteRenderer* spriteRenderer, TextRenderer* textRenderer);

};

};

#endif
