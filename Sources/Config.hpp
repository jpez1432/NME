
#ifndef CCONFIG_HPP
#define CCONFIG_HPP

#include "Logger.hpp"

#include <windows.h>
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <Glm.hpp>

extern CLogger *Logger;
extern std::string ConfigFile;

class CConfig
{

public:

    struct Rect {
        float X;
        float Y;
        float Width;
        float Height;
    };

    struct {
        bool Help;
        bool Config;
        bool About;
        bool StatusBar;
        bool SideBar;
        bool AutoTool;
        bool SnapToGrid;
        bool RedrawAll;
        bool Maximized;
        bool LoadLast;
        char LastFile[255];
    } General;

    struct {
        int Mode;
        int Tool;
        int Option;
        int Viewport;
        int LastView;
        bool Selected;
        Rect Selection;
        bool IgnoreBackfaces;
        bool SelectByVertex;
        glm::bvec3 ChkRotate;
        bool RotateGlobal;
        int RotateAbout;
        glm::bvec3 ChkMove;
        int MoveAbout;
        glm::bvec3 ChkScale;
        int ScaleAbout;
    } Editor;

    struct Viewport {
        int Projection;
        glm::vec3 Pan;
        glm::vec3 Scale;
        glm::vec3 Rotate;
        bool Overlay;
        bool Grid;
        bool Axis;
        bool Culling;
        bool Transparent;
        int RenderMode;
        int GridScale;
        int GridSize;
        glm::vec4 GridColor;
        glm::vec4 ClearColor;
    };

    struct {
        int Width;
        int Height;
        int Monitor;
        int RefreshRate;
        int Antialiasing;
        bool VSync;
        bool DoubleBuffer;
        bool Multisample;
        int TextureQuality;
    } Graphics;

    Viewport Viewports[4];
    Viewport Default3D;
    Viewport Default2D;


public:

    CConfig(void);
    CConfig(std::string Filename);
    ~CConfig(void);

    bool Load(std::string Filename);
    bool Save(std::string Filename = "");

    void Defaults(std::string Filename = "");
    void SetCamera(glm::vec3 Pan, glm::vec3 Scale, glm::vec3 Rotate, int Projection, int Index);
    void ResetView(int Index);

};

#endif
