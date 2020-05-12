
#ifndef VBO_HPP
#define VBO_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

#include <vector>

class CVBO {

private:

    GLint Size;
    GLint CurrentSize;
    GLint Type;
    GLuint Buffer;
    bool Uploaded;

    std::vector<GLubyte>Data;

public:

    CVBO(void);
    ~CVBO(void);

    void Create(int DefaultSize = 0);
    bool Destroy(void);

    void *MapBufferToMemory(GLint UsageHint);
    void *MapSubBufferToMemory(GLint UsageHint, GLuint Offset, GLuint Length);
    void UnmapBuffer(void);

    void AddData(const void *PtrData, GLuint DataSize);
    void UploadData(int DrawingHint, bool Update = false);

    void Bind(GLint BufferType = GL_ARRAY_BUFFER);

    void *GetDataPointer(void);
    GLuint GetBufferID(void) { return Buffer; }
    int GetCurrentSize(void) { return CurrentSize; }

};

#endif
