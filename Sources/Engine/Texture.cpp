
#include "Texture.hpp"

CTexture::CTexture(void) : GLTexture(0)
{

}

CTexture::~CTexture(void)
{
    Destroy();
}

bool CTexture::LoadBlank(GLuint Dimensions, GLuint Color, GLenum Format)
{
    std::vector<GLubyte>Data(Dimensions * Dimensions * 4, Color);

    glGenTextures(1, &GLTexture);
    glBindTexture(GL_TEXTURE_2D, GLTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, Format, Dimensions, Dimensions, 0, Format, GL_UNSIGNED_BYTE, &Data[0]);

    Data.clear();

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, &Data[0]);

    if(glIsTexture(GLTexture)) {
        Logger->Text("Created New Blank Texture");
    } else {
        Logger->Error("Error Creating New Blank Texture");
    }

    return glIsTexture(GLTexture);

}

bool CTexture::LoadImage(std::string Filename, std::string AlternateDir)
{
    this->Path = Filename.substr(0, Filename.find_last_of("\\") + 1).c_str();
    this->Filename = Filename.substr(Filename.find_last_of("\\") + 1).c_str();

    if(AlternateDir != "") {
        Filename.insert(0, RootDir + ResourceDir + AlternateDir);
    }

    if(Config->Graphics.TextureQuality == 2) {
        GLTexture = SOIL_load_OGL_texture(Filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    } else {
        GLTexture = SOIL_load_OGL_texture(Filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    }

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    if(glIsTexture(GLTexture)) {
        Logger->Text("Loaded Texture - " + Filename);
    } else {
        Logger->Error("Error Loading Texture - " + Filename, true, false, false);
    }

    return glIsTexture(GLTexture);
}

bool CTexture::LoadMemory(GLubyte *Data, GLuint Dimensions, GLuint Bpp, GLenum Format)
{
    glGenTextures(1, &GLTexture);
    glBindTexture(GL_TEXTURE_2D, GLTexture);

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, &Data[0]);

    if(glIsTexture(GLTexture)) {
        Logger->Text("Loaded Texture From Memory");
    } else {
        Logger->Error("Error Loading Texture From Memory");
    }

    return glIsTexture(GLTexture);

}

void CTexture::Destroy(void)
{
    if(glIsTexture(GLTexture)) {
        glDeleteTextures(1, &GLTexture);
        Logger->Text("Released Texture Memory - " + Filename);
    }
}

void CTexture::Refresh(bool Reload)
{
    Destroy();
    if (!glIsTexture(GLTexture) || Reload) {
        LoadImage(Path + Filename);
    }
}

void CTexture::Bind(GLuint TextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D, GLTexture);
}

void CTexture::SetParameters(GLint MinFilter, GLint MagFilter, GLint WrapS, GLint WrapT)
{
    if(glIsTexture(GLTexture)) {
        glBindTexture(GL_TEXTURE_2D, GLTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
    }
}
