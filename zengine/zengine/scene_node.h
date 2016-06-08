#ifndef Z_SCENE_NODE_H
#define Z_SCENE_NODE_H

#include "node.h"

#include "glm/glm.hpp"

namespace Z {

class SceneNode : public Node {
    public:
        SceneNode();
        ~SceneNode();

        glm::vec4 backgroundColor();
        void backgroundColor(glm::vec4 newBackgroundColor);
    private:
    protected:
        glm::vec4 backgroundColor_;

};

};

#endif
