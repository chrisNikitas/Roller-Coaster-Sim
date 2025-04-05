#include "Mesh.h"
//This Class was made with help from:
//https://learnopengl.com/Model-Loading/Model

    Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
        setupMesh();
    }

    // render the mesh
    void Mesh::Draw(Shader& shader)
    {
        // draw mesh
        VAO.bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        VAO.unbind();
    }

 
    void Mesh::setupMesh()
    {   
        VBO.init();
        IBO.init();
        VAO.init();

        VAO.bind();
        VBO.addData(&vertices[0], vertices.size() * sizeof(Vertex));
        IBO.addData(&indices[0], indices.size());

        BufferLayout bl;
        bl.addAttribute<float>(3);
        bl.addAttribute<float>(3);
        bl.addAttribute<float>(3);
        VAO.addBufferWithLayout(VBO, bl);

        VAO.unbind();
    }