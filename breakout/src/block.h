#ifndef BLOCK_H
#define BLOCK_H

class Block : public Z::SpriteNode {
    public:
        Block() : destroyed(false) {}
        bool destroyed;
        bool solid;
    private:
};

#endif

