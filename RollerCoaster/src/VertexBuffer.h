#pragma once
#include <GL/glew.h>

class VertexBuffer {      
private:            
    unsigned int id;         
public:
    VertexBuffer();
    VertexBuffer(const void* data, unsigned int size);

    void init();
    void addData(const void* data, unsigned int size);
    void bind() const;
    void unbind() const;

};