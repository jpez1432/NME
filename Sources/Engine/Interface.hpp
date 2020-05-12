
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "Tools.hpp"

#include "../Interface/imgui.h"
#include "../Interface/imgui_impl_glfw.h"
#include "../Interface/imgui_impl_opengl3.h"

namespace Interface {

    bool Create(void);
    void Destroy(void);

    void Render(int Width, int Height);
    void Selection(CConfig::Rect &Rect, int Index, glm::mat4 MVP);
    void Resize(int Maximize = -1, bool Drag = false);

}

#endif
