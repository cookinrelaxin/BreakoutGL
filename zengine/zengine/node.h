#ifndef Z_NODE_H
#define Z_NODE_H

#include "size_2.h"
#include "pos_2.h"
#include "velocity_2.h"

#include <set>

class SpriteRenderer;

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
            , velocity(0,0) {}
        Node(const Node& other)
            : size(other.size)
            , position(other.position)
            , velocity(other.velocity) {}

        ~Node();

        size2 size;
        pos2 position;
        velocity2 velocity;
        int zPosition;

        void addChild(Node* child);
        // void removeChild(Node* child);

    private:
    protected:

        virtual void draw(SpriteRenderer* renderer);

        std::multiset<Node*, NodeComparator> children_;
};

};

#endif
