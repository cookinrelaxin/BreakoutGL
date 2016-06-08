#ifndef Z_NODE_H
#define Z_NODE_H

#include "glm/glm.hpp"
#include "sprite_renderer.h"
#include <vector>

namespace Z {

class Engine;

class Node {
    public:
        Node()
            : size_(10,10)
            , position_(0,0) {}
        ~Node();

        glm::vec2 size();
        void size(glm::vec2 newSize);

        glm::vec2 position();
        void position(glm::vec2 newPosition);

        int zPosition();
        void zPosition(int newZPosition);

        void addChild(Node* child);
        // void removeChild(Node* child);

    private:
    protected:

        virtual void draw(SpriteRenderer& renderer) const;

        glm::vec2 size_;
        glm::vec2 position_;
        int zPosition_;

        std::vector<Node*> children_;
};

};

#endif
