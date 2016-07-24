#include <cassert>
#include <cstdlib>

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_set>

#include "tinyxml2.h"
#include <glm/glm.hpp>

using namespace tinyxml2;

// Output socket types
enum SocketOutType {
    COLOR,
    UV,
    NORMAL,
};

// blend types
enum BlendType {
    MIX,
    ADD,
    MULTIPLY,
};

enum ColorType {
    RGB,
};

enum HueInterpolationType {
    NEAR,
};

enum InterpolationType {
    LINEAR,
};

struct ColorRampElement {
    glm::vec3 color;
    float alpha;
    float position;
};

struct ConverterNode {
    virtual void addChild(XMLElement* element) = 0;
};


struct ShaderNode;
struct ShaderNodeOutput;

struct ColorSocket_in;
struct FloatSocket_in;
struct FactorSocket_in;

// Shader nodes
struct ShaderNode : ConverterNode {
    std::string name;
    virtual void addChild(XMLElement* element) = 0;
};

struct ShaderNodeOutput : public ShaderNode {
    ColorSocket_in* color;
    FloatSocket_in* alpha;
    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct ShaderNodeMixRGB : public ShaderNode {
    BlendType blendType;
    bool clamp;

    FactorSocket_in* factor;
    ColorSocket_in* color1;
    ColorSocket_in* color2;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct ShaderNodeValToRGB : public ShaderNode {
    ColorType colorMode;
    HueInterpolationType hueInterpolationType;
    InterpolationType interpolationType;

    FactorSocket_in* factor;
    std::vector<ColorRampElement> colorRampElements; 

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct ShaderNodeTexture : public ShaderNode {
    std::string name;
    std::string path;

    unsigned int width;
    unsigned int height;

    unsigned int glTextureID;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct ShaderNodeGeometry : public ShaderNode {
    std::string color_layer;
    std::string uv_layer;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

// Input sockets

struct Socket_in : ConverterNode {
    virtual void addChild(XMLElement* element) = 0;
};

struct ColorSocket_in : Socket_in  {
    glm::vec3 defaultColor;
    ShaderNode* fromNode;
    SocketOutType fromSocketType;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct FloatSocket_in : Socket_in {
    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct FactorSocket_in {
    float defaultValue;
    ShaderNode* fromNode;
    SocketOutType fromSocketType;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

struct VectorSocket_in : Socket_in {
    glm::vec3 defaultValue;
    ShaderNode* fromNode;
    SocketOutType fromSocketType;

    virtual void addChild(XMLElement* element) {
        //TODO
        
    }
};

class fs_generator {
    public:
        fs_generator(std::string nodePath);
        //~fs_generator();
        /* Public Data */
    private:
        ShaderNodeOutput* output;

};

//void ShaderNode::addChild(XMLElement* element) {
//}

fs_generator::fs_generator(std::string nodePath) {
    XMLDocument doc;
    doc.LoadFile(nodePath.c_str());
    if (doc.Error()) {
        doc.PrintError();
        abort();
    }

    XMLElement* rootElement = doc.RootElement();
    assert(rootElement != nullptr);

    std::string rootName = rootElement->Name();
    assert(rootName == "shader");

    XMLElement* outputElement = rootElement->FirstChildElement();
    assert(outputElement != nullptr);
    std::string outputName = outputElement->Name();
    assert(outputName == "ShaderNodeOutput");

    this->output = new ShaderNodeOutput;

    auto visit = [](XMLElement* element) {
        std::string name = element->Name();
        std::cout << "name: " << name << std::endl;
    };

    //auto addChild = [](ShaderNode* node, XMLElement* element) {
    //};

    //auto addChild = [](FloatSocket_in* node, XMLElement* element) {
    //};

    std::stack<XMLElement*> elementStack;
    XMLElement* currentElement = outputElement;
    elementStack.push(currentElement);

    std::stack<ConverterNode*> converterNodeStack;
    ConverterNode* currentConverterNode = this->output;
    converterNodeStack.push(currentConverterNode);

    while (!elementStack.empty()) {
        currentElement = elementStack.top(); elementStack.pop();
        currentConverterNode = converterNodeStack.top(); converterNodeStack.pop();

        visit(currentElement);
        XMLElement* child = currentElement->LastChildElement();
        while (child != nullptr) {
            currentConverterNode->addChild(child);
            elementStack.push(child);
            child = child->PreviousSiblingElement();
        }
    }
    
}

int main(int argc, const char *argv[]) {
    fs_generator gen("/Users/johnfeldcamp/temp.xml");
    return 0;
}
