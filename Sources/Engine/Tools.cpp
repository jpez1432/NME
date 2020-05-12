
#include "Tools.hpp"

#include "Model.hpp"

extern CModel *Model;

int Tools::FindView(double XPos, double YPos)
{
    if (XPos < (Config->General.SideBar ? 202 : 0)) return -2;
    if (YPos > Config->Graphics.Height - (Config->General.StatusBar ? 22 : 0)) return -3;
    if (YPos < 22) return -4;

    for (int i = 0; i < 4; i++) {
        if (XPos > Viewports[i].X && XPos < Viewports[i].X + Viewports[i].Width) {
            if (Config->Graphics.Height - YPos > Viewports[i].Y && Config->Graphics.Height - YPos < Viewports[i].Y + Viewports[i].Height) {
                return i;
            }
        }
    }
    return -1;
}

void Tools::Frame(int Index, bool All)
{
    glm::vec3 Dimensions, Min, Max;

    Dimensions = Model->CalculateBounds(Min, Max, All);

    if (Config->Viewports[Index].Projection == 0) {

        Config->Viewports[Index].Scale = glm::vec3(1.0f);
        Config->Viewports[Index].Pan = glm::vec3(0.0f, 0.0f, -Model->GetBoundingSphere());

    } else if (Config->Viewports[Index].Projection == 1) {

        Config->Viewports[Index].Scale = glm::vec3(Dimensions.x >= Dimensions.y && Viewports[Index].Width <= Viewports[Index].Height ? Viewports[Index].Width : Viewports[Index].Height) / (Dimensions.x >= Dimensions.y ? Dimensions.x : Dimensions.y);
        Config->Viewports[Index].Pan = glm::vec3((Max.x + Min.x) / 2.0f, -(Max.y + Min.y) / 2.0f, 0.0f);

    } else if (Config->Viewports[Index].Projection == 2) {

        Config->Viewports[Index].Scale = glm::vec3(Dimensions.x >= Dimensions.y && Viewports[Index].Width <= Viewports[Index].Height ? Viewports[Index].Width : Viewports[Index].Height) / (Dimensions.x >= Dimensions.y ? Dimensions.x : Dimensions.y);
        Config->Viewports[Index].Pan = glm::vec3(-(Max.x + Min.x) / 2.0f, -(Max.y + Min.y) / 2.0f, 0.0f);

    } else if (Config->Viewports[Index].Projection == 3) {

        Config->Viewports[Index].Scale = glm::vec3(Dimensions.z <= Dimensions.y && Viewports[Index].Width >= Viewports[Index].Height ? Viewports[Index].Height : Viewports[Index].Width) / (Dimensions.z >= Dimensions.y ? Dimensions.z : Dimensions.y);
        Config->Viewports[Index].Pan = glm::vec3(-(Max.z + Min.z) / 2.0f, -(Max.y + Min.y) / 2.0f, 0.0f);

    } else if (Config->Viewports[Index].Projection == 4) {

        Config->Viewports[Index].Scale = glm::vec3(Dimensions.z <= Dimensions.y && Viewports[Index].Width >= Viewports[Index].Height ? Viewports[Index].Height : Viewports[Index].Width) / (Dimensions.z >= Dimensions.y ? Dimensions.z : Dimensions.y);
        Config->Viewports[Index].Pan = glm::vec3((Max.z + Min.z) / 2.0f, -(Max.y + Min.y) / 2.0f, 0.0f);

    } else if (Config->Viewports[Index].Projection == 5) {

        Config->Viewports[Index].Scale =  glm::vec3(Dimensions.x >= Dimensions.z && Viewports[Index].Width <= Viewports[Index].Height ? Viewports[Index].Width : Viewports[Index].Height) / (Dimensions.x >= Dimensions.z ? Dimensions.x : Dimensions.z);
        Config->Viewports[Index].Pan = glm::vec3(-(Max.x + Min.x) / 2.0f, (Max.z + Min.z) / 2.0f, 0.0f);

    } else if (Config->Viewports[Index].Projection == 6) {

        Config->Viewports[Index].Scale =  glm::vec3(Dimensions.x >= Dimensions.z && Viewports[Index].Width <= Viewports[Index].Height ? Viewports[Index].Width : Viewports[Index].Height) / (Dimensions.x >= Dimensions.z ? Dimensions.x : Dimensions.z);
        Config->Viewports[Index].Pan = glm::vec3(-(Max.x + Min.x) / 2.0f, -(Max.z + Min.z) / 2.0f, 0.0f);

    }

    Config->Viewports[Index].Scale *= (0.95f);

}

void Tools::Select(int Index, bool Start)
{
//    Model->AddUndo();

    glm::vec3 TempPos;

    CConfig::Rect Swap = Config->Editor.Selection;
    if (Config->Editor.Selection.X > Config->Editor.Selection.Width) {
        Swap.X = Config->Editor.Selection.Width;
        Swap.Width = Config->Editor.Selection.X;
    }
    if (Config->Editor.Selection.Y > Config->Editor.Selection.Height) {
        Swap.Y = Config->Editor.Selection.Height;
        Swap.Height = Config->Editor.Selection.Y;
    }

    if (glfwGetKey(GLWindow, GLFW_KEY_LEFT_CONTROL) || !glfwGetMouseButton(GLWindow, GLFW_MOUSE_BUTTON_LEFT) || Model->Obj.Vertices.size() == 0) return;

    static std::vector<bool> SelectedVertex;
    static std::vector<bool> SelectedFace;
//    static std::vector<bool> SelectedGroup;

    if (Start) {

        SelectedVertex.resize(Model->Obj.Vertices.size() - 1);
        for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
            SelectedVertex[i] = Model->Obj.Vertices[i].Selected;
        }

        SelectedFace.resize(Model->Obj.Triangles.size());
        for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
            SelectedFace[j] = Model->Obj.Triangles[j].Selected;
        }

//        SelectedGroup.resize(Model->Obj.Groups.size());
//        for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
//            SelectedGroup[j] = Model->Obj.Groups[j].Selected;
//        }
    }

    if (Config->Editor.Option == 0) {

        if (Config->Viewports[Index].Projection == 1){

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= glm::vec3(-Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, 0.0f);

                if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }

        } else if (Config->Viewports[Index].Projection == 2) {

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= Config->Viewports[Index].Scale;
                TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, 0.0f);

                if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }

        } else if (Config->Viewports[Index].Projection == 3) {

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                TempPos += glm::vec3(0.0f, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, Viewports[Index].Width + Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                TempPos += glm::vec3(0.0f, 0.0f, Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x);

                if (TempPos.z > Swap.X && TempPos.z < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }

        } else if (Config->Viewports[Index].Projection == 4) {

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, -Config->Viewports[Index].Scale.z);
                TempPos += glm::vec3(0.0f, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, Viewports[Index].Width + -Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                TempPos += glm::vec3(0.0f, 0.0f, Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x);

                if (TempPos.z > Swap.X && TempPos.z < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }

        } else if (Config->Viewports[Index].Projection == 5) {

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, -Config->Viewports[Index].Scale.z);
                TempPos += glm::vec3(Viewports[Index].Width - Config->Viewports[Index].Pan.x, 0.0f, Viewports[Index].Height - Config->Viewports[Index].Pan.z);
                TempPos += glm::vec3(Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y);

                if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.z > Swap.Y && TempPos.z < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }

        } else if (Config->Viewports[Index].Projection == 6) {

            for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
                TempPos = Model->Obj.Vertices[i].Coord;
                TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                TempPos += glm::vec3(Viewports[Index].Width - Config->Viewports[Index].Pan.x, 0.0f, Viewports[Index].Height - Config->Viewports[Index].Pan.z);
                TempPos += glm::vec3(Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y);

                if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.z > Swap.Y && TempPos.z < Swap.Height) {
                    Model->Obj.Vertices[i].Selected = true;
                } else {
                    Model->Obj.Vertices[i].Selected = false;
                }
            }
        }

        for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
            if (SelectedVertex[i] == true) {
                Model->Obj.Vertices[i].Selected = true;
            }
        }

    } else if (Config->Editor.Option == 1) {

        if (Config->Editor.Selection.Width == Config->Editor.Selection.X && Config->Editor.Selection.Height == Config->Editor.Selection.Y) {

            double XPos, YPos;
            unsigned char Array[3];

            glfwGetCursorPos(GLWindow, &XPos, &YPos);
            glReadPixels(XPos, Config->Graphics.Height - YPos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Array);

            for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                if (Model->Obj.Triangles[j].Visible) {
                    if ((int(Model->Obj.Triangles[j].Color[0] * 255) == Array[0] &&
                        (int(Model->Obj.Triangles[j].Color[1] * 255) == Array[1] &&
                        (int(Model->Obj.Triangles[j].Color[2] * 255) == Array[2])))) {
                        Model->Obj.Triangles[j].Selected = true;
                    }
                }
            }

            for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                if (SelectedFace[j] == true) {
                    Model->Obj.Triangles[j].Selected = true;
                }
            }

        } else if (Config->Editor.SelectByVertex) {

            switch (Config->Viewports[Index].Projection) {
                case 1:

                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= glm::vec3(-Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                        TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, 0.0f);
                        if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

                case 2:
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= Config->Viewports[Index].Scale;
                        TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, 0.0f);
                        if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

                case 3:
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                        TempPos += glm::vec3(0.0f, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, Viewports[Index].Width + Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                        TempPos += glm::vec3(0.0f, 0.0f, Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x);

                        if (TempPos.z > Swap.X && TempPos.z < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

                case 4:
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, -Config->Viewports[Index].Scale.z);
                        TempPos += glm::vec3(0.0f, Viewports[Index].Height + Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y, Viewports[Index].Width + -Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                        TempPos += glm::vec3(0.0f, 0.0f, Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x);

                        if (TempPos.z > Swap.X && TempPos.z < Swap.Width && TempPos.y > Swap.Y && TempPos.y < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

                case 5:
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, -Config->Viewports[Index].Scale.z);
                        TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Viewports[Index].Height + -Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                        TempPos += glm::vec3(Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y);

                        if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.z > Swap.Y && TempPos.z < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

                case 6:
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    for (unsigned int k = 0; k < 3; k++) {

                        TempPos = Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Coord;
                        TempPos *= glm::vec3(Config->Viewports[Index].Scale.x, Config->Viewports[Index].Scale.y, Config->Viewports[Index].Scale.z);
                        TempPos += glm::vec3(Viewports[Index].Width + Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Viewports[Index].Height + Config->Viewports[Index].Scale.z * Config->Viewports[Index].Pan.z);
                        TempPos += glm::vec3(Config->Viewports[Index].Scale.x * Config->Viewports[Index].Pan.x, 0.0f, Config->Viewports[Index].Scale.y * Config->Viewports[Index].Pan.y);

                        if (TempPos.x > Swap.X && TempPos.x < Swap.Width && TempPos.z > Swap.Y && TempPos.z < Swap.Height) {
                            Model->Obj.Triangles[j].Selected = true;
//                            if (Config->Editor.SelectByVertex) continue;
                        } else {
                            Model->Obj.Triangles[j].Selected = false;
                        }
                    }
                }
                break;

            }
        }

        for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
            if (SelectedFace[j] == true || Model->Obj.Triangles[j].Selected) {
                for (unsigned int k = 0; k < 3; k++) {
                    Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Selected = true;
                }
            } else {
                for (unsigned int k = 0; k < 3; k++) {
                    Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Selected = false;
                }
            }
        }

    } else if (Config->Editor.Option == 2) {

        double XPos, YPos;
        unsigned char Array[3];

        glfwGetCursorPos(GLWindow, &XPos, &YPos);
        glReadPixels(XPos, Config->Graphics.Height - YPos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Array);

        if ((0x0 | (0x0 << 8) | (0x0 << 16)) == ((Array[0]) | (Array[1] << 8) | (Array[2] << 16))) {
            CurrentGroup = -1;
        } else {
            CurrentGroup = ((Array[0]) | (Array[1] << 8) | (Array[2] << 16)) - 1;
        }

        if (CurrentGroup > int(Model->Obj.Groups.size() - 1)) CurrentGroup = -1;

        if (CurrentGroup != -1) {
            CurrentMaterial = Model->Obj.Groups[CurrentGroup].MaterialIndex;
            Model->Obj.Groups[CurrentGroup].Selected = true;
            Model->Obj.Groups[CurrentGroup].Visible = true;
            for (unsigned int i = 0; i < Model->Obj.Triangles.size(); i++) {
                if (CurrentGroup == Model->Obj.Triangles[i].GroupIndex) {
                    Model->Obj.Triangles[i].Selected = true;
                    for (unsigned int j = 0; j < 3; j++) {
                        Model->Obj.Vertices[Model->Obj.Triangles[i].Vertex[j]].Selected = true;
                    }
                }
            }
        }

//        for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
//            if (SelectedGroup[i] == true) {
//                Model->Obj.Groups[i].Selected = true;
//            }
//        }

    }

    if (Config->Editor.Selected == false) {
        for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
            if (Model->Obj.Groups[i].Selected) {
                Config->Editor.Selected = true;
                return;
            }
            for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                if (Model->Obj.Triangles[j].Selected) {
                    Config->Editor.Selected = true;
                    return;
                }
                for (unsigned int k = 0; k < 3; k++) {
                    if (Model->Obj.Vertices[Model->Obj.Triangles[j].Vertex[k]].Selected) {
                        Config->Editor.Selected = true;
                        return;
                    }
                }
            }
        }
    }

    Model->Update(true);

}

void Tools::DuplicateSelection(void)
{
    Model->AddUndo();

    for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
        if (Model->Obj.Groups[i].Selected) {
            Model->Obj.Groups.resize(Model->Obj.Groups.size() + 1);
            Model->Obj.Groups[Model->Obj.Groups.size() - 1] = Model->Obj.Groups[i];
            Model->Obj.Groups[Model->Obj.Groups.size() - 1].Selected = false;
            for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                if (Model->Obj.Triangles[j].Selected && Model->Obj.Triangles[j].GroupIndex == int(i)) {
                    Model->Obj.Triangles.resize(Model->Obj.Triangles.size() + 1);
                    Model->Obj.Triangles[Model->Obj.Triangles.size() - 1] = Model->Obj.Triangles[j];
                    Model->Obj.Triangles[Model->Obj.Triangles.size() - 1].GroupIndex = Model->Obj.Groups.size() - 1;
                }
            }
        }
    }

    Model->Update();

}

void Tools::DeleteSelection(int Option)
{
    ///////////////////////////////////////////

    Model->AddUndo();

    if (Option == 0) {

        int Index = 0;
        for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {
            if (Model->Obj.Vertices[i].Selected == false)  Model->Obj.Vertices[Index++] =  Model->Obj.Vertices[i];
        }

        Model->Obj.Vertices.resize(Index);


//    } else if (Option == 1) {
//
//        std::vector<CModel::Triangle> TriList;
////        for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
////            if (Model->Obj.Groups[i].Selected) {
//                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
//                    if (/*Model->Obj.Triangles[j].GroupIndex != int(i) && */!Model->Obj.Triangles[j].Selected) {
//                        TriList.push_back(Model->Obj.Triangles[j]);
//                    }
//                }
////            }
////        }
//        Model->Obj.Triangles = TriList;
//
//
//    } else if (Option == 2) {

    } else if (Option == 1 || Option == 2) {

        int Index = 0;

        std::vector<CModel::Triangle> TriList;
//        for (unsigned int i = 0; i < Model->Obj.Groups.size(); i++) {
//            if (Model->Obj.Groups[i].Selected) {
                for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                    if (/*Model->Obj.Triangles[j].GroupIndex != int(i) && */!Model->Obj.Triangles[j].Selected) {
                        TriList.push_back(Model->Obj.Triangles[j]);
                    }
                }
//            }
//        }
        Model->Obj.Triangles = TriList;

        Index = 0;
        std::vector<int> VertIndex;
        std::vector<CModel::Vertex> VertList;
        for (unsigned int i = 0; i < Model->Obj.Vertices.size(); i++) {

            if (!Model->Obj.Vertices[i].Selected) {
                VertIndex.push_back(Index++);
                VertList.push_back(Model->Obj.Vertices[i]);
            } else {
                VertIndex.push_back(Index);
            }
            for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
                for (unsigned int k = 0; k < 3; k++) {
                    if (Model->Obj.Triangles[j].Vertex[k] == i) {
                        Model->Obj.Triangles[j].Vertex[k] = VertIndex[i];
                    }
                }
            }
        }
        Model->Obj.Vertices = VertList;

    }

    Model->Update();
    ///////////////////////////
}

void Tools::DeleteAll(void)
{
    Model->AddUndo();

    Model->Obj.Groups.clear();
    Model->Obj.Vertices.clear();
    Model->Obj.Normals.clear();
    Model->Obj.Materials.clear();
    Model->Obj.TexCoords.clear();
    Model->Obj.Triangles.clear();

    Model->Update();

}

void Tools::SelectAll(void)
{
    Model->AddUndo();

    for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
        Model->Obj.Groups[j].Selected = true;
    }
    for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
        Model->Obj.Triangles[j].Selected = true;
    }
    for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
        Model->Obj.Vertices[j].Selected = true;
    }

    Model->Update();

}

void Tools::SelectNone(void)
{
    Model->AddUndo();

    for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
        Model->Obj.Groups[j].Selected = false;
    }
    for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
        Model->Obj.Triangles[j].Selected = false;
    }
    for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
        Model->Obj.Vertices[j].Selected = false;
    }

    Model->Update();

}

void Tools::SelectInvert(void)
{
    Model->AddUndo();

    for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
        Model->Obj.Groups[j].Selected = !Model->Obj.Groups[j].Selected;
    }
    for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
        Model->Obj.Triangles[j].Selected = !Model->Obj.Triangles[j].Selected;
    }
    for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
        Model->Obj.Vertices[j].Selected = !Model->Obj.Vertices[j].Selected;
    }

    Model->Update();
}

void Tools::HideSelection(void)
{
    Model->AddUndo();

    for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
        if (Model->Obj.Groups[j].Selected) { Model->Obj.Groups[j].Visible = false; }
    }
    for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
        if (Model->Obj.Triangles[j].Selected) { Model->Obj.Triangles[j].Visible = false; }
    }
    for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
        if (Model->Obj.Vertices[j].Selected) { Model->Obj.Vertices[j].Visible = false; }
    }

    Model->Update();
}

void Tools::HideNone(void)
{
    Model->AddUndo();

    for (unsigned int j = 0; j < Model->Obj.Groups.size(); j++) {
        Model->Obj.Groups[j].Visible = true;
    }
    for (unsigned int j = 0; j < Model->Obj.Triangles.size(); j++) {
        Model->Obj.Triangles[j].Visible = true;
    }
    for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
        Model->Obj.Vertices[j].Visible = true;
    }

    Model->Update();
}

void Tools::Rotate(glm::vec3 Rotation, glm::bvec3 Include, int About, bool Global)
{
///////////////////////////
    Model->AddUndo();

//    int Count = 0;
    glm::vec3 Center(0);

    switch (About) {

        case 0:
//        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
//            if (Model->Obj.Vertices[j].Selected) {
//                Count++;
//                Center += Model->Obj.Vertices[j].Coord;
//            }
//        }
//        Center /= Count;
//        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
//            if (Model->Obj.Vertices[j].Selected) {
//                Model->Obj.Vertices[j].Coord += (glm::vec3(0) - Center) + Move;
//            }
//        }
        break;

        case 1:
//        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
//            if (Model->Obj.Vertices[j].Selected) {
//                Model->Obj.Vertices[j].Coord += Move;
//            }
//        }
        break;

    }

    Model->Update();
    ////////////////////////////
}

void Tools::Scale(glm::vec3 Scale, glm::bvec3 Include, int About, bool Into)
{
    Model->AddUndo();

    glm::vec3 Dimensions, Center;
    glm::vec3 Min, Max;

    if (!Include.x) Scale.x = 1.0;
    if (!Include.y) Scale.y = 1.0;
    if (!Include.z) Scale.z = 1.0;

    Dimensions = Model->CalculateBounds(Min, Max, false);
    Center = Dimensions / 2.0f;

    if (Into) {

        if (!Include.x) Dimensions.x = 1.0;
        if (!Include.y) Dimensions.y = 1.0;
        if (!Include.z) Dimensions.z = 1.0;

        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
            if (Model->Obj.Vertices[j].Selected) {
                Model->Obj.Vertices[j].Coord *= Scale / Dimensions;
            }
        }

    } else {

        switch (About) {
//////////////////////
            case 0:
            for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
                if (Model->Obj.Vertices[j].Selected) {

                    Model->Obj.Vertices[j].Coord *= (Center - Scale);

                }
            }
            break;
//////////////////////
            case 1:
            for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
                if (Model->Obj.Vertices[j].Selected) {
                    Model->Obj.Vertices[j].Coord *= Scale;
                }
            }
            break;

        }

    }

    Model->Update();
}

void Tools::Move(glm::vec3 Move, glm::bvec3 Include, int Offset)
{
    Model->AddUndo();

    int Count = 0;
    glm::vec3 Center(0);
    Move *= Include;

    switch (Offset) {

        case 0:
        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
            if (Model->Obj.Vertices[j].Selected) {
                Count++;
                Center += Model->Obj.Vertices[j].Coord;
            }
        }
        Center /= Count;
        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
            if (Model->Obj.Vertices[j].Selected) {
                Model->Obj.Vertices[j].Coord += (glm::vec3(0) - Center) + Move;
            }
        }
        break;

        case 1:
        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
            if (Model->Obj.Vertices[j].Selected) {
                Model->Obj.Vertices[j].Coord += Move;
            }
        }
        break;

///////////////////////
        case 2:
        for (unsigned int j = 0; j < Model->Obj.Vertices.size(); j++) {
            if (Model->Obj.Vertices[j].Selected) {



            }
        }
        break;
//////////////////////
    }

    Model->Update();
}
