#include "VertexBuffer.h"
//This Class was made with help from:
//The Cherno opengl youtube series
VertexBuffer::VertexBuffer()
{
}


VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &id);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

//VertexBuffer::~VertexBuffer()
//{
//    glDeleteBuffers(1, &id);
//}

void VertexBuffer::init()
{
    glGenBuffers(1, &id);
}

void VertexBuffer::addData(const void* data, unsigned int size)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}


void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}
