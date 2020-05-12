
#include "VBO.hpp"

CVBO::CVBO(void) : Buffer(0)
{
}

CVBO::~CVBO(void)
{
    Destroy();
}

void CVBO::Create(int DefaultSize)
{
    Destroy();

    glGenBuffers(1, &Buffer);
    Data.reserve(DefaultSize);
    Size = DefaultSize;
    CurrentSize = 0;
}

bool CVBO::Destroy(void)
{
    if (glIsBuffer(Buffer)) {
        glDeleteBuffers(1, &Buffer);
        Data.clear();
    }
    return !glIsBuffer(Buffer);
}

void *CVBO::MapBufferToMemory(GLint UsageHint)
{
    void *PtrRes = glMapBuffer(Type, UsageHint);
    return PtrRes;
}

void *CVBO::MapSubBufferToMemory(GLint UsageHint, GLuint Offset, GLuint Length)
{
    void *PtrRes = glMapBufferRange(Type, Offset, Length, UsageHint);
    return PtrRes;
}

void CVBO::UnmapBuffer(void)
{
    glUnmapBuffer(Type);
}

void CVBO::Bind(GLint BufferType)
{
    Type = BufferType;
    glBindBuffer(Type, Buffer);
}

void CVBO::UploadData(GLint DrawingHint, bool Update)
{
    if (Update == false) {
        glBufferData(Type, Data.size(), &Data[0], DrawingHint);
    } else {
        glBufferSubData(Type, 0, Data.size(), &Data[0]);
    }
    Data.clear();
}

void CVBO::AddData(const void *PtrData, GLuint DataSize)
{
    Data.insert(Data.end(), (char*)PtrData, (char*)PtrData + DataSize);
    CurrentSize += DataSize;
}

void *CVBO::GetDataPointer(void)
{
    return (void*) &Data[0];
}
