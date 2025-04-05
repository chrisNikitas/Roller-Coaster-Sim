#pragma once
#include <GL/glew.h>
#include "VertexBuffer.h"
#include "BufferLayout.h"

class VertexArray {
private:
    unsigned int id;
public:
    VertexArray();
    //~VertexArray();
    void init();

    void addBufferWithLayout( VertexBuffer const &vb, BufferLayout const &bl);

    void bind();
    void unbind();
    unsigned int getCount();
};