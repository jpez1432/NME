
#include "Config.hpp"

CConfig::CConfig(void)
{
    Default3D.Projection = 0;
    Default3D.Pan = glm::vec3(0, -5, -25);
    Default3D.Scale = glm::vec3(1);
    Default3D.Rotate = glm::vec3(0, -135, 0);
    Default3D.Grid = true;
    Default3D.Axis = true;
    Default3D.RenderMode = 3;
    Default3D.Culling = true;
    Default3D.Overlay = false;
    Default3D.Transparent = true;
    Default3D.GridScale = 1;
    Default3D.GridSize = 25;
    Default3D.GridColor = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
    Default3D.ClearColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);

    Default2D.Projection = 1;
    Default2D.Pan = glm::vec3(0, 0, 0);
    Default2D.Scale = glm::vec3(25);
    Default2D.Rotate = glm::vec3(0, 0, 0);
    Default2D.Grid = true;
    Default2D.Axis = true;
    Default2D.RenderMode = 0;
    Default2D.Culling = true;
    Default2D.Overlay = false;
    Default2D.Transparent = false;
    Default2D.GridScale = 5;
    Default2D.GridSize = 100;
    Default2D.GridColor = glm::vec4(0.85f, 0.85f, 0.85f, 1.0f);
    Default2D.ClearColor = glm::vec4(0.65f, 0.65f, 0.65f, 1.0f);

    Editor.LastView = 0;
    Editor.Viewport = 0;

}

CConfig::CConfig(std::string Filename)
{
    Load(Filename);
}

CConfig::~CConfig(void)
{
}

void CConfig::Defaults(std::string Filename)
{

    if(Filename == "") {
        Filename = ConfigFile;
    }

    General.Config = false;
    General.About = false;
    General.StatusBar = true;
    General.SideBar = true;
    General.AutoTool = true;
    General.SnapToGrid = false;
    General.RedrawAll = false;
    General.Maximized = false;
    General.LoadLast = true;
    ZeroMemory(General.LastFile, 255);
//    sprintf(General.LastFile, "%s", "Untitled.obj");

    Editor.Selected = false;
    ZeroMemory(&Editor.Selection, sizeof(Rect));
    Editor.Tool = 0;
    Editor.Option = 0;
    Editor.Viewport = 0;
    Editor.LastView = 0;
    Editor.IgnoreBackfaces = false;
    Editor.SelectByVertex = false;
    Editor.ChkRotate = glm::bvec3(1);
    Editor.RotateGlobal = true;
    Editor.RotateAbout = 0;
    Editor.ChkMove = glm::bvec3(1);
    Editor.MoveAbout = 0;
    Editor.ChkScale = glm::bvec3(1);
    Editor.ScaleAbout = 0;

    Viewports[0] = Default2D;
    Viewports[0].Projection = 3;
    Viewports[0].Rotate = glm::vec3(0, 90, 0);

    Viewports[1] = Default3D;

    Viewports[2] = Default2D;
    Viewports[2].Projection = 5;
    Viewports[2].Rotate = glm::vec3(90.0f, 0.0f, 0.0f);

    Viewports[3] = Default2D;
    Viewports[3].Projection = 1;
    Viewports[3].Rotate = glm::vec3(0.0f, 180.0f, 0.0f);

    Graphics.Width = 900;
    Graphics.Height = 600;
    Graphics.Monitor = 0;
    Graphics.RefreshRate = 60;
    Graphics.Antialiasing = 2;
    Graphics.VSync = true;
    Graphics.DoubleBuffer = true;
    Graphics.Multisample = true;
    Graphics.TextureQuality = 2;

    Logger->Text("Loaded Default Configuration...");

//    Save(Filename);

}

bool CConfig::Load(std::string Filename)
{

//    Defaults(Filename);
//    return true;

    std::ifstream CfgFile(Filename.c_str(), std::ios::in | std::ios::binary);

    if(CfgFile.is_open()) {

        CfgFile.seekg(0, std::ios::end);

        if (CfgFile.tellg() != sizeof(General) + sizeof(Editor) + sizeof(Graphics) + sizeof(Viewports)) {
            Logger->Error("Configuration File Is Corrupt Or Missing, Generating Defaults...", true, true, false);
            Defaults(Filename);
            return true;
        }

        CfgFile.seekg(0, std::ios::beg);

        CfgFile.read((char*) &General, sizeof(General));
        CfgFile.read((char*) &Editor, sizeof(Editor));
        CfgFile.read((char*) &Graphics, sizeof(Graphics));
        CfgFile.read((char*) &Viewports[0], sizeof(Viewport) * 4);

        CfgFile.close();
        Logger->Text("Loaded Configuration");

    } else {
        Logger->Error("Error Loading Configuration, Generating Defaults...", true, true, false);
        Defaults(Filename);
    }

    return true;
}

bool CConfig::Save(std::string Filename)
{
    std::ofstream CfgFile(Filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if(CfgFile.is_open()) {

        CfgFile.write((char*) &General, sizeof(General));
        CfgFile.write((char*) &Editor, sizeof(Editor));
        CfgFile.write((char*) &Graphics, sizeof(Graphics));
        CfgFile.write((char*) &Viewports[0], sizeof(Viewport) * 4);

        CfgFile.close();
        Logger->Text("Saved Configuration");

    } else {
        Logger->Error("Error Saving Configuration, Aborting...");
    }

    return true;
}

void CConfig::ResetView(int Index)
{
    switch(Index) {
        case 0:
            Viewports[0] = Default2D;
            Viewports[0].Projection = 3;
            Viewports[0].Rotate = glm::vec3(0, 90, 0);

            break;

        case 1:
            Viewports[1] = Default3D;
            break;

        case 2:
            Viewports[2] = Default2D;
            Viewports[2].Projection = 5;
            Viewports[2].Rotate = glm::vec3(90, 0, 0);
            break;

        case 3:
            Viewports[3] = Default2D;
            Viewports[3].Projection = 1;
            Viewports[3].Rotate = glm::vec3(0, 180, 0);
            break;
    }
}

void CConfig::SetCamera(glm::vec3 Pan, glm::vec3 Scale, glm::vec3 Rotate, int Projection, int Index)
{
    if(Projection == 0) {
        Viewports[Index].Pan = glm::vec3(0, -5, -25);
        Viewports[Index].Scale = glm::vec3(10.0f);
        Viewports[Index].Rotate = glm::vec3(0.0f, 0.0f, 0.0f);
        Viewports[Index].GridColor = glm::vec4(0.88f, 0.88f, 0.88f, 1.0f);
        Viewports[Index].ClearColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
    } else {
        Viewports[Index].GridColor = glm::vec4(0.88f, 0.88f, 0.88f, 1.0f);
        Viewports[Index].ClearColor = glm::vec4(0.65f, 0.65f, 0.65f, 1.0f);
    }

    Viewports[Index].Projection = Projection;
//    Viewports[Index].Pan = Pan;
//    Viewports[Index].Scale = Scale;
    Viewports[Index].Rotate = Rotate;

}
