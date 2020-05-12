
#include "Model.hpp"

CModel::CModel(void)
{
    New();
    Obj.Materials.reserve(64);
}

CModel::~CModel(void)
{
    Obj.Vertices.clear();
    Obj.Normals.clear();
    Obj.TexCoords.clear();
    Obj.Groups.clear();
    Obj.Materials.clear();
    Obj.Triangles.clear();

    for (unsigned int i = 0;  i < Obj.Textures.size(); i++) {
        Obj.Textures[i].Destroy();
    }

    sprintf(Config->General.LastFile, Filename.c_str());
}

bool CModel::New(bool ClearEdits)
{
    if (Dirty) {
        if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
            Save(true);
        }
    }

    Filename = "Untitled.obj";
    Obj.MaterialLib = "Untitled.mtl";
    Obj.Vertices.clear();
    Obj.Normals.clear();
    Obj.FaceNormals.clear();
    Obj.TexCoords.clear();
    Obj.Groups.clear();
    Obj.Materials.clear();
    Obj.Textures.clear();
    Obj.Triangles.clear();

    if (ClearEdits) {
        Undos.clear();
        Redos.clear();
    }

    BoundingSphere = 0.0f;
    Center = Dimensions = Min = Max = glm::vec3(0);

    for (unsigned int i = 0; i < 4; i++) {
        Config->ResetView(i);
    }

    glfwSetWindowTitle(GLWindow, GetInfo().c_str());

    Dirty = false;

    return true;
}

bool CModel::Load(bool Merge, std::string Filename)
{
    glm::ivec3 Offsets = glm::ivec3(Obj.Vertices.size(), Obj.Normals.size(), Obj.TexCoords.size());

    if (!Merge) {
        if (Dirty) {
            if(MessageBox(NULL, "Would You Like To Save Changes?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                if (StrCompare(this->Filename, "Untitled.Obj")) {
                    Save();
                } else {
                    Save(false);
                }
            }
        }
        if (Filename.size() != 0) {
            this->Filename = Filename;
        } else {
            std::string File = FileOpen(NULL, (char*)("Obj Models\0*.Obj\0All Files\0*.*\0"), &std::string("")[0], &std::string(".Obj")[0]);
            if(File.size() != 0) {
                New(false);
                Offsets = glm::ivec3(0);
                this->Filename = File;
            } else {
                return true;
            }
        }
    } else {
        std::string File = FileOpen(NULL, (char*)("Obj Models\0*.Obj\0All Files\0*.*\0"), &std::string("")[0], &std::string(".Obj")[0]);
        if(File.size() != 0) {
            this->Filename = File;
        }
        Dirty = true;
    }

    std::string Line;
    bool Duplicate = false;
    Group Group;
    Triangle Triangle;
    Material Material;
    Vertex Vertex;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    std::string Path = this->Filename.substr(0, this->Filename.find_last_of("\\"));
    std::ifstream StreamObj(this->Filename.c_str(), std::ios::in);

    if ( StreamObj.fail() ) return false;

    while ( !StreamObj.eof() && !StreamObj.fail()){

        StreamObj >> Line;

        if ( Line == "mtllib" ) {

            StreamObj >> Obj.MaterialLib;

            Obj.MaterialLib.insert(0, Path + "\\");
            std::ifstream StreamMat(Obj.MaterialLib.c_str(), std::ios::in);

            if ( StreamMat.fail() ) return false;

            while ( !StreamMat.eof() ){

                StreamMat >> Line;

                if (Line == "newmtl") {

                    StreamMat >> Material.Name;
                    Duplicate = false;

                } else if (Line == "illum") {
                    StreamMat >> Material.Illumination;
                } else if (Line == "Kd") {
                    StreamMat >> Material.Diffuse.r >> Material.Diffuse.g >> Material.Diffuse.b;
                } else if (Line == "Ka") {
                    StreamMat >> Material.Ambient.r >> Material.Ambient.g >> Material.Ambient.b;
                } else if (Line == "Ks") {
                    StreamMat >> Material.Specular.r >> Material.Specular.g >> Material.Specular.b;
                } else if (Line == "Ke") {
                    StreamMat >> Material.Emissive.r >> Material.Emissive.g >> Material.Emissive.b;
                } else if (Line == "Ns") {
                    StreamMat >> Material.Shiny;
                } else if (Line == "map_Kd") {

                    for (unsigned int i = 0; i < Obj.Materials.size(); i++) {
                        if (Obj.Materials[i].Name == Material.Name) {
                            Duplicate = true;
                            continue;
                        }
                    }
                    StreamMat >> Material.DiffuseTexture;
                    if (Duplicate == false) {
                        Obj.Materials.push_back(Material);
                    }
                }

            }

            StreamMat.close();

        } else if ( Line == "v" ) {

            Vertex.Selected = false;
            Vertex.Visible = true;
            StreamObj >> Vertex.Coord.x >> Vertex.Coord.y >> Vertex.Coord.z;
            Obj.Vertices.push_back(Vertex);

        } else if ( Line == "vn" ) {

//            StreamObj >> Normal.x >> Normal.y >> Normal.z;
//            if (!std::isnan(Normal.x) && !std::isnan(Normal.y) && !std::isnan(Normal.z)) {
//                Obj.Normals.push_back(Normal);
//            }

        } else if ( Line == "vt" ) {

            StreamObj >> TexCoord.x >> TexCoord.y;
            Obj.TexCoords.push_back(TexCoord);

        } else if ( Line == "g" ) {

            StreamObj >> Group.Name;

            Group.Visible = true;
            Group.Selected = false;
            Group.MaterialIndex = -1;
            Obj.Groups.push_back(Group);

        } else if ( Line == "usemtl" ) {

            StreamObj >> Obj.Groups[Obj.Groups.size()-1].MaterialName;

            for (unsigned int j = 0; j < Obj.Materials.size(); j++) {
                if (StrCompare(Obj.Groups[Obj.Groups.size()-1].MaterialName, Obj.Materials[j].Name)) {
                    Obj.Groups[Obj.Groups.size()-1].MaterialIndex = j;
                    break;
                }
            }

        } else if ( Line == "f" ) {

            for ( int v = 0; v < 3; v++ ) {

                StreamObj >> Line;

                Triangle.Selected = false;
                Triangle.Visible = true;
                Triangle.Vertex[v] = Offsets[0] + atoi(Line.substr(0, Line.find_first_of("/")).c_str()) - 1;
                Triangle.Normal[v] = Offsets[1] + atoi(Line.substr(Line.find_last_of("/") + 1, Line.length()).c_str()) - 1;
                Triangle.TexCoord[v] = Offsets[2] + atoi(Line.substr(Line.find_first_of("/") + 1, Line.find_last_of("/") - 1 - Line.find_first_of("/") + 1).c_str()) - 1;

            }

            Triangle.GroupIndex = Obj.Groups.size() - 1;
            Obj.Triangles.push_back(Triangle);
        }

    }

    StreamObj.close();

    Obj.Textures.resize(Obj.Materials.size());
    for (unsigned int i = 0; i < Obj.Textures.size(); i++) {
        Obj.Textures[i].LoadImage(Path + "\\" + Obj.Materials[i].DiffuseTexture);
    }

    Dimensions = CalculateBounds(Min, Max, true);
    CalculateNormals();

    Update();

    sprintf(Config->General.LastFile, "%s", this->Filename.c_str());
    glfwSetWindowTitle(GLWindow, std::string(GetInfo() + " - " + this->Filename.substr(this->Filename.find_last_of("\\") + 1)).c_str());

//    Dirty = true;

    return true;

}

bool CModel::Save(bool SaveAs)
{

    if (StrCompare(Filename, "Untitled.Obj") || SaveAs) {
        std::string File = FileSave(NULL, (char*)("Obj Models\0*.Obj\0All Files\0*.*\0"), &std::string("")[0], &std::string(".Obj")[0]);
        if(File.size() != 0) {
            this->Filename = File;
        }
    }

    glfwSetWindowTitle(GLWindow, std::string(GetInfo() + " - " + Filename.substr(Filename.find_last_of("\\") + 1)).c_str());

    std::ofstream StreamObj(Filename.c_str(), std::ios::out | std::ios::trunc);

    if ( StreamObj.fail() ) return false;

    Obj.MaterialLib = Filename.substr(Filename.find_last_of("\\") + 1, Filename.size() - Filename.find_last_of("\\") - 4) + "mtl";
    StreamObj << "mtllib " << Obj.MaterialLib << std::endl;

    StreamObj << std::endl;
    for (unsigned int i = 0; i < Obj.Vertices.size(); i++) {
        StreamObj << "v " << Obj.Vertices[i].Coord.x << " " << Obj.Vertices[i].Coord.y << " " << Obj.Vertices[i].Coord.z << std::endl;
    }

    StreamObj << std::endl;
    for (unsigned int i = 0; i < Obj.Normals.size(); i++) {
        StreamObj << "vn " << Obj.Normals[i].x << " " << Obj.Normals[i].y << " " << Obj.Normals[i].z << std::endl;
    }

    StreamObj << std::endl;
    for (unsigned int i = 0; i < Obj.TexCoords.size(); i++) {
        StreamObj << "vt " << Obj.TexCoords[i].x << " " << Obj.TexCoords[i].y << std::endl;
    }

    StreamObj << std::endl;
    for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
        StreamObj << "g " << Obj.Groups[i].Name <<  std::endl;
        StreamObj << "usemtl " << Obj.Groups[i].MaterialName <<  std::endl;

        for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
            if (Obj.Triangles[j].GroupIndex == int(i)) {
            StreamObj << "f " << Obj.Triangles[j].Vertex[0] + 1 << "/" << Obj.Triangles[j].TexCoord[0] + 1 << "/" << Obj.Triangles[j].Normal[0] + 1 <<
                      " " << Obj.Triangles[j].Vertex[1] + 1 << "/" << Obj.Triangles[j].TexCoord[1] + 1 << "/" << Obj.Triangles[j].Normal[1] + 1 <<
                      " " << Obj.Triangles[j].Vertex[2] + 1 << "/" << Obj.Triangles[j].TexCoord[2] + 1 << "/" << Obj.Triangles[j].Normal[2] + 1 << std::endl;
            }
        }
    }

    StreamObj.close();

    std::ofstream StreamMtl(std::string(Filename.substr(0, Filename.find_last_of("\\") + 1) + Obj.MaterialLib).c_str(), std::ios::out | std::ios::trunc);

    if ( StreamMtl.fail() ) return false;

    for (unsigned int i = 0; i < Obj.Materials.size(); i++) {
        StreamMtl << "newmtl " << Obj.Materials[i].Name <<  std::endl;
        StreamMtl << "illum " << Obj.Materials[i].Illumination <<  std::endl;
        StreamMtl << "Kd " << Obj.Materials[i].Diffuse[0] << " " << Obj.Materials[i].Diffuse[1] << " " << Obj.Materials[i].Diffuse[2] << std::endl;
        StreamMtl << "Ka " << Obj.Materials[i].Ambient[0] << " " << Obj.Materials[i].Ambient[1] << " " << Obj.Materials[i].Ambient[2] << std::endl;
        StreamMtl << "Ks " << Obj.Materials[i].Specular[0] << " " << Obj.Materials[i].Specular[1] << " " << Obj.Materials[i].Specular[2] << std::endl;
        StreamMtl << "Ke " << Obj.Materials[i].Emissive[0] << " " << Obj.Materials[i].Emissive[1] << " " << Obj.Materials[i].Emissive[2] << std::endl;
        StreamMtl << "Ns " << Obj.Materials[i].Shiny <<  std::endl;
        StreamMtl << "map_Kd " << Obj.Materials[i].DiffuseTexture.substr(Obj.Materials[i].DiffuseTexture.find_last_of("\\") + 1) <<  std::endl;
        StreamMtl << std::endl;
    }

    StreamMtl.close();

    Dirty = false;

        sprintf(Config->General.LastFile, "%s", this->Filename.c_str());

    return true;
}

void CModel::Render(int Index, CProgram &ColorShader, bool Selection)
{
    glPointSize(4);
    glLineWidth(1.0f);

    if (Config->Viewports[Index].Projection == 0 || !Selection) {

        int Count = 0;
        glBindVertexArray(ModelVAO);
        for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
            if (Obj.Groups[i].MaterialIndex == -1) {
                glActiveTexture(0);
                glBindTexture(GL_TEXTURE_2D, 0);
            } else {
                Obj.Textures[Obj.Groups[i].MaterialIndex].Bind(0);
            }
            glDrawArrays(GL_TRIANGLES, Count, Offsets[i]);
            Count += Offsets[i];
        }

        glBindVertexArray(0);
    }

    if (Config->Viewports[Index].Projection != 0 && Selection) {
        ColorShader.Bind();
        glBindVertexArray(SelectVAO);
        glDrawArrays(GL_TRIANGLES, 0, Obj.Groups.size() * 3 + Obj.Triangles.size() * 3 + Obj.Vertices.size() * 3);

        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

        glDrawArrays(GL_TRIANGLES, Obj.Groups.size() * 3 + Obj.Triangles.size() * 3, Obj.Vertices.size() * 9);

        glBindVertexArray(0);
    }

}

void CModel::Update(bool Selection)
{

    for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
        Obj.Groups[i].Color = glm::vec4(float((i + 1) &0xFF) / 255.0f, float(((i + 1) >> 8) &0xFF) / 255.0f, float(((i + 1) >> 16) &0xFF) / 255.0f, 1.0f);
    }

    for (unsigned int i = 0; i < Obj.Triangles.size(); i++) {
        Obj.Triangles[i].Color = glm::vec4(float((i + 1) &0xFF) / 255.0f, float(((i + 1) >> 8) &0xFF) / 255.0f, float(((i + 1) >> 16) &0xFF) / 255.0f, 1.0f);
    }

    int Count = 0;
    Offsets.clear();

    SelectVBO.Create();

    for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
        for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
            if (Obj.Triangles[j].GroupIndex == int(i) && Obj.Groups[i].Visible) {
                for (unsigned int k = 0; k < 3; k++) {
                    if (Obj.Groups[i].Selected) {
                        SelectVBO.AddData(&Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord[0], sizeof(glm::vec3));
                        SelectVBO.AddData(&glm::u8vec3(1.0f, 0.0f, 0.0f)[0], sizeof(glm::u8vec3));
                    } else {
                        SelectVBO.AddData(&Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord[0], sizeof(glm::vec3));
                        SelectVBO.AddData(&glm::u8vec3(0)[0], sizeof(glm::u8vec3));
                    }
                }
            }
        }
    }

    for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
        if (Obj.Triangles[j].Selected && Obj.Triangles[j].Visible) {
            for (unsigned int k = 0; k < 3; k++) {
                if (Obj.Triangles[j].Selected) {
                    SelectVBO.AddData(&Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord[0], sizeof(glm::vec3));
                    SelectVBO.AddData(&glm::u8vec3(1.0f, 0.0f, 0.0f)[0], sizeof(glm::u8vec3));
                } else {
                    SelectVBO.AddData(&Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord[0], sizeof(glm::vec3));
                    SelectVBO.AddData(&glm::u8vec3(0)[0], sizeof(glm::u8vec3));
                }
            }
        }
    }

    for (unsigned int i = 0; i < Obj.Vertices.size(); i++) {
        if (Obj.Vertices[i].Visible) {
            for (unsigned int k = 0; k < 3; k++) {
                if (Obj.Vertices[i].Selected) {
                    SelectVBO.AddData(&Obj.Vertices[i].Coord[0], sizeof(glm::vec3));
                    SelectVBO.AddData(&glm::u8vec3(1.0f, 0.0f, 0.0f)[0], sizeof(glm::u8vec3));
                } else {
                    SelectVBO.AddData(&Obj.Vertices[i].Coord[0], sizeof(glm::vec3));
                    SelectVBO.AddData(&glm::u8vec3(0)[0], sizeof(glm::u8vec3));
                }
            }
        }
    }

    glGenVertexArrays(1, &SelectVAO);
    glBindVertexArray(SelectVAO);

    SelectVBO.Bind();
    SelectVBO.UploadData(GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::u8vec3), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::u8vec3), (void*)sizeof(glm::vec3));

    glBindVertexArray(0);

    if (Selection) return

    CalculateNormals();


    ModelVBO.Create();
    for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
        Count = 0;
        for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
            if (Obj.Triangles[j].GroupIndex == int(i) && Obj.Triangles[j].Visible) {
                Count += 3;
                for (unsigned int k = 0; k < 3; k++) {
                    ModelVBO.AddData(&Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord[0], sizeof(glm::vec3));
                    ModelVBO.AddData(&Obj.TexCoords[Obj.Triangles[j].TexCoord[k]][0], sizeof(glm::vec2));

                    if (Config->Viewports[Config->Editor.Viewport].RenderMode <= 1) {
                        ModelVBO.AddData(&Obj.FaceNormals[j][0], sizeof(glm::vec3));
                    } else {
                        ModelVBO.AddData(&Obj.Normals[Obj.Triangles[j].Normal[k]][0], sizeof(glm::vec3));
                    }

                    if (Config->Editor.Option == 0) {

                        ModelVBO.AddData(&glm::vec3(1.0f, 1.0f, 1.0f)[0], sizeof(glm::vec3));

                    } else if (Config->Editor.Option == 1) {

                        ModelVBO.AddData(&glm::vec3(Obj.Triangles[j].Color)[0], sizeof(glm::vec3));

                    } else if (Config->Editor.Option == 2) {

                        ModelVBO.AddData(&glm::vec3(Obj.Groups[i].Color)[0], sizeof(glm::vec3));
                    }
                }
            }
        }
        Offsets.push_back(Count);
    }

    glGenVertexArrays(1, &ModelVAO);
    glBindVertexArray(ModelVAO);

    ModelVBO.Bind();
    ModelVBO.UploadData(GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) + sizeof(glm::vec3) * 3, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) + sizeof(glm::vec3) * 3, (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) + sizeof(glm::vec3) * 3, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) + sizeof(glm::vec3) * 3, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(glm::vec3)));

    glBindVertexArray(0);

    Dirty = true;
}

void CModel::AddUndo(void)
{
    if (Undos.size() < 25) {
        if (Dirty) {
            CalculateNormals();
        }
        Undos.push_back(Obj);
    }
    Dirty = false;
    Update();
}

void CModel::Undo(void)
{
    if (Undos.size() > 0) {
        Redos.push_back(Obj);
        Obj = Undos[Undos.size() - 1];
        Undos.resize(Undos.size() - 1);
        for (unsigned int i = 0; i < Obj.Textures.size(); i++) {
            Obj.Textures[i].Refresh();
        }
    }
    Dirty = true;
    Update();
}

void CModel::Redo(void) {

    if (Redos.size() > 0) {
        Undos.push_back(Obj);
        Obj = Redos[Redos.size() - 1];
        Redos.resize(Redos.size() - 1);
        for (unsigned int i = 0; i < Obj.Textures.size(); i++) {
            Obj.Textures[i].Refresh();
        }
    }
    Dirty = true;
    Update();

}

glm::vec3 CModel::CalculateBounds(glm::vec3 &GetMin, glm::vec3 &GetMax, bool All)
{
    GetMin = glm::vec3(65536.0f);
    GetMax = glm::vec3(-65536.0f);
    Dimensions = glm::vec3(0.0f);
    Center = glm::vec3(0.0f);
    BoundingSphere = 0.0f;

    if (All) {

        for (unsigned int i = 0; i < Obj.Vertices.size(); i++) {

            Center += Obj.Vertices[i].Coord;
            if (Obj.Vertices[i].Coord.x < GetMin.x) {
                GetMin.x = Obj.Vertices[i].Coord.x;
            }
            if (Obj.Vertices[i].Coord.y < GetMin.y) {
                GetMin.y = Obj.Vertices[i].Coord.y;
            }
            if (Obj.Vertices[i].Coord.z < GetMin.z) {
                GetMin.z = Obj.Vertices[i].Coord.z;
            }
            if (Obj.Vertices[i].Coord.x > GetMax.x) {
                GetMax.x = Obj.Vertices[i].Coord.x;
            }
            if (Obj.Vertices[i].Coord.y > GetMax.y) {
                GetMax.y = Obj.Vertices[i].Coord.y;
            }
            if (Obj.Vertices[i].Coord.z > GetMax.z) {
                GetMax.z = Obj.Vertices[i].Coord.z;
            }
        }

    } else {

        if (Config->Editor.Option == 0) {

            for (unsigned int i = 0; i < Obj.Vertices.size(); i++) {
                if (Obj.Vertices[i].Selected && Obj.Vertices[i].Visible) {
                    if (Obj.Vertices[i].Coord.x < GetMin.x) {
                        GetMin.x = Obj.Vertices[i].Coord.x;
                    }
                    if (Obj.Vertices[i].Coord.y < GetMin.y) {
                        GetMin.y = Obj.Vertices[i].Coord.y;
                    }
                    if (Obj.Vertices[i].Coord.z < GetMin.z) {
                        GetMin.z = Obj.Vertices[i].Coord.z;
                    }
                    if (Obj.Vertices[i].Coord.x > GetMax.x) {
                        GetMax.x = Obj.Vertices[i].Coord.x;
                    }
                    if (Obj.Vertices[i].Coord.y > GetMax.y) {
                        GetMax.y = Obj.Vertices[i].Coord.y;
                    }
                    if (Obj.Vertices[i].Coord.z > GetMax.z) {
                        GetMax.z = Obj.Vertices[i].Coord.z;
                    }
                }
            }

        } else if (Config->Editor.Option == 1) {

            for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
                if (Obj.Triangles[j].Selected && Obj.Triangles[j].Visible) {
                    for (int k = 0; k < 3; k++) {
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x < GetMin.x) {
                            GetMin.x = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x;
                        }
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y < GetMin.y) {
                            GetMin.y = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y;
                        }
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z < GetMin.z) {
                            GetMin.z = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z;
                        }
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x > GetMax.x) {
                            GetMax.x = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x;
                        }
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y > GetMax.y) {
                            GetMax.y = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y;
                        }
                        if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z > GetMax.z) {
                            GetMax.z = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z;
                        }
                    }
                }
            }

        } else if (Config->Editor.Option == 2) {

            for (unsigned int i = 0; i < Obj.Groups.size(); i++) {
                if (Obj.Groups[i].Selected && Obj.Groups[i].Visible) {
                    for (unsigned int j = 0; j < Obj.Triangles.size(); j++) {
                        if (Obj.Triangles[j].Selected && Obj.Triangles[j].Visible && Obj.Triangles[j].GroupIndex == int(i)) {
                            for (int k = 0; k < 3; k++) {
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x < GetMin.x) {
                                    GetMin.x = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x;
                                }
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y < GetMin.y) {
                                    GetMin.y = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y;
                                }
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z < GetMin.z) {
                                    GetMin.z = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z;
                                }
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x > GetMax.x) {
                                    GetMax.x = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.x;
                                }
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y > GetMax.y) {
                                    GetMax.y = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.y;
                                }
                                if (Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z > GetMax.z) {
                                    GetMax.z = Obj.Vertices[Obj.Triangles[j].Vertex[k]].Coord.z;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Center /= Obj.Vertices.size();
    Dimensions = (GetMax - GetMin);

    if (Dimensions.x > BoundingSphere) BoundingSphere = Dimensions.x;
    if (Dimensions.y > BoundingSphere) BoundingSphere = Dimensions.y;
    if (Dimensions.z > BoundingSphere) BoundingSphere = Dimensions.z;

    return Dimensions;

}

void CModel::CalculateNormals(void)
{
    glm::vec3 N, A, B;
    std::vector<int> SharedFaces;


    Obj.Normals.resize(Obj.Vertices.size());
    SharedFaces.resize(Obj.Vertices.size());

    for(unsigned int i = 0; i < Obj.Groups.size(); i++) {
        for(unsigned int j = 0; j < Obj.Triangles.size(); j++) {
            if (Obj.Triangles[j].GroupIndex == int(i)) {

                A = Obj.Vertices[Obj.Triangles[j].Vertex[2]].Coord - Obj.Vertices[Obj.Triangles[j].Vertex[0]].Coord;
                B = Obj.Vertices[Obj.Triangles[j].Vertex[1]].Coord - Obj.Vertices[Obj.Triangles[j].Vertex[0]].Coord;

                N = glm::normalize(glm::cross(B, A));

                Obj.FaceNormals.push_back(N);

                Obj.Normals[Obj.Triangles[j].Vertex[0]] += N;
                Obj.Normals[Obj.Triangles[j].Vertex[1]] += N;
                Obj.Normals[Obj.Triangles[j].Vertex[2]] += N;

                SharedFaces[Obj.Triangles[j].Vertex[0]]++;
                SharedFaces[Obj.Triangles[j].Vertex[1]]++;
                SharedFaces[Obj.Triangles[j].Vertex[2]]++;
            }
        }
    }

    for(unsigned int i = 0; i < Obj.Normals.size(); i++) {
        Obj.Normals[i] /= SharedFaces[i];
        Obj.Normals[i] = glm::normalize(Obj.Normals[i]);
    }

}
