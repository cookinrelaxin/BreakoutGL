#ifndef ZNODE_H
#define ZNODE_H

#include <glm/glm.hpp>

class ZNode {
    public:
        ZNode();
        ~ZNode();
        //POSITION
        glm::vec2& get_position() { return pos_; }
        void set_position(glm::vec2 new_pos) {
            pos_ = new_pos;
        }
        
        int zPosition;
        glm::vec2 frame;

        //SCALING
        glm::vec2 scale;

        //ROTATION
        float zRotation;

        //NODE HIERARCHY
        void addChild(ZNode* child);
        void removeChild(ZNode* child);
        std::vector<ZNode*> getChildren();
        ZNode* getParent();

        //NAME
        ZNode* getChildWithName(std::string name);
        std::string name;

        //ACTIONS
        // void runAction(ZAction* action);

        //COLLISIONS
        bool containsPoint(glm::vec2 point);

    private:
        glm::vec2 pos_;
    protected:
};

#endif

