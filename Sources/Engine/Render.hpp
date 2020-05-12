
#ifndef RENDER_HPP
#define RENDER_HPP

#include "Interface.hpp"
#include "VBO.hpp"
#include "Shaders.hpp"

#include <glm.hpp>

class CRender
{
public:

    GLuint AxisVAO;
    CVBO AxisVBO;

    GLuint GridVAO;
    CVBO GridVBO;

    glm::vec2 Start, End;

    CProgram ColorShader;
    CProgram MainShader;
    CProgram SelectionShader;

public:

    CRender(void);
    ~CRender(void);

    void Scene(void);
    void Update(int Index);
    void Viewport(int Index);
    void Grid(int Index);
    void Axis(int Index, int Length);

};

#endif
