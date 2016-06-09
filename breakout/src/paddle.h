#ifndef PADDLE_H
#define PADDLE_H

#include "zengine/sprite_node.h"

class Paddle : public Z::SpriteNode {
    public:
        Paddle();
        ~Paddle();
    private:
        void draw(SpriteRenderer& renderer) const;
};

#endif
