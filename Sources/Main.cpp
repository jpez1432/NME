

#include "Common.hpp"
#include "Engine/Render.hpp"
#include "Engine/Timer.hpp"
#include "Engine/Model.hpp"

#include <Soil2/Soil2.h>

bool Dirty = false;
bool Loop = true;
std::string ResourceDir;
std::string ConfigFile;
std::string InterfaceFile;
std::string RootDir;

int CurrentVertex = 0;
int CurrentFace = 0;
int CurrentGroup = 0;
int CurrentMaterial = 0;

CModel *Model;
CRender *Render;
CTimer *Timer;
CConfig *Config;
CLogger *Logger;
GLFWwindow *GLWindow;

void ErrorCallback(int Error, const char* Description)
{
    Logger->Error("GLFW3 Error #" + std::to_string(Error) + ", " + std::string(Description));
}

void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
    ImGui_ImplGlfw_KeyCallback(Window, Key, Scancode, Action, Mods);

    if(Key == GLFW_KEY_N && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Model->New();

    } else if(Key == GLFW_KEY_O && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Model->Load(false);
        for (int i = 0; i < 4; i++) {
            Tools::Frame(i, true);
        }

    } else if(Key == GLFW_KEY_M && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Model->Load(true);
        for (int i = 0; i < 4; i++) {
            Tools::Frame(i, true);
        }

    } else if(Key == GLFW_KEY_S && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Model->Save(false);

    } else if(Key == GLFW_KEY_D && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Tools::DuplicateSelection();

    } else if(Key == GLFW_KEY_DELETE && Action == GLFW_PRESS) {

        Tools::DeleteSelection(Config->Editor.Option);

    } else if(Key == GLFW_KEY_DELETE && Mods == GLFW_MOD_SHIFT && Action == GLFW_PRESS) {

        Tools::DeleteAll();

    } else if(Key == GLFW_KEY_A && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Tools::SelectAll();

    } else if(Key == GLFW_KEY_A && Mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT) && Action == GLFW_PRESS) {

        Tools::SelectNone();

    } else if(Key == GLFW_KEY_I && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Tools::SelectInvert();

    } else if(Key == GLFW_KEY_H && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Tools::HideSelection();

    } else if(Key == GLFW_KEY_H && Mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT) && Action == GLFW_PRESS) {

        Tools::HideNone();

    } else if(Key == GLFW_KEY_Z && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Model->Undo();

    } else if(Key == GLFW_KEY_Z && Mods == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT) && Action == GLFW_PRESS) {

        Model->Redo();

    } else if(Key == GLFW_KEY_Q && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        glfwSetWindowShouldClose(GLWindow, GLFW_TRUE);

    } else if(Key == GLFW_KEY_V && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->Graphics.VSync = !Config->Graphics.VSync;
        glfwSwapInterval(Config->Graphics.VSync);

    } else if(Key == GLFW_KEY_ENTER && Mods == GLFW_MOD_ALT && Action == GLFW_PRESS) {

        if(!Config->General.Maximized) {
            glfwMaximizeWindow(GLWindow);
        } else {
            glfwRestoreWindow(GLWindow);
        }
    }

}

void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    static double XPos, YPos;
    glfwGetCursorPos(GLWindow, &XPos, &YPos);

    Config->Editor.Viewport = Tools::FindView(XPos,  YPos);

    if (ImGui::GetIO().WantCaptureMouse == false) {

        if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_PRESS) {

            Render->Start = glm::vec2(XPos, YPos);

            YPos = (Config->Graphics.Height - YPos);
            Config->Editor.Selection.X = XPos - Viewports[Config->Editor.Viewport].X + Viewports[Config->Editor.Viewport].Width / 2.0f;
            Config->Editor.Selection.Y = YPos - Viewports[Config->Editor.Viewport].Y + Viewports[Config->Editor.Viewport].Height / 2.0f;
            Config->Editor.Selection.Width = XPos - Viewports[Config->Editor.Viewport].X + Viewports[Config->Editor.Viewport].Width / 2.0f;
            Config->Editor.Selection.Height = YPos - Viewports[Config->Editor.Viewport].Y + Viewports[Config->Editor.Viewport].Height / 2.0f;

            if (Config->Viewports[Config->Editor.Viewport].Projection != 0 && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) && !glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) && Config->Editor.Selection.Width == Config->Editor.Selection.X && Config->Editor.Selection.Height == Config->Editor.Selection.Y) {

                for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
                    Model->Obj.Groups[j].Selected = false;
                }
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    Model->Obj.Triangles[j].Selected = false;
                }
                for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
                    Model->Obj.Vertices[j].Selected = false;
                }

                Config->Editor.Selected = false;
            }

            Tools::Select(Config->Editor.Viewport, true);

        } else if (Button == GLFW_MOUSE_BUTTON_LEFT && Action == GLFW_RELEASE) {

//            Tools::Select(Config->Editor.Viewport);

        }
    }

    if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_PRESS) {
        Config->Editor.LastView = Tools::FindView(XPos,  YPos);
    } else if (Button == GLFW_MOUSE_BUTTON_RIGHT && Action == GLFW_RELEASE) {
        Config->Editor.LastView = Config->Editor.Viewport;
    }


}

void MouseMoveCallback(GLFWwindow* Window, double XPos, double YPos)
{
    if (Config->Editor.Viewport == -1 && glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_1)) {
        Interface::Resize(-1, true);
        for (unsigned int i = 0; i < 4; i++) {
            Tools::Frame(i, true);
        }
        return;
    }

    if (glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT)) {

        Render->End = Render->Start;
        Render->Start = glm::vec2(XPos, YPos);

        if (ImGui::GetIO().WantCaptureMouse == false) {
            Render->Update(Config->Editor.Viewport);
        }

        Config->Editor.Selection.Width = XPos - Viewports[Config->Editor.Viewport].X + Viewports[Config->Editor.Viewport].Width / 2.0f;
        Config->Editor.Selection.Height = (Config->Graphics.Height - YPos) - Viewports[Config->Editor.Viewport].Y + Viewports[Config->Editor.Viewport].Height / 2.0f;

//        Tools::Select(Config->Editor.Viewport, false);
    }

    int TempViewport = Tools::FindView(XPos, YPos);

    if (TempViewport < 0) {
        Cursor = glm::vec3(0.0f);
        return;
    }

    YPos = (Config->Graphics.Height - YPos);
    XPos -= Viewports[TempViewport].X + Viewports[TempViewport].Width / 2.0f;
    YPos -= Viewports[TempViewport].Y + Viewports[TempViewport].Height / 2.0f;

    switch (Config->Viewports[TempViewport].Projection) {
        case 0:
            Cursor = glm::vec3(0.0f);
            break;

        case 1:
            Cursor.x = (XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x;
            Cursor.y = ((YPos / -Config->Viewports[TempViewport].Scale.y) + Config->Viewports[TempViewport].Pan.y) * -1.0f;
            Cursor.z = 0.0f;
            break;

        case 2:

            Cursor.x = ((XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x) * -1.0f;
            Cursor.y = ((YPos / -Config->Viewports[TempViewport].Scale.y) + Config->Viewports[TempViewport].Pan.y) * -1.0f;
            Cursor.z = 0.0f;
            break;

        case 3:
            Cursor.x = 0.0f;
            Cursor.y = ((YPos / -Config->Viewports[TempViewport].Scale.y) + Config->Viewports[TempViewport].Pan.y) * -1.0f;
            Cursor.z = ((XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x);
            break;

        case 4:
            Cursor.x = 0.0f;
            Cursor.y = ((YPos / -Config->Viewports[TempViewport].Scale.y) - Config->Viewports[TempViewport].Pan.y) * -1.0f;
            Cursor.z = ((XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x) * -1.0f;
            break;

        case 5:
            Cursor.x = ((XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x) * -1.0f;
            Cursor.y = 0.0f;
            Cursor.z = ((YPos / -Config->Viewports[TempViewport].Scale.y) + Config->Viewports[TempViewport].Pan.y) * -1.0f;
            break;

        case 6:
            Cursor.x = ((XPos / -Config->Viewports[TempViewport].Scale.x) + Config->Viewports[TempViewport].Pan.x) * -1.0f;
            Cursor.y = 0.0f;
            Cursor.z = ((YPos / -Config->Viewports[TempViewport].Scale.y) + Config->Viewports[TempViewport].Pan.y);
            break;
    }

    if (Config->General.SnapToGrid) {
        Cursor = glm::vec3(int(Cursor.x + 0.5f), int(Cursor.y + 0.5f), int(Cursor.z + 0.5f));
    }

}

void MouseScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    static double XPos, YPos;
    glfwGetCursorPos(GLWindow, &XPos, &YPos);

    Config->Editor.Viewport = Tools::FindView(XPos,  YPos);

    if(Config->Viewports[Config->Editor.Viewport].Projection == 0) {
        if(glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            Config->Viewports[Config->Editor.Viewport].Scale += YOffset * 0.5f;
        } else {
            Config->Viewports[Config->Editor.Viewport].Scale += YOffset * 0.01f;
        }
        Config->Viewports[Config->Editor.Viewport].Scale = glm::clamp(Config->Viewports[Config->Editor.Viewport].Scale, glm::vec3(0.1f), glm::vec3(1000.0f));
    } else {
        if(glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            Config->Viewports[Config->Editor.Viewport].Scale += YOffset * 5.0f;
        } else {
            Config->Viewports[Config->Editor.Viewport].Scale += YOffset * 0.5f;
        }
        Config->Viewports[Config->Editor.Viewport].Scale = glm::clamp(Config->Viewports[Config->Editor.Viewport].Scale, glm::vec3(0.5f), glm::vec3(1000.0f));
    }

    Render->Update(Config->Editor.Viewport);
}

void ResizeCallback(GLFWwindow* Window, int Width, int Height)
{
    glfwGetFramebufferSize(GLWindow, &Config->Graphics.Width, &Config->Graphics.Height);
    Interface::Resize();

    for (unsigned int i = 0; i < 4; i++) {
        Tools::Frame(i, true);
    }
}

void MaximizeCallback(GLFWwindow* Window)
{
    Config->General.Maximized = !Config->General.Maximized;
    glfwGetFramebufferSize(GLWindow, &Config->Graphics.Width, &Config->Graphics.Height);
    Interface::Resize();
}

bool Create(std::string ResDir, std::string CfgFile, std::string AppRoot)
{
    RootDir = AppRoot.substr(0, AppRoot.find_last_of("\\") + 1);
    ResourceDir = ResDir;
    InterfaceFile = RootDir + ResourceDir + "Interface.ini";
    ConfigFile = RootDir + ResourceDir + CfgFile;

    Logger = new CLogger(RootDir + ResourceDir + "App.log");
    Config = new CConfig();
    Config->Load(ConfigFile);

    Logger->Text(GetInfo());
    Logger->Text("Resource Directory - " + ResDir);
    Logger->Text("Configuration Filename - " + CfgFile);
    Logger->Text("Interface Filename - Interface.ini");
    Logger->Text("Root Directory - " + RootDir);

    glfwSetErrorCallback(ErrorCallback);

    if(glfwInit()) {
        Logger->Text("Initialized GLFW3 Library");
    } else {
        Logger->Error("Error Initializing GLFW3 Library");
    }

    glfwWindowHint(GLFW_SAMPLES, Config->Graphics.Antialiasing);
    glfwWindowHint(GLFW_DOUBLEBUFFER, Config->Graphics.DoubleBuffer);
    glfwWindowHint(GLFW_REFRESH_RATE, Config->Graphics.RefreshRate);
//    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    int Cnt, PosX, PosY;
    GLFWmonitor** Monitors = glfwGetMonitors(&Cnt);

    if(Config->Graphics.Monitor > Cnt) {
        Logger->Error("Invalid Monitor Selection, Defaulting To Primary...", true, true, false);
        Config->Graphics.Monitor = 0;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    GLWindow = glfwCreateWindow(Config->Graphics.Width, Config->Graphics.Height, GetInfo().c_str(), NULL, NULL);

    if(GLWindow != NULL) {
        Logger->Text("Created GLFW3 Window");
    } else {
        Logger->Error("Error Creating GLFW3 Window");
    }

    if (Config->General.Maximized) {
        glfwMaximizeWindow(GLWindow);
    } else {
        const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor)]);
        glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor)], &PosX, &PosY);
        glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
    }

    GLFWimage Icon;
    std::string IconFile = RootDir + ResDir + "\\General\\Icon.png";
    Icon.pixels = SOIL_load_image(IconFile.c_str(), &Icon.width, &Icon.height, 0, SOIL_LOAD_RGBA);
    glfwSetWindowIcon(GLWindow, 1, &Icon);
    SOIL_free_image_data(Icon.pixels);

    glfwMakeContextCurrent(GLWindow);
    glfwSwapInterval(Config->Graphics.VSync);

    if(Config->Graphics.Multisample) {
        glEnable(GL_MULTISAMPLE);
    }

    if(GLEW_OK == glewInit()) {
        Logger->Text("Initialized GLEW");
    } else {
        Logger->Error("Error Initializing GLEW");
    }

    Logger->Text("GLEW - v" + std::string((char*)glewGetString(GLEW_VERSION)));
    Logger->Text("GLFW3 - v" + std::string((char*)glfwGetVersionString()));
    Logger->Text("OpenGL Renderer - " + std::string((char*)glGetString(GL_RENDERER)));
    Logger->Text("OpenGL Vendor - " + std::string((char*)glGetString(GL_VENDOR)));
    Logger->Text("OpenGL Version - v" + std::string((char*)glGetString(GL_VERSION)));

    Model = new CModel();
    if (Config->General.LoadLast) {
        if (Model->Load(false, Config->General.LastFile)) {
            Logger->Text("Loaded Model File - " + std::string(Config->General.LastFile));
        } else {
            Logger->Error("Error Loading Model File - " + std::string(Config->General.LastFile), true, false, false);
            Model->New();
        }
    }

    Render = new CRender();
    if(Render) {
        Logger->Text("Created Rendering Engine");
    } else {
        Logger->Error("Error Creating Rendering Engine");
    }

    for (int i = 0; i < 4; i++) {
        Tools::Frame(i, true);
    }

    Timer = new CTimer();
    if(Timer) {
        Logger->Text("Created Timer");
    } else {
        Logger->Error("Error Creating Timer");
    }


    glfwSetWindowMaximizeCallback(GLWindow, (GLFWwindowmaximizefun)MaximizeCallback);
    glfwSetKeyCallback(GLWindow, (GLFWkeyfun)KeyCallback);
    glfwSetWindowSizeCallback(GLWindow, (GLFWwindowsizefun)ResizeCallback);
    glfwSetScrollCallback(GLWindow, (GLFWscrollfun)MouseScrollCallback);
    glfwSetMouseButtonCallback(GLWindow, (GLFWmousebuttonfun)MouseButtonCallback);
    glfwSetCursorPosCallback(GLWindow, (GLFWcursorposfun)MouseMoveCallback);

    glfwSetCursor(GLWindow, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));

    Logger->Text("NME Created");

    return true;

}

bool Destroy(void)
{
    if(Config) {
        Config->Save(ConfigFile);
        delete Config;
        Config = NULL;
        Logger->Text("Destroyed Configuration Object");
    }

    if (Render) {
        delete Render;
        Render = NULL;
        Logger->Text("Destroyed Rendering Engine");
    }

    if(GLWindow) {
        glfwTerminate();
        Logger->Text("Destroyed OpenGL Framework 3 Window");
    }

    Logger->Text("NME Destroyed");

    if(Logger) {
        delete Logger;
        Logger = NULL;
    }

    return true;

}

int main(int argc, char* argv[])
{
    while(Loop) {

        Loop = Create("Resources\\", "Config.bin", argv[0]);

        do {

            Timer->Update();
            Render->Scene();
            glfwPollEvents();

        } while(Loop && !glfwWindowShouldClose(GLWindow));

        Destroy();
        Loop = !Loop;

    };
    return EXIT_SUCCESS;
}
