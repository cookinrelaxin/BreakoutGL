#ifndef Z_NODE_H
#define Z_NODE_H

#include "size_2.h"
#include "pos_2.h"
#include "velocity_2.h"

#include <set>
#include <string>

class SpriteRenderer;
class TextRenderer;

namespace Z {

class Engine;

class Node {
    friend class SceneNode;

    struct NodeComparator {
        bool operator() (const Node* left, const Node* right) {
            return left->zPosition < right->zPosition;
        }
    };

    public:
        Node()
            : size(10,10)
            , position(0,0)
            , zPosition(0)
            , name("default")
            , velocity(0,0) {}
        Node(const Node& other)
            : size(other.size)
            , position(other.position)
            , zPosition(other.zPosition)
            , name(other.name)
            , velocity(other.velocity) {}

        ~Node();

        size2 size;
        pos2 position;
        velocity2 velocity;
        int zPosition;
        std::string name;

        Node* parent;

        void addChild(Node* child);
        void removeChild(std::string name);

    private:
    protected:

        virtual void draw(SpriteRenderer* spriteRenderer,
                          TextRenderer* textRenderer);

        std::multiset<Node*, NodeComparator> children_;
};

};

#endif
