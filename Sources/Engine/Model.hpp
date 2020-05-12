
#ifndef MODEL_HPP
#define MODEL_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

#include "../Common.hpp"
#include "Texture.hpp"
#include "VBO.hpp"
#include "Shaders.hpp"

extern GLFWwindow *GLWindow;
extern CConfig::Rect Viewports[4];

class CModel {

    public:

        struct Vertex {
            glm::vec3 Coord;
            bool Selected;
            bool Visible;
        };

        struct Triangle {
            int GroupIndex;
            glm::uvec3 Vertex;
            glm::uvec3 Normal;
            glm::uvec3 TexCoord;
            glm::vec4 Color;
            bool Selected;
            bool Visible;
        };

        struct Material {
            std::string Name;
            unsigned int Illumination;
            glm::vec3 Diffuse;
            glm::vec3 Ambient;
            glm::vec3 Specular;
            glm::vec3 Emissive;
            float Shiny;
            std::string DiffuseTexture;
        };

        struct Group {
            std::string Name;
            std::string MaterialName;
            int MaterialIndex;
            bool Selected;
            bool Visible;
            glm::vec4 Color;
        };

        struct Model {
            std::vector<Vertex>Vertices;
            std::vector<Triangle>Triangles;
            std::vector<glm::vec3>FaceNormals;
            std::vector<glm::vec3>Normals;
            std::vector<glm::vec2>TexCoords;
            std::vector<Group>Groups;
            std::vector<Material>Materials;
            std::vector<CTexture>Textures;
            std::string MaterialLib;
        };

        GLuint ModelVAO;
        CVBO ModelVBO;
        std::vector<unsigned int> Offsets;

        GLuint SelectVAO;
        CVBO SelectVBO;

        glm::vec3 Center;
        glm::vec3 Min, Max;
        glm::vec3 Dimensions;
        float BoundingSphere;

        Model Obj;

        std::vector<Model> Undos;
        std::vector<Model> Redos;

        std::string Filename;

    public:

        CModel(void);
        ~CModel(void);

        bool New(bool ClearEdits = true);
        bool Load(bool Merge = false, std::string Filename = "");
        bool Save(bool SaveAs = true);

        void Render(int Index, CProgram &ColorShader, bool Selection = true);
        void Update(bool Selection = false);

        void AddUndo(void);
        void Undo(void);
        void Redo(void);

        void CalculateNormals(void);
        glm::vec3 CalculateBounds(glm::vec3 &GetMin, glm::vec3 &GetMax, bool All = true);
        float GetBoundingSphere(void) { return BoundingSphere; }

        std::string &GetFilename(void) { return Filename; }
};

#endif

