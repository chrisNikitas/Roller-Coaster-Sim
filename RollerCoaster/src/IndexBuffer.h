#pragma once
#include <GL/glew.h>

class IndexBuffer {      
private:            
    unsigned int c_renderer_id;
    unsigned int c_count;
public:
    IndexBuffer();
    IndexBuffer(const unsigned int* data, unsigned int count);
    void init();
    void addData(const unsigned int* data, unsigned int count);
    void bind();
    void unbind();
    unsigned int getCount();
};