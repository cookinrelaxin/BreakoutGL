#include "zengine.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "znode.h"
#include "zsprite_node.h"
#include "zscene_node.h"
#include "collisions.h"
#include "require.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

void ZEngine::create(v8pp::context* context) {
    v8::Isolate* isolate = context->isolate();

    v8pp::class_<glm::vec2> vec2_class = Vec2::create(isolate);
    v8pp::class_<glm::vec3> vec3_class = Vec3::create(isolate);
    v8pp::class_<glm::vec4> vec4_class = Vec4::create(isolate);

    v8pp::class_<ZNode> znode_class = ZNode::create(isolate);
    v8pp::class_<ZSpriteNode> zsprite_node_class = ZSpriteNode::create(isolate);
    v8pp::class_<ZSceneNode> zscene_node_class = ZSceneNode::create(isolate);

    v8pp::class_<ZCollision> zcollision_class = ZCollision::create(isolate);

    Require::create(context);

    v8pp::module m(isolate);
    m
        .set("Vec2", vec2_class)
        .set("Vec3", vec3_class)
        .set("Vec4", vec4_class)
        .set("Node", znode_class)
        .set("SpriteNode", zsprite_node_class)
        .set("SceneNode", zscene_node_class)
        .set("Collision", zcollision_class)
        ;

    //KEY VALUES
    m
        .set_const("KEY_UNKNOWN",    GLFW_KEY_UNKNOWN)
        .set_const("KEY_SPACE",      GLFW_KEY_SPACE)
        .set_const("KEY_APOSTROPHE", GLFW_KEY_APOSTROPHE)
        .set_const("KEY_COMMA",      GLFW_KEY_COMMA)
        .set_const("KEY_MINUS",      GLFW_KEY_MINUS)
        .set_const("KEY_PERIOD",     GLFW_KEY_PERIOD)
        .set_const("KEY_SLASH",      GLFW_KEY_SLASH)

        .set_const("KEY_0", GLFW_KEY_0)
        .set_const("KEY_1", GLFW_KEY_1)
        .set_const("KEY_2", GLFW_KEY_2)
        .set_const("KEY_3", GLFW_KEY_3)
        .set_const("KEY_4", GLFW_KEY_4)
        .set_const("KEY_5", GLFW_KEY_5)
        .set_const("KEY_6", GLFW_KEY_6)
        .set_const("KEY_7", GLFW_KEY_7)
        .set_const("KEY_8", GLFW_KEY_8)
        .set_const("KEY_9", GLFW_KEY_9)

        .set_const("KEY_SEMICOLON", GLFW_KEY_SEMICOLON)
        .set_const("KEY_EQUAL", GLFW_KEY_EQUAL)

        .set_const("KEY_A", GLFW_KEY_A)
        .set_const("KEY_B", GLFW_KEY_B)
        .set_const("KEY_C", GLFW_KEY_C)
        .set_const("KEY_D", GLFW_KEY_D)
        .set_const("KEY_E", GLFW_KEY_E)
        .set_const("KEY_F", GLFW_KEY_F)
        .set_const("KEY_G", GLFW_KEY_G)
        .set_const("KEY_H", GLFW_KEY_H)
        .set_const("KEY_I", GLFW_KEY_I)
        .set_const("KEY_J", GLFW_KEY_J)
        .set_const("KEY_K", GLFW_KEY_K)
        .set_const("KEY_L", GLFW_KEY_L)
        .set_const("KEY_M", GLFW_KEY_M)
        .set_const("KEY_N", GLFW_KEY_N)
        .set_const("KEY_O", GLFW_KEY_O)
        .set_const("KEY_P", GLFW_KEY_P)
        .set_const("KEY_Q", GLFW_KEY_Q)
        .set_const("KEY_R", GLFW_KEY_R)
        .set_const("KEY_S", GLFW_KEY_S)
        .set_const("KEY_T", GLFW_KEY_T)
        .set_const("KEY_U", GLFW_KEY_U)
        .set_const("KEY_V", GLFW_KEY_V)
        .set_const("KEY_W", GLFW_KEY_W)
        .set_const("KEY_X", GLFW_KEY_X)
        .set_const("KEY_Y", GLFW_KEY_Y)
        .set_const("KEY_Z", GLFW_KEY_Z)

        .set_const("KEY_LEFT_BRACKET",  GLFW_KEY_LEFT_BRACKET)
        .set_const("KEY_BACKSLASH",     GLFW_KEY_BACKSLASH)
        .set_const("KEY_RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET)
        .set_const("KEY_GRAVE_ACCENT",  GLFW_KEY_GRAVE_ACCENT)
        .set_const("KEY_WORLD_1",       GLFW_KEY_WORLD_1)
        .set_const("KEY_WORLD_2",       GLFW_KEY_WORLD_2)
        .set_const("KEY_ESCAPE",        GLFW_KEY_ESCAPE)
        .set_const("KEY_ENTER",         GLFW_KEY_ENTER)
        .set_const("KEY_TAB",           GLFW_KEY_TAB)
        .set_const("KEY_BACKSPACE",     GLFW_KEY_BACKSPACE)
        .set_const("KEY_INSERT",        GLFW_KEY_INSERT)
        .set_const("KEY_DELETE",        GLFW_KEY_DELETE)

        .set_const("KEY_RIGHT",         GLFW_KEY_RIGHT)
        .set_const("KEY_LEFT",          GLFW_KEY_LEFT)
        .set_const("KEY_DOWN",          GLFW_KEY_DOWN)
        .set_const("KEY_UP",            GLFW_KEY_UP)

        .set_const("KEY_PAGE_UP",       GLFW_KEY_PAGE_UP)
        .set_const("KEY_PAGE_DOWN",     GLFW_KEY_PAGE_DOWN)
        .set_const("KEY_HOME",          GLFW_KEY_HOME)
        .set_const("KEY_END",           GLFW_KEY_END)

        .set_const("KEY_CAPS_LOCK",   GLFW_KEY_CAPS_LOCK)
        .set_const("KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK)
        .set_const("KEY_NUM_LOCK",    GLFW_KEY_NUM_LOCK)

        .set_const("KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN)
        .set_const("KEY_PAUSE",        GLFW_KEY_PAUSE)

        .set_const("KEY_F1",  GLFW_KEY_F1)
        .set_const("KEY_F2",  GLFW_KEY_F2)
        .set_const("KEY_F3",  GLFW_KEY_F3)
        .set_const("KEY_F4",  GLFW_KEY_F4)
        .set_const("KEY_F5",  GLFW_KEY_F5)
        .set_const("KEY_F6",  GLFW_KEY_F6)
        .set_const("KEY_F7",  GLFW_KEY_F7)
        .set_const("KEY_F8",  GLFW_KEY_F8)
        .set_const("KEY_F9",  GLFW_KEY_F9)
        .set_const("KEY_F10", GLFW_KEY_F10)
        .set_const("KEY_F11", GLFW_KEY_F11)
        .set_const("KEY_F12", GLFW_KEY_F12)
        .set_const("KEY_F13", GLFW_KEY_F13)
        .set_const("KEY_F14", GLFW_KEY_F14)
        .set_const("KEY_F15", GLFW_KEY_F15)
        .set_const("KEY_F16", GLFW_KEY_F16)
        .set_const("KEY_F17", GLFW_KEY_F17)
        .set_const("KEY_F18", GLFW_KEY_F18)
        .set_const("KEY_F19", GLFW_KEY_F19)
        .set_const("KEY_F20", GLFW_KEY_F20)
        .set_const("KEY_F21", GLFW_KEY_F21)
        .set_const("KEY_F22", GLFW_KEY_F22)
        .set_const("KEY_F23", GLFW_KEY_F23)
        .set_const("KEY_F24", GLFW_KEY_F24)
        .set_const("KEY_F25", GLFW_KEY_F25)

        .set_const("KEY_KP_0", GLFW_KEY_KP_0)
        .set_const("KEY_KP_1", GLFW_KEY_KP_1)
        .set_const("KEY_KP_2", GLFW_KEY_KP_2)
        .set_const("KEY_KP_3", GLFW_KEY_KP_3)
        .set_const("KEY_KP_4", GLFW_KEY_KP_4)
        .set_const("KEY_KP_5", GLFW_KEY_KP_5)
        .set_const("KEY_KP_6", GLFW_KEY_KP_6)
        .set_const("KEY_KP_7", GLFW_KEY_KP_7)
        .set_const("KEY_KP_8", GLFW_KEY_KP_8)
        .set_const("KEY_KP_9", GLFW_KEY_KP_9)

        .set_const("KEY_KP_DECIMAL",  GLFW_KEY_KP_DECIMAL)
        .set_const("KEY_KP_DIVIDE",   GLFW_KEY_KP_DIVIDE)
        .set_const("KEY_KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY)
        .set_const("KEY_KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT)
        .set_const("KEY_KP_ADD",      GLFW_KEY_KP_ADD)
        .set_const("KEY_KP_ENTER",    GLFW_KEY_KP_ENTER)
        .set_const("KEY_KP_EQUAL",    GLFW_KEY_KP_EQUAL)

        .set_const("KEY_LEFT_SHIFT",   GLFW_KEY_LEFT_SHIFT)
        .set_const("KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL)
        .set_const("KEY_LEFT_ALT",     GLFW_KEY_LEFT_ALT)
        .set_const("KEY_LEFT_SUPER",   GLFW_KEY_LEFT_SUPER)

        .set_const("KEY_RIGHT_SHIFT",   GLFW_KEY_RIGHT_SHIFT)
        .set_const("KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL)
        .set_const("KEY_RIGHT_ALT",     GLFW_KEY_RIGHT_ALT)
        .set_const("KEY_RIGHT_SUPER",   GLFW_KEY_RIGHT_SUPER)

        .set_const("KEY_MENU",          GLFW_KEY_MENU)
        ;
    
    context->set("ZEngine", m);
}
