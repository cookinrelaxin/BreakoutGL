#ifndef ZSCENE_NODE_H
#define ZSCENE_NODE_H

#include <glm/glm.hpp>

#include <string>

#include "znode.h"

class ZSceneNode : public ZNode {
    public:
        ZSceneNode();
        ~ZSceneNode();
        //BACKGROUND
        glm::vec4& get_background_color();
        void set_background_color(glm::vec4 new_background_color);

        //Scripting

        static v8pp::class_<ZSceneNode> create(v8::Isolate* isolate);

    protected:
        glm::vec4 background_color_;
};

#endif

