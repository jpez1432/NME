
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>
#include <Soil2/Soil2.h>

#include "../Config.hpp"

extern CConfig *Config;

class CTexture {

private:

    GLuint GLTexture;
    std::string Path;
    std::string Filename;

public:

    CTexture(void);
    ~CTexture(void);

    bool LoadBlank(GLuint Dimensions, GLuint Color, GLenum Format);
    bool LoadImage(std::string Filename, std::string AlternateDir = "");
    bool LoadMemory(GLubyte *Data, GLuint Dimensions, GLuint Bpp, GLenum Format);

    void Destroy(void);

    void Refresh(bool Reload = false);

    void SetParameters(GLint MinFilter, GLint MagFilter, GLint WrapS, GLint WrapT);

    void Bind(GLuint TextureUnit);
    void Unbind(void) { glBindTexture(GL_TEXTURE_2D, 0); }
    GLuint &GetTexture(void) { return GLTexture; }
    std::string &GetFilename(void) { return Filename; }
};

#endif
