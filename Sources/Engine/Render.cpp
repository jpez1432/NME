
#include "Render.hpp"

#include "Model.hpp"

#include <glm.hpp>
#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_access.hpp>

extern CModel *Model;

CRender::CRender(void) : Start(0.0f), End(0.0f)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GEQUAL, 0.75f);
    glEnable(GL_ALPHA_TEST);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_NORMALIZE);

    if (MainShader.Create("Main.frag", "Main.vert", NULL)) {
        Logger->Text("Created Main Shader Program");
    } else {
        Logger->Error("Error Creating Main Shader Program");
    }

    if (ColorShader.Create("Color.frag", "Color.vert", NULL)) {
        Logger->Text("Created Color Shader Program");
    } else {
        Logger->Error("Error Creating Color Shader Program");
    }


    Interface::Create();

}

CRender::~CRender(void)
{
    Interface::Destroy();
}

void CRender::Update(int Index)
{
    int Dx = End.x - Start.x;
    int Dy = Start.y - End.y;

    if (glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {

        Config->Viewports[Index].Pan.x -= Dx / 10.0f;
        Config->Viewports[Index].Pan.y -= Dy / 10.0f;

    } else if (glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

        if(Config->Viewports[Index].Projection == 0) {
            Config->Viewports[Index].Pan.z -= (Dx + Dy) / 10.0f;
        }

    } else if (glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && Config->Viewports[Index].Projection == 0) {

        Config->Viewports[Index].Rotate.x += (Dy * 180.0f) / 100.0f;
        Config->Viewports[Index].Rotate.y -= (Dx * 180.0f) / 100.0f;
        Config->Viewports[Index].Rotate.x -= 360.0f * std::floor(Config->Viewports[Index].Rotate.x * (1.0f / 360.0f));
        Config->Viewports[Index].Rotate.y -= 360.0f * std::floor(Config->Viewports[Index].Rotate.y * (1.0f / 360.0f));

    }

}

void CRender::Scene(void)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < 4; i++) {
        Viewport(i);
    }

    Interface::Render(Config->Graphics.Width, Config->Graphics.Height);

    glfwSwapBuffers(GLWindow);
}

void CRender::Viewport(int Index)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(Viewports[Index].X, Viewports[Index].Y, Viewports[Index].Width + 1, Viewports[Index].Height + 1);
    glViewport(Viewports[Index].X, Viewports[Index].Y, Viewports[Index].Width + 1, Viewports[Index].Height + 1);

    glClearColor(0.28f, 0.28f, 0.28f, 0.75f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);
    glScissor(Viewports[Index].X + 2, Viewports[Index].Y + 2, Viewports[Index].Width - 2, Viewports[Index].Height - 2);
    glViewport(Viewports[Index].X + 2, Viewports[Index].Y + 2, Viewports[Index].Width - 2, Viewports[Index].Height - 2);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 Projection, ModelView;
    if (Config->Viewports[Index].Projection == 0) {
        Projection = glm::perspective(glm::radians(60.0f), float(Viewports[Index].Width) / float(Viewports[Index].Height), 0.001f, 100000.0f);
    } else {
        Projection = glm::ortho(0.0f, float(Viewports[Index].Width), 0.0f, float(Viewports[Index].Height), -10000.0f, 10000.0f);
    }

    if (glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT) && Index == Config->Editor.Viewport && Config->Editor.Tool == 0) {

        if(Config->General.SnapToGrid) {
            ModelView = glm::translate(glm::mat4(1.0f), glm::vec3(int(Viewports[Index].Width / 2), int(Viewports[Index].Height / 2), 0));
        } else {
            ModelView = glm::translate(glm::mat4(1.0f), glm::vec3(Viewports[Index].Width / 2, Viewports[Index].Height / 2, 0));
        }
        ModelView = glm::scale(ModelView, glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z));

        if(Config->Viewports[Index].Projection != 0) {

            if(Config->General.SnapToGrid) {
                ModelView = glm::translate(ModelView, glm::vec3(int(Config->Viewports[Index].Pan.x), int(Config->Viewports[Index].Pan.y), int(Config->Viewports[Index].Pan.z)));
            } else {
                ModelView = glm::translate(ModelView, Config->Viewports[Index].Pan);
            }

            ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.x), glm::vec3(1, 0, 0));
            ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.y), glm::vec3(0, 1, 0));
            ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.z), glm::vec3(0, 0, 1));

        }

        MainShader.Bind();
        MainShader.SetUniform("bSelection", true);
        MainShader.SetUniform("bProjection", false);
        MainShader.SetUniform("projectionMatrix", Projection);
        MainShader.SetUniform("modelViewMatrix", ModelView);
        MainShader.SetUniform("vColor", glm::vec4(1.0f));
        MainShader.SetUniform("bTextured", false);
        MainShader.SetUniform("bLighted", false);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        Model->Render(Config->Editor.Viewport, ColorShader, false);
        Tools::Select(Index);

        MainShader.SetUniform("bSelection", false);
//return;
    }

    glClearColor(Config->Viewports[Index].ClearColor.r, Config->Viewports[Index].ClearColor.g, Config->Viewports[Index].ClearColor.b, Config->Viewports[Index].ClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(Config->Viewports[Index].Projection == 0) {

        MainShader.Bind();
        MainShader.SetUniform("projectionMatrix", Projection);

        ModelView = glm::translate(glm::mat4(1.0f), Config->Viewports[Index].Pan);
        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.x), glm::vec3(1, 0, 0));
        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.y), glm::vec3(0, 1, 0));
        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.z), glm::vec3(0, 0, 1));
        ModelView = glm::scale(ModelView, Config->Viewports[Index].Scale);

        ColorShader.Bind();
        ColorShader.SetUniform("projectionMatrix", Projection);
        ColorShader.SetUniform("modelViewMatrix", ModelView);

        if(Config->Viewports[Index].Grid) Grid(Index);

        if(Config->Viewports[Index].Axis) Axis(Index, Config->Viewports[Index].GridSize);

    } else {

        ColorShader.Bind();
        ColorShader.SetUniform("projectionMatrix", Projection);

        if(Config->General.SnapToGrid) {
            ModelView = glm::translate(glm::mat4(1.0f), glm::vec3(int(Viewports[Index].Width / 2), int(Viewports[Index].Height / 2), 0));
        } else {
            ModelView = glm::translate(glm::mat4(1.0f), glm::vec3(Viewports[Index].Width / 2, Viewports[Index].Height / 2, 0));
        }
        ModelView = glm::scale(ModelView, glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z));

        ColorShader.SetUniform("modelViewMatrix", ModelView);
        if(Config->Viewports[Index].Grid) Grid(Index);

        if(Config->General.SnapToGrid) {
            ModelView = glm::translate(ModelView, glm::vec3(int(Config->Viewports[Index].Pan.x), int(Config->Viewports[Index].Pan.y), int(Config->Viewports[Index].Pan.z)));
        } else {
            ModelView = glm::translate(ModelView, Config->Viewports[Index].Pan);
        }

        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.x), glm::vec3(1, 0, 0));
        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.y), glm::vec3(0, 1, 0));
        ModelView = glm::rotate(ModelView, glm::radians(Config->Viewports[Index].Rotate.z), glm::vec3(0, 0, 1));

        ColorShader.SetUniform("modelViewMatrix", ModelView);
        if(Config->Viewports[Index].Axis) Axis(Index, Config->Viewports[Index].GridSize);

    }

    MainShader.Bind();
    MainShader.SetUniform("projectionMatrix", Projection);
    MainShader.SetUniform("modelViewMatrix", ModelView);
    MainShader.SetUniform("gSampler", 0);
    MainShader.SetUniform("vColor", glm::vec4(1.0f));
    MainShader.SetUniform("Light.vColor", glm::vec3(0.72f));
    MainShader.SetUniform("Light.fAmbient", 0.65f);
    MainShader.SetUniform("Light.vDirection", -glm::normalize(glm::vec3(0, 25, 0)));

    switch(Config->Viewports[Index].RenderMode) {

    case 0:

        MainShader.SetUniform("bTextured", false);
        MainShader.SetUniform("bLighted", false);
        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;

    case 1:

        MainShader.SetUniform("bTextured", false);
        MainShader.SetUniform("bLighted", true);
        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_FLAT);
        break;

    case 2:

        MainShader.SetUniform("bTextured", false);
        MainShader.SetUniform("bLighted", true);
        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_SMOOTH);
        break;

    case 3:

        MainShader.SetUniform("bTextured", true);
        MainShader.SetUniform("bLighted", true);
        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_SMOOTH);
        break;
    }

    Config->Viewports[Index].Culling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    Config->Viewports[Index].RenderMode == 0 ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);

    if (Config->Viewports[Index].RenderMode == 0) {
        MainShader.SetUniform("vColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    } else {
        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    Model->Render(Index, ColorShader, false);

    if (Config->Viewports[Index].Projection == 0) {

        MainShader.SetUniform("bProjection", 1);

        if(Config->Viewports[Index].Overlay) {
            MainShader.Bind();
            MainShader.SetUniform("bTextured", false);
            MainShader.SetUniform("bLighted", false);
            MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            Model->Render(Index, ColorShader, true);
        }

    } else if(Config->Viewports[Index].Projection != 0) {

        MainShader.Bind();
        MainShader.SetUniform("bProjection", 1);
        MainShader.SetUniform("bTextured", false);
        MainShader.SetUniform("bLighted", false);
        MainShader.SetUniform("vColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Model->Render(Index, ColorShader, true);
    }

    glDisable(GL_SCISSOR_TEST);

    if (Config->Editor.Viewport == Index && glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT)) {
        ModelView = glm::translate(glm::mat4(1.0f), glm::vec3(-Viewports[Index].Width / 2, -Viewports[Index].Height / 2, 0));
        Interface::Selection(Config->Editor.Selection, Index, Projection * ModelView);
    }

}

void CRender::Grid(int Index)
{
    int Count = 0;

    GridVBO.Create();

    if(Config->Viewports[Index].Projection == 0) {
        Config->Viewports[Index].GridScale = int(Config->Viewports[Index].Scale.z / 50.0f) + 1;
        for(int i = -Config->Viewports[Index].GridSize; i <= Config->Viewports[Index].GridSize; i += Config->Viewports[Index].GridScale) {
            GridVBO.AddData(&glm::vec3(i, 0, -Config->Viewports[Index].GridSize)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(i, 0, Config->Viewports[Index].GridSize)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(-Config->Viewports[Index].GridSize, 0, i)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridSize, 0, i)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            Count += 4;
        }
    } else {
        Config->Viewports[Index].GridScale = int(Config->Viewports[Index].Scale.z / 50.0f) + 1;
        for(int i = -Viewports[Index].Width; i < Viewports[Index].Width; i += Config->Viewports[Index].GridScale) {
            GridVBO.AddData(&glm::vec3(i * Config->Viewports[Index].GridScale, Viewports[Index].Height, 0)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(i * Config->Viewports[Index].GridScale, -Viewports[Index].Height, 0)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            Count += 2;
        }
        for(int i = -Viewports[Index].Height; i < Viewports[Index].Height; i += Config->Viewports[Index].GridScale) {
            GridVBO.AddData(&glm::vec3(Viewports[Index].Width, i * Config->Viewports[Index].GridScale, 0)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(-Viewports[Index].Width, i * Config->Viewports[Index].GridScale, 0)[0], sizeof(glm::vec3));
            GridVBO.AddData(&glm::vec3(Config->Viewports[Index].GridColor)[0], sizeof(glm::vec3));
            Count += 2;
        }
    }

    glDeleteVertexArrays(1, &GridVAO);
    glGenVertexArrays(1, &GridVAO);
    glBindVertexArray(GridVAO);

    GridVBO.Bind();
    GridVBO.UploadData(GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glBindVertexArray(0);

    glBindVertexArray(GridVAO);
    glDrawArrays(GL_LINES, 0, Count);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(1.0f);


}

void CRender::Axis(int Index, int Length)
{
    AxisVBO.Create();

    AxisVBO.AddData(&glm::vec3(0, 0, 0)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(1.0f, 0.0f, 0.0f)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(Length, 0, 0)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(1.0f, 0.0f, 0.0f)[0], sizeof(glm::vec3));

    AxisVBO.AddData(&glm::vec3(0, 0, 0)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0.0f, 1.0f, 0.0f)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0, Length, 0)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0.0f, 1.0f, 0.0f)[0], sizeof(glm::vec3));

    AxisVBO.AddData(&glm::vec3(0, 0, 0)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0.0f, 0.0f, 1.0f)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0, 0, -Length)[0], sizeof(glm::vec3));
    AxisVBO.AddData(&glm::vec3(0.0f, 0.0f, 1.0f)[0], sizeof(glm::vec3));

    glDeleteVertexArrays(1, &AxisVAO);
    glGenVertexArrays(1, &AxisVAO);
    glBindVertexArray(AxisVAO);

    AxisVBO.Bind();
    AxisVBO.UploadData(GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glBindVertexArray(0);

    glLineWidth(2.5f);
    glBindVertexArray(AxisVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}
