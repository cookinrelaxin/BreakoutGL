#ifndef ZSCENE_NODE_H
#define ZSCENE_NODE_H

#include <glm/glm.hpp>

#include <include/v8.h>

#include "v8pp/module.hpp"
#include "v8pp/class.hpp"
#include "v8pp/convert.hpp"
#include "v8pp/object.hpp"

#include <string>

#include "znode.h"

class ZSceneNode : public ZNode {
    public:
        ZSceneNode();
        ~ZSceneNode();
        //BACKGROUND
        glm::vec4& get_background_color() { return background_color_; }
        void set_background_color(glm::vec4 new_background_color) {
            background_color_ = new_background_color;
        }
        std::string temp;

    private:
        glm::vec4 background_color_;
};

#endif

