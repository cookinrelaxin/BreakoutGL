#ifndef ZNODE_H
#define ZNODE_H

#include <glm/glm.hpp>

#include <set>
#include <string>

#include <v8pp/module.hpp>
#include "v8pp/class.hpp"

#include "sprite_renderer.h"

class ZNode {
    public:
        struct NodeComparator {
            bool operator()(ZNode a, ZNode b) const {
                return a.get_zposition() < b.get_zposition();
            }
        };

        //CONSTRUCTORS
        ZNode();
        ~ZNode();

        //POSITION
        glm::vec2& get_position();
        void       set_position(glm::vec2 new_pos);

        int& get_zposition();
        void       set_zposition(int new_zpos);
        
        // int zPosition;
        // glm::vec2 frame;
        glm::vec2& get_size();
        void       set_size(glm::vec2 new_size);

        //SCALING
        // glm::vec2 scale;

        //ROTATION
        // float zRotation;

        //NODE HIERARCHY
        void                add_child(ZNode* child);
        void                remove_child(ZNode* child);
        std::multiset<ZNode, NodeComparator>const & get_children();
        ZNode*              get_parent();

        //NAME
        // ZNode* getChildWithName(std::string name);
        std::string get_name();
        void        set_name(std::string new_name);

        //ACTIONS
        // void runAction(ZAction* action);

        //COLLISIONS
        // bool containsPoint(glm::vec2 point);
        void doesCollideAABBAABB(v8::FunctionCallbackInfo<v8::Value> const& args);
        void getCollision(v8::FunctionCallbackInfo<v8::Value> const& args);

        //RENDERING
        virtual void draw(SpriteRenderer& renderer) const;

        //SCRIPTING
        static v8pp::class_<ZNode> create(v8::Isolate* isolate);

    protected:
        // glm::vec2* pos_;
        glm::vec2 pos_;
        int zpos_;
        // glm::vec2 frame_;
        glm::vec2 size_;

        std::multiset<ZNode, NodeComparator> children_;
        ZNode* parent_;

        std::string name_;
};

#endif
