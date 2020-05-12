
#include "Interface.hpp"

#include "Model.hpp"

extern CModel *Model;

glm::vec3 Cursor;
CConfig::Rect Viewports[4];
GLuint GLIcon;
CProgram SelectionShader;

bool Interface::Create(void)
{
    if (SelectionShader.Create("Select.frag", "Select.vert", NULL)) {
        Logger->Text("Created Selection Shader Program");
    } else {
        Logger->Error("Error Creating Selection Shader Program");
    }

    GLIcon = SOIL_load_OGL_texture(std::string(RootDir + ResourceDir + "General\\Icon.png").c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);

    glfwGetFramebufferSize(GLWindow, &Config->Graphics.Width, &Config->Graphics.Height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    io.WantCaptureMouse = true;
    io.MouseDrawCursor = false;
    io.IniFilename = &InterfaceFile.c_str()[0];
    ImGui_ImplGlfw_InitForOpenGL(GLWindow, true);
    ImGui_ImplOpenGL3_Init(NULL);
    ImGuiStyle& Style = ImGui::GetStyle();

    Style.WindowRounding = 1.85f;
    Style.FrameRounding = 1.85f;
    Style.ScrollbarRounding = 1.85f;
    Style.WindowBorderSize = 1.5f;
    Style.FrameBorderSize = 1.5f;

    Style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    Style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.85f);
    Style.Colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    Style.Colors[ImGuiCol_Border]                 = ImVec4(0.16f, 0.16f, 0.16f, 0.85f);
    Style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
    Style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.29f, 0.30f, 0.31f, 0.40f);
    Style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.49f, 0.46f, 0.46f, 0.67f);
    Style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    Style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.40f, 0.40f, 0.40f, 0.85f);
    Style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    Style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    Style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.87f, 0.88f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.90f, 0.95f, 0.95f, 1.00f);
    Style.Colors[ImGuiCol_Button]                 = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    Style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.84f, 0.84f, 0.84f, 1.00f);
    Style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Header]                 = ImVec4(0.52f, 0.52f, 0.52f, 0.31f);
    Style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.80f, 0.80f, 0.80f);
    Style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    Style.Colors[ImGuiCol_Separator]              = ImVec4(0.83f, 0.83f, 0.83f, 0.50f);
    Style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
    Style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    Style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    Style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.49f, 0.49f, 0.49f, 0.67f);
    Style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.50f, 0.50f, 0.50f, 0.95f);
    Style.Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.26f, 0.26f, 0.48f);
    Style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.57f, 0.57f, 0.57f, 0.90f);
    Style.Colors[ImGuiCol_NavHighlight]           = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
    Style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    Style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    Cursor = glm::vec3(0.0f);

    Resize();


    return true;

}

void Interface::Destroy(void)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    Logger->Text("Released imGui Interface");
}



void Interface::Selection(CConfig::Rect &Rect, int Index, glm::mat4 MVP)
{
    glLineWidth(2.0f);

    SelectionShader.Bind();
    SelectionShader.SetUniform("u_resolution", glm::vec2(Config->Graphics.Width, Config->Graphics.Height));
    SelectionShader.SetUniform("u_dashSize", 25.0f);
    SelectionShader.SetUniform("u_gapSize", 15.0f);
    SelectionShader.SetUniform("vColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    SelectionShader.SetUniform("u_mvp", MVP);

    std::vector<glm::vec2> vArray {glm::vec2(Rect.X, Rect.Y), glm::vec2(Rect.X, Rect.Height), glm::vec2(Rect.Width, Rect.Height), glm::vec2(Rect.Width, Rect.Y)};
    std::vector<unsigned char> iArray {0, 1, 1, 2, 2, 3, 3, 0};

    GLuint Buffer[2], VAO;
    glGenBuffers(2, Buffer);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, Buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, vArray.size() * sizeof(*vArray.data()), vArray.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iArray.size() * sizeof(*iArray.data()), iArray.data(), GL_STATIC_DRAW);

    glDrawElements(GL_LINES, (GLsizei)iArray.size(), GL_UNSIGNED_BYTE, nullptr);

    glDeleteBuffers(2, &Buffer[0]);
    glDeleteVertexArrays(1, &VAO);

    glLineWidth(1.0f);

}

void Interface::Render(int Width, int Height)
{
    static bool Maximize;

    glAlphaFunc(GL_GEQUAL, 0.25f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("New", "Ctrl+N")) {
                Model->New();
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                Model->Load(false);
                for (int i = 0; i < 4; i++) {
                    Tools::Frame(i, true);
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Merge", "Ctrl+M")) {
                Model->Load(true);
                for (int i = 0; i < 4; i++) {
                    Tools::Frame(i, true);
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Load Last File", "", Config->General.LoadLast)) {
                Config->General.LoadLast = !Config->General.LoadLast;
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                Model->Save(false);
            }

            if (ImGui::MenuItem("Save As...")) {
                Model->Save(true);
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Configuration...", "Shift+C", Config->General.Config)) {
                Config->General.Config = !Config->General.Config;
                if (Config->General.Config) {
                    Config->General.About = false;
                }
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
                glfwSetWindowShouldClose(GLWindow, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, Model->Undos.size() > 0 ? true : false)) {
                Model->Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, Model->Redos.size() > 0 ? true : false)) {
                Model->Redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "Ctrl+X", false, Config->Editor.Selected)) {

            }
            if (ImGui::MenuItem("Copy", "Ctrl+C", false, Config->Editor.Selected)) {

            }
            if (ImGui::MenuItem("Paste", "Ctrl+V", false, Config->Editor.Selected)) {

            }

            ImGui::Separator();
            if (ImGui::MenuItem("Duplicate Selection", "Ctrl+D", false, Config->Editor.Selected)) {
                Tools::DuplicateSelection();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Delete Selection", "Delete", false, Config->Editor.Selected)) {
                Tools::DeleteSelection(Config->Editor.Option);
            }
            if (ImGui::MenuItem("Delete All", "Shift+Delete")) {
                Tools::DeleteAll();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {
                Tools::SelectAll();
            }

            if (ImGui::MenuItem("Select None", "Ctrl+Shift+A")) {
                Tools::SelectNone();
            }

            if (ImGui::MenuItem("Select Invert", "Ctrl+I")) {
                Tools::SelectInvert();
            }

            ImGui::Separator();
            if (ImGui::MenuItem("Hide Selection", "Ctrl+H")) {
                Tools::HideSelection();
            }

            if (ImGui::MenuItem("Unhide All", "Ctrl+Shift+H")) {
                Tools::HideNone();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Side Bar", "Shift+B", Config->General.SideBar)) {
                Config->General.SideBar = !Config->General.SideBar;
                Interface::Resize();
            }

            if (ImGui::MenuItem("Status Bar", "Shift+I", Config->General.StatusBar)) {
                Config->General.StatusBar = !Config->General.StatusBar;
                Interface::Resize();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Insert"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Model"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Vertex"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Face"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Textures"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Groups"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("ReadMe.txt", "F1")) {

            }
            if (ImGui::MenuItem("About NME...", "", Config->General.About)) {
                Config->General.About = !Config->General.About;
                if (Config->General.About) {
                    Config->General.Config = false;
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (Config->General.StatusBar) {

        ImGui::Begin("Status Bar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if (Cursor != glm::vec3(0.0f)) {
            ImGui::Text("Position (%0.3f, %0.3f, %0.3f)", Cursor.x, Cursor.y, Cursor.z);
        } else {
            ImGui::Text(GetInfo().c_str());
        }

        ImGui::SetWindowSize(ImVec2(Width, 24.0f));
        ImGui::SetWindowPos(ImVec2(0.0f, Height - 24.0f));

        ImGui::End();
    }

    if (Config->General.About) {

        ImGui::Begin("About...", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::GetIO().WantCaptureMouse = true;

        ImGui::SetWindowSize(ImVec2(300, 186), 0);
        ImGui::SetWindowPos(ImVec2(200 + float((Width - 200) / 2 - 150), (float(Height / 2) - 78)));

        ImGui::Image((void*)GLIcon, ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
        ImGui::SameLine();
        ImGui::TextWrapped("%s %swww.Natas8.com/NME %sJpez1432@gmail.com", GetInfo().c_str(), "\n", "\n");
        ImGui::Separator();
        ImGui::TextWrapped("NME was created for simplicity, and ease of use. NME uses OpenGL for accelerated graphics processing. It aims to be easy to use for all aspects of model editing. All questions, comments and bug reports are welcome.");
        ImGui::Separator();
        ImGui::Text("                               ");
        ImGui::SameLine();

        if (ImGui::Button("Okay", ImVec2(60, 18))) {
            Config->General.About = false;
            ImGui::GetIO().WantCaptureMouse = false;
        }

        ImGui::End();
    }

    if(Config->General.Config) {

        ImGui::Begin("Configuration", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::GetIO().WantCaptureMouse = true;

        ImGui::SetWindowSize(ImVec2(380, 110), 0);
        ImGui::SetWindowPos(ImVec2(200 + float((Width - 200) / 2 - 190), (float (Height / 2) - 55)));

        ImGui::PushItemWidth(360);

        ImGui::Separator();

        if (ImGui::Button("Cancel", ImVec2(56, 18))) {
            Config->General.Config = false;
            ImGui::GetIO().WantCaptureMouse = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(56, 18))) {
            Config->General.Config = false;
            ImGui::GetIO().WantCaptureMouse = false;
            Config->Save(ConfigFile);
        }

        ImGui::End();

    }

    if (Config->General.SideBar) {

        ImGui::Begin("Side Bar", NULL, (Config->General.About || Config->General.Config ? ImGuiWindowFlags_NoInputs : 0) | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        if (Config->General.StatusBar) {
            ImGui::SetWindowSize(ImVec2(196, Height - 48));
        } else {
            ImGui::SetWindowSize(ImVec2(196, Height - 26));
        }
        ImGui::SetWindowPos(ImVec2(4, 22));

        static char MoveX[16] = {"0.000"};
        static char MoveY[16] = {"0.000"};
        static char MoveZ[16] = {"0.000"};

        static char ScaleX[16] = {"1.000"};
        static char ScaleY[16] = {"1.000"};
        static char ScaleZ[16] = {"1.000"};

        static char RotateX[16] = {"0.000"};
        static char RotateY[16] = {"0.000"};
        static char RotateZ[16] = {"0.000"};

        if (ImGui::CollapsingHeader("Modeling", ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Modeling"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Materials"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Groups"), 0);

            ImGui::Separator();
            ImGui::Checkbox("Snap Grid", &Config->General.SnapToGrid);
            ImGui::SameLine();
            ImGui::Checkbox("Auto-Tool", &Config->General.AutoTool);
//            ImGui::Checkbox("Redraw All Viewports", &Config->General.RedrawAll);
//            ImGui::Separator();

//            ImGui::Text("Primitives");

            if (ImGui::CollapsingHeader("Primitives", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (Config->Editor.Mode == 0) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Vertices", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 0;
                } else {
                    if (ImGui::Button("Vertices", ImVec2(87, 18))) {
                        Config->Editor.Mode = 0;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Mode == 1) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Triangle", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 1;
                } else {
                    if (ImGui::Button("Triangle", ImVec2(87, 18))) {
                        Config->Editor.Mode = 1;
                    }
                }

                if (Config->Editor.Mode == 2) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Cube", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 2;
                } else {
                    if (ImGui::Button("Cube", ImVec2(87, 18))) {
                        Config->Editor.Mode = 2;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Mode == 3) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Cylinder", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 3;
                } else {
                    if (ImGui::Button("Cylinder", ImVec2(87, 18))) {
                        Config->Editor.Mode = 3;
                    }
                }

                if (Config->Editor.Mode == 4) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Sphere", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 4;
                } else {
                    if (ImGui::Button("Sphere", ImVec2(87, 18))) {
                        Config->Editor.Mode = 4;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Mode == 5) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Plane", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 5;
                } else {
                    if (ImGui::Button("Plane", ImVec2(87, 18))) {
                        Config->Editor.Mode = 5;
                    }
                }

                if (Config->Editor.Mode == 6) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Cone", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 6;
                } else {
                    if (ImGui::Button("Cone", ImVec2(87, 18))) {
                        Config->Editor.Mode = 6;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Mode == 7) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Torus", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Mode = 7;
                } else {
                    if (ImGui::Button("Torus", ImVec2(87, 18))) {
                        Config->Editor.Mode = 7;
                    }
                }
            }

            ImGui::Separator();
            ImGui::Text("Tools");
            if (Config->Editor.Tool == 0) {
                ImGui::PushID(1);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::Button("Select", ImVec2(87, 18));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
                Config->Editor.Tool = 0;
            } else {
                if (ImGui::Button("Select", ImVec2(87, 18))) {
                    Config->Editor.Tool = 0;
                }
            }

            ImGui::SameLine();
            if (Config->Editor.Tool == 1) {
                ImGui::PushID(1);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::Button("Rotate", ImVec2(87, 18));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
                Config->Editor.Tool = 1;
            } else {
                if (ImGui::Button("Rotate", ImVec2(87, 18))) {
                    Config->Editor.Tool = 1;
                }
            }

            if (Config->Editor.Tool == 2) {
                ImGui::PushID(1);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::Button("Scale", ImVec2(87, 18));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
                Config->Editor.Tool = 2;
            } else {
                if (ImGui::Button("Scale", ImVec2(87, 18))) {
                    Config->Editor.Tool = 2;
                }
            }

            ImGui::SameLine();
            if (Config->Editor.Tool == 3) {
                ImGui::PushID(1);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                ImGui::Button("Move", ImVec2(87, 18));
                ImGui::PopStyleColor(3);
                ImGui::PopID();
                Config->Editor.Tool = 3;
            } else {
                if (ImGui::Button("Move", ImVec2(87, 18))) {
                    Config->Editor.Tool = 3;
                }
            }

            ImGui::Separator();
            ImGui::Text("Options");

            if (Config->Editor.Tool == 0) {
                if (Config->Editor.Option == 0) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Vertex", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Option = 0;
                } else {
                    if (ImGui::Button("Vertex", ImVec2(55, 18))) {
                        Config->Editor.Option = 0;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Option == 1) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Face", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Option = 1;
                } else {
                    if (ImGui::Button("Face", ImVec2(55, 18))) {
                        Config->Editor.Option = 1;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.Option == 2) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Group", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.Option = 2;
                } else {
                    if (ImGui::Button("Group", ImVec2(55, 18))) {
                        Config->Editor.Option = 2;
                    }
                }

                if (Config->Editor.Option < 2) {
                    ImGui::Checkbox("Ignore Backfaces", &Config->Editor.IgnoreBackfaces);
                }
                if (Config->Editor.Option == 1) {
                    ImGui::Checkbox("Faces By Vertices", &Config->Editor.SelectByVertex);
                }

            } else if (Config->Editor.Tool == 1) {


                if (ImGui::Button("Rotate", ImVec2(182, 18))) {

                }

                ImGui::PushItemWidth(85);

                ImGui::InputText("X", &RotateX[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkRotate.x) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Rotate X", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.x = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Rotate X", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.x = true;
                    }
                }

                ImGui::InputText("Y", &RotateY[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkRotate.y) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Rotate Y", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.y = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Rotate Y", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.y = true;
                    }
                }

                ImGui::InputText("Z", &RotateZ[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkRotate.z) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Rotate Z", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.z = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Rotate Z", ImVec2(78, 18))) {
                        Config->Editor.ChkRotate.z = true;
                    }
                }

                if (Config->Editor.RotateAbout == 0) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Mass", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.RotateAbout = 0;
                } else {
                    if (ImGui::Button("Mass", ImVec2(55, 18))) {
                        Config->Editor.RotateAbout = 0;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.RotateAbout == 1) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Origin", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.RotateAbout = 1;
                } else {
                    if (ImGui::Button("Origin", ImVec2(55, 18))) {
                        Config->Editor.RotateAbout = 1;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.RotateAbout == 2) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Point", ImVec2(55, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.RotateAbout = 2;
                } else {
                    if (ImGui::Button("Point", ImVec2(55, 18))) {
                        Config->Editor.RotateAbout = 2;
                    }
                }

                if (Config->Editor.RotateGlobal) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Global", ImVec2(87, 18))) {
                        Config->Editor.RotateGlobal = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Global", ImVec2(87, 18))) {
                        Config->Editor.RotateGlobal = true;
                    }
                }

                ImGui::SameLine();
                ImGui::SameLine();
                if (!Config->Editor.RotateGlobal) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Local", ImVec2(87, 18))) {
                        Config->Editor.RotateGlobal = true;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Local", ImVec2(87, 18))) {
                        Config->Editor.RotateGlobal = false;
                    }
                }


            } else if (Config->Editor.Tool == 2) {

                if (ImGui::Button("Scale", ImVec2(182, 18))) {
                    Tools::Scale(glm::vec3(atof(ScaleX), atof(ScaleY), atof(ScaleZ)), Config->Editor.ChkScale, Config->Editor.ScaleAbout, false);
                }

                if (ImGui::Button("Scale Into", ImVec2(182, 18))) {
                    Tools::Scale(glm::vec3(atof(ScaleX), atof(ScaleY), atof(ScaleZ)), Config->Editor.ChkScale, Config->Editor.ScaleAbout, true);
                }

                ImGui::PushItemWidth(85);

                ImGui::InputText("X", &ScaleX[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkScale.x) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Scale X", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.x = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Scale X", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.x = true;
                    }
                }

                ImGui::InputText("Y", &ScaleY[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkScale.y) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Scale Y", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.y = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Scale Y", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.y = true;
                    }
                }

                ImGui::InputText("Z", &ScaleZ[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkScale.z) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Scale Z", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.z = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Scale Z", ImVec2(78, 18))) {
                        Config->Editor.ChkScale.z = true;
                    }
                }

                if (Config->Editor.ScaleAbout == 0) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Mass", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.ScaleAbout = 0;
                } else {
                    if (ImGui::Button("Mass", ImVec2(87, 18))) {
                        Config->Editor.ScaleAbout = 0;
                    }
                }

                ImGui::SameLine();
                if (Config->Editor.ScaleAbout == 1) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::Button("Origin", ImVec2(87, 18));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    Config->Editor.ScaleAbout = 1;
                } else {
                    if (ImGui::Button("Origin", ImVec2(87, 18))) {
                        Config->Editor.ScaleAbout = 1;
                    }
                }



            } else if (Config->Editor.Tool == 3) {

                if (ImGui::Button("Move", ImVec2(182, 18))) {
                    Tools::Move(glm::vec3(atof(MoveX), atof(MoveY), atof(MoveZ)), Config->Editor.ChkMove, Config->Editor.MoveAbout);
                }

                ImGui::PushItemWidth(85);

                ImGui::InputText("X", &MoveX[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkMove.x) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Move X", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.x = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Move X", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.x = true;
                    }
                }

                ImGui::InputText("Y", &MoveY[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkMove.y) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Move Y", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.y = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Move Y", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.y = true;
                    }
                }

                ImGui::InputText("Z", &MoveZ[0], 16, ImGuiInputTextFlags_CharsDecimal);
                ImGui::SameLine();
                if (Config->Editor.ChkMove.z) {
                    ImGui::PushID(1);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.52f, 0.52f, 0.52f, 1.0f));
                    if (ImGui::Button("Move Z", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.z = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                } else {
                    if (ImGui::Button("Move Z", ImVec2(78, 18))) {
                        Config->Editor.ChkMove.z = true;
                    }
                }

                const char* MoveAboutItems[] = { "Absolute", "Relative", "Center Group" };
                ImGui::SetNextItemWidth(130);
                ImGui::Combo("Offset", &Config->Editor.MoveAbout, MoveAboutItems, 3, 3);

            }
        }

        if (ImGui::CollapsingHeader("Materials")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Modeling"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Materials"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Groups"), 0);

            ImGui::PushItemWidth(182);

            static char Buffer[64];
            static int ListBox_MaterialItem = 0;
            static char **MaterialItems = new char*[Model->Obj.Materials.size()];

            if (Model->Obj.Materials.size() != 0) {

                for(unsigned int i = 0; i < Model->Obj.Materials.size(); i++) {
                    MaterialItems[i] = &Model->Obj.Materials[i].Name[0];
                }
                ImGui::SetNextItemWidth(182);
                if (ImGui::ListBox("", &ListBox_MaterialItem, MaterialItems, Model->Obj.Materials.size(), 6)) {
                    CurrentMaterial = ListBox_MaterialItem;
//                    sprintf(Buffer, Model->Obj.Materials[CurrentMaterial].Name.c_str());
                }

                ImGui::SetNextItemWidth(182);
                if (Model->Obj.Textures.size() != 0) {
                    ImGui::Image((void*)Model->Obj.Textures[ListBox_MaterialItem].GetTexture(), ImVec2(182, 182), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                }
                sprintf(Buffer, Model->Obj.Materials[CurrentMaterial].Name.c_str());
            }

            ImGui::PushItemWidth(182);

            if (Model->Obj.Materials.size() > 0) {

                ImGui::Text("Ambient ");
                ImGui::SameLine();
                ImGui::ColorEdit3("Ambient", &Model->Obj.Materials[CurrentMaterial].Ambient[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                ImGui::SameLine();
                ImGui::Text("Diffuse  ");
                ImGui::SameLine();
                ImGui::ColorEdit3("Diffuse", &Model->Obj.Materials[CurrentMaterial].Diffuse[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                ImGui::Text("Specular");
                ImGui::SameLine();
                ImGui::ColorEdit3("Specular", &Model->Obj.Materials[CurrentMaterial].Specular[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                ImGui::SameLine();
                ImGui::Text("Emissive ");
                ImGui::SameLine();
                ImGui::ColorEdit3("Emissive", &Model->Obj.Materials[CurrentMaterial].Emissive[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                ImGui::Text("Shininess");
                ImGui::SameLine();

                ImGui::SetNextItemWidth(112);
                ImGui::DragFloat("  ", &Model->Obj.Materials[CurrentMaterial].Shiny, 0.001f, 0.0f, 1.0f, "%0.4f", 1);

                ImGui::Separator();

                ImGui::PushItemWidth(182);

                if (ImGui::Button(Model->Obj.Materials[CurrentMaterial].DiffuseTexture.substr(Model->Obj.Materials[CurrentMaterial].DiffuseTexture.find_last_of("\\") + 1).c_str(), ImVec2(182, 18))) {
                    std::string File = FileOpen(NULL, (char*)("All Files\0*.*\0"), &std::string("")[0], &std::string("")[0]);
                    if(File.size() != 0) {
                        Model->AddUndo();
                        Model->Obj.Textures.resize(Model->Obj.Textures.size() + 1);
                        Model->Obj.Textures[Model->Obj.Textures.size() - 1].LoadImage(File);
                        Model->Obj.Materials[CurrentMaterial].DiffuseTexture = File.substr(File.find_last_of("\\") + 1);
                        Dirty = true;
                        Model->Update();
                    }
                }

                if (ImGui::Button("Assign", ImVec2(87, 18))) {
                    Model->AddUndo();
                    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                        if (Model->Obj.Groups[i].Selected) {
                            Model->Obj.Groups[i].MaterialIndex = CurrentMaterial;
                            Model->Obj.Groups[i].MaterialName = Model->Obj.Materials[CurrentMaterial].Name;
                            Dirty = true;
                            Model->Update();
                        }
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Select By", ImVec2(87, 18))) {
                    Model->AddUndo();
                    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                        Model->Obj.Groups[i].Selected = false;
                        if (Model->Obj.Groups[i].MaterialIndex == CurrentMaterial) {
                            Model->Obj.Groups[i].Selected = true;
                            Model->Update();
                        }
                    }
                }
            }

            if (ImGui::Button("New", ImVec2(87, 18))) {
                Model->AddUndo();
                CModel::Material Material;
                Material.Illumination = 1;
                Material.Ambient = glm::vec3(0.75f);
                Material.Diffuse = glm::vec3(0.85f);
                Material.Specular = glm::vec3(0.0f);
                Material.Emissive = glm::vec3(0.0f);
                Material.Shiny = 0.0f;
                static int Index = 0;
                Material.Name = "Material" + std::to_string(Index++);
                Material.DiffuseTexture = "Texture...";
                Model->Obj.Materials.push_back(Material);
                CurrentMaterial = Model->Obj.Materials.size() - 1;
                Dirty = true;
                Model->Update();
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(87, 18))) {
                if (Model->Obj.Materials.size() > 0) {
                    Model->AddUndo();
                    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                        if (Model->Obj.Groups[i].MaterialIndex == CurrentMaterial) {
                            Model->Obj.Groups[i].MaterialIndex = -1;
                        }
                    }
                    std::swap(Model->Obj.Materials[CurrentMaterial], Model->Obj.Materials[Model->Obj.Materials.size() - 1]);
                    Model->Obj.Textures[Model->Obj.Materials.size() - 1].Destroy();
                    Model->Obj.Materials.resize(Model->Obj.Materials.size() - 1);

                    if (Model->Obj.Materials.size() > 0) {
                        CurrentMaterial = Model->Obj.Materials.size() - 1;
                        ListBox_MaterialItem = CurrentMaterial;
                    }
                }
                Dirty = true;
                Model->Update();
            }

            ImGui::SetNextItemWidth(87);

            if (Model->Obj.Materials.size() != 0) {

                ImGui::SetNextItemWidth(87);
                if (ImGui::InputText("", &Buffer[0], 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    Model->AddUndo();
                    Model->Obj.Materials[CurrentMaterial].Name = Buffer;
                    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                        if (Model->Obj.Groups[i].Selected) {
                            Model->Obj.Groups[i].MaterialIndex = CurrentMaterial;
                            Model->Obj.Groups[i].MaterialName = Model->Obj.Materials[CurrentMaterial].Name;
                            Model->Update();
                        }
                    }
                    Dirty = true;
                    Model->Update();
                }

                ImGui::SameLine();
                if (ImGui::Button("Rename ", ImVec2(87, 18))) {
                    Model->AddUndo();
                    Model->Obj.Materials[CurrentMaterial].Name = Buffer;
                    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                        if (Model->Obj.Groups[i].Selected) {
                            Model->Obj.Groups[i].MaterialIndex = CurrentMaterial;
                            Model->Obj.Groups[i].MaterialName = Model->Obj.Materials[CurrentMaterial].Name;
                            Model->Update();
                        }
                    }
                    Dirty = true;
                    Model->Update();
                }
            }
        }

        if (ImGui::CollapsingHeader("Groups")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Modeling"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Materials"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Groups"), 1);

            ImGui::PushItemWidth(182);

            static char Buffer[64];
            static int ListBox_GroupItem = 0;
            static char **GroupItems = new char*[Model->Obj.Groups.size()];

            if (Model->Obj.Groups.size() != 0) {
                for(unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                    GroupItems[i] = &Model->Obj.Groups[i].Name[0];
                }
                if (ImGui::ListBox("", &ListBox_GroupItem, GroupItems, Model->Obj.Groups.size(), 6)) {
                    CurrentGroup = ListBox_GroupItem;
                    sprintf(Buffer, Model->Obj.Groups[CurrentGroup].Name.c_str());
                }

                if (ImGui::IsMouseDoubleClicked(0)) {
                    Model->AddUndo();
                    if (Model->Obj.Groups.size() != 0) {
                        Model->Obj.Groups[CurrentGroup].Selected = !Model->Obj.Groups[CurrentGroup].Selected;
                    }
                }

            }

            if (ImGui::Button("Select", ImVec2(87, 18))) {
                if (Model->Obj.Groups.size() != 0) {
                    Model->AddUndo();
                    Model->Obj.Groups[CurrentGroup].Selected = !Model->Obj.Groups[CurrentGroup].Selected;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Hide", ImVec2(87, 18))) {
                if (Model->Obj.Groups.size() != 0) {
                    Model->AddUndo();
                    Model->Obj.Groups[CurrentGroup].Visible = !Model->Obj.Groups[CurrentGroup].Visible;
                }
            }

            if (ImGui::Button("Regroup", ImVec2(87, 18))) {

            }

            ImGui::SameLine();
            if (ImGui::Button("Delete", ImVec2(87, 18))) {
                Model->AddUndo();

                int Index = 0;
                for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
                    if (Model->Obj.Groups[i].Selected == false) Model->Obj.Groups[Index++] = Model->Obj.Groups[i];
                }
                Model->Obj.Groups.resize(Index);
            }

            if (ImGui::Button("Move Up", ImVec2(87, 18))) {
                if (Model->Obj.Groups.size() != 0 && CurrentGroup > 0) {
                    Model->AddUndo();
                    std::swap(Model->Obj.Groups[CurrentGroup], Model->Obj.Groups[CurrentGroup - 1]);
                    ListBox_GroupItem--;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Move Down", ImVec2(87, 18))) {
                if (Model->Obj.Groups.size() != 0 && (unsigned int)CurrentGroup < Model->Obj.Groups.size() - 1) {
                    Model->AddUndo();
                    std::swap(Model->Obj.Groups[CurrentGroup + 1], Model->Obj.Groups[CurrentGroup]);
                    ListBox_GroupItem++;
                }
            }

            if (Model->Obj.Groups.size() != 0) {

                ImGui::SetNextItemWidth(87);
                if (ImGui::InputText("", &Buffer[0], 64, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    Model->AddUndo();
                    Model->Obj.Groups[CurrentGroup].Name = Buffer;
                }

                ImGui::SameLine();
                if (ImGui::Button("Rename", ImVec2(87, 18))) {
                    Model->AddUndo();
                    Model->Obj.Groups[CurrentGroup].Name = Buffer;
                }
            }
        }

        ImGui::End();
    }

    static double XPos, YPos;
    glfwGetCursorPos(GLWindow, &XPos, &YPos);

    if (ImGui::IsMouseReleased(1) && Config->Editor.Viewport > -1) {
        ImGui::OpenPopup("Menu");
    }

    if (ImGui::BeginPopup("Menu")) {
        Config->Editor.Viewport = Config->Editor.LastView;
        if (ImGui::BeginMenu("Projection")) {
            if (ImGui::MenuItem("3D", "", Config->Viewports[Config->Editor.Viewport].Projection == 0)) {
                Config->SetCamera(glm::vec3(0, -5, -25), glm::vec3(1), glm::vec3(0, -135, 0), 0, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Front", "", Config->Viewports[Config->Editor.Viewport].Projection == 1)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(0, -180, 0), 1, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Back", "", Config->Viewports[Config->Editor.Viewport].Projection == 2)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(0), 2, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Left", "", Config->Viewports[Config->Editor.Viewport].Projection == 3)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(0, 90, 0), 3, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Right", "", Config->Viewports[Config->Editor.Viewport].Projection == 4)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(0, -90, 0), 4, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Top", "", Config->Viewports[Config->Editor.Viewport].Projection == 5)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(90, 0, 0), 5, Config->Editor.Viewport);
            }
            if (ImGui::MenuItem("Bottom", "", Config->Viewports[Config->Editor.Viewport].Projection == 6)) {
                Config->SetCamera(glm::vec3(0), glm::vec3(1), glm::vec3(-90, 0, 0), 6, Config->Editor.Viewport);
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::BeginMenu("Render")) {
            if (ImGui::MenuItem("Wireframe", "", Config->Viewports[Config->Editor.Viewport].RenderMode == 0)) {
                Config->Viewports[Config->Editor.Viewport].RenderMode = 0;
                Model->Update();
            }
            if (ImGui::MenuItem("Flat Shaded", "", Config->Viewports[Config->Editor.Viewport].RenderMode == 1)) {
                Config->Viewports[Config->Editor.Viewport].RenderMode = 1;
                Model->Update();
            }
            if (ImGui::MenuItem("Smooth Shaded", "", Config->Viewports[Config->Editor.Viewport].RenderMode == 2)) {
                Config->Viewports[Config->Editor.Viewport].RenderMode = 2;
                Model->Update();
            }
            if (ImGui::MenuItem("Textured", "", Config->Viewports[Config->Editor.Viewport].RenderMode == 3)) {
                Config->Viewports[Config->Editor.Viewport].RenderMode = 3;
                Model->Update();
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Overlay", "", Config->Viewports[Config->Editor.Viewport].Overlay)) {
            Config->Viewports[Config->Editor.Viewport].Overlay = !Config->Viewports[Config->Editor.Viewport].Overlay;
        }
        if (ImGui::MenuItem("Grid", "", Config->Viewports[Config->Editor.Viewport].Grid)) {
            Config->Viewports[Config->Editor.Viewport].Grid = !Config->Viewports[Config->Editor.Viewport].Grid;
        }
        if (ImGui::MenuItem("Axis", "", Config->Viewports[Config->Editor.Viewport].Axis)) {
            Config->Viewports[Config->Editor.Viewport].Axis = !Config->Viewports[Config->Editor.Viewport].Axis;
        }
        if (ImGui::MenuItem("Culling", "", Config->Viewports[Config->Editor.Viewport].Culling)) {
            Config->Viewports[Config->Editor.Viewport].Culling = !Config->Viewports[Config->Editor.Viewport].Culling;
        }
        if (ImGui::MenuItem("Transparency", "", Config->Viewports[Config->Editor.Viewport].Transparent)) {
            Config->Viewports[Config->Editor.Viewport].Transparent = !Config->Viewports[Config->Editor.Viewport].Transparent;
        }
        ImGui::Separator();

        ImGui::Text("Back Color     ");
        ImGui::SameLine();
        ImGui::ColorEdit4("BackColor", (float*)&Config->Viewports[Config->Editor.Viewport].ClearColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::Text("Grid Color     ");
        ImGui::SameLine();
        ImGui::ColorEdit4("GridColor", (float*)&Config->Viewports[Config->Editor.Viewport].GridColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

        ImGui::Separator();
        if (ImGui::MenuItem("Reset View")) {
            Config->ResetView(Config->Editor.Viewport);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Frame All", "")) {
            Tools::Frame(Config->Editor.Viewport, true);
        }
        if (ImGui::MenuItem("Frame Selection", "", false, Config->Editor.Selected)) {
            Tools::Frame(Config->Editor.Viewport, false);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Maximize View", "", Maximize, true)) {
            Maximize = !Maximize;
            if (Maximize) {
                Interface::Resize(Config->Editor.Viewport);
            } else {
                Interface::Resize(-1);
            }
        }
        ImGui::EndPopup();
    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Interface::Resize(int Maximize, bool Drag)
{
    static int Width, Height;
    static double XPos, YPos;

    glfwGetCursorPos(GLWindow, &XPos, &YPos);
    glfwGetFramebufferSize(GLWindow, &Width, &Height);

    int SpaceLeft = (Config->General.SideBar ? 202 : 4);
    int SpaceBottom = (Config->General.StatusBar ? 22 : 4);
    int SpaceWidth = (Width - (Config->General.SideBar ? 202 : 4));
    int SpaceHeight = (Height - (Config->General.StatusBar ? 22 : 4) - 22);

    if (Drag) {

        YPos = (Height - YPos);
        SpaceLeft = (Config->General.SideBar ? 202 : 4);
        SpaceBottom = (Config->General.StatusBar ? 22 : 4);
        SpaceWidth = 2.0f * (XPos - ((Config->General.SideBar ? 202 : 4)));
        SpaceHeight = 2.0f * (YPos - ((Config->General.StatusBar ? 22 : 4) - 22));

        Viewports[0].X = SpaceLeft;
        Viewports[0].Width = (SpaceWidth / 2) - 4;
        Viewports[0].Y = SpaceBottom + 4;
        Viewports[0].Height = (SpaceHeight / 2) - SpaceBottom - 4;

        Viewports[1].X = SpaceLeft;
        Viewports[1].Width = (SpaceWidth / 2) - 4;
        Viewports[1].Y = SpaceBottom + 4 + Viewports[0].Height + 4;
        Viewports[1].Height = (Height - (Config->General.StatusBar ? 24 : 4) - 22) - Viewports[0].Height - 8;

        Viewports[2].X = SpaceLeft + 4 + Viewports[1].Width;
        Viewports[2].Width = Width - (Config->General.SideBar ? 202 : 4) - Viewports[1].Width - 8;
        Viewports[2].Y = SpaceBottom + 4 + Viewports[0].Height + 4;
        Viewports[2].Height = (Height - (Config->General.StatusBar ? 24 : 4) - 22) - Viewports[0].Height - 8;

        Viewports[3].X = SpaceLeft + 4 + Viewports[1].Width;
        Viewports[3].Width = Width - (Config->General.SideBar ? 202 : 4) - Viewports[1].Width - 8;
        Viewports[3].Y = SpaceBottom + 4;
        Viewports[3].Height = (SpaceHeight / 2) - SpaceBottom - 4;

    } else if (Maximize != -1) {

        for (int i = 0; i < 4; i++) {
            Viewports[i].X = 0;
            Viewports[i].Width = 1;
            Viewports[i].Y = 0;
            Viewports[i].Height = 1;
        }

        Viewports[Maximize].X = SpaceLeft;
        Viewports[Maximize].Width = SpaceWidth - 4;
        Viewports[Maximize].Y = SpaceBottom;
        Viewports[Maximize].Height = SpaceHeight - 4;

    } else {

        Viewports[0].X = SpaceLeft;
        Viewports[0].Width = (SpaceWidth / 2) - 4;
        Viewports[0].Y = SpaceBottom + 4;
        Viewports[0].Height = (SpaceHeight / 2) - 4;

        Viewports[1].X = SpaceLeft;
        Viewports[1].Width = (SpaceWidth / 2) - 4;
        Viewports[1].Y = SpaceBottom + 4 + (SpaceHeight / 2);
        Viewports[1].Height = (SpaceHeight / 2) - 4;

        Viewports[2].X = SpaceLeft + (SpaceWidth / 2);
        Viewports[2].Width = (SpaceWidth / 2) - 4;
        Viewports[2].Y = SpaceBottom + 4 + (SpaceHeight / 2);
        Viewports[2].Height = (SpaceHeight / 2) - 4;

        Viewports[3].X = SpaceLeft + (SpaceWidth / 2);
        Viewports[3].Width = (SpaceWidth / 2) - 4;
        Viewports[3].Y = SpaceBottom + 4;
        Viewports[3].Height = (SpaceHeight / 2) - 4;
    }
}
