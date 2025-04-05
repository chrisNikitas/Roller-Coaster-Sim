#include "IndexBuffer.h"
//This Class was made with help from:
//The Cherno opengl youtube series
IndexBuffer::IndexBuffer()
{
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
{
    c_count = count;
    glGenBuffers(1, &c_renderer_id);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

//IndexBuffer::~IndexBuffer()
//{
//    glDeleteBuffers(1, &c_renderer_id);
//}

void IndexBuffer::init()
{
    glGenBuffers(1, &c_renderer_id);
}

void IndexBuffer::addData(const unsigned int* data, unsigned int count)
{
    c_count = count;
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}


void IndexBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c_renderer_id);
}

void IndexBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

unsigned int IndexBuffer::getCount()
{
    return c_count;
}
