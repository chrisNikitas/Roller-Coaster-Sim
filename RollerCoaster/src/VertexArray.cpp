#include "VertexArray.h"
//This Class was made with help from:
//The Cherno opengl youtube series
VertexArray::VertexArray()
{

}

void VertexArray::init() {
	glGenVertexArrays(1, &id);
}

//VertexArray::~VertexArray()
//{
//	glDeleteVertexArrays(1, &id);
//}

void VertexArray::addBufferWithLayout(const VertexBuffer& vb, const BufferLayout& bl)
{
	unsigned int index = 0, offset = 0;
	bind();
	vb.bind();
	std::vector<Attribute> attributes = bl.getAttributes();

	for (Attribute attribute : attributes) {
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		glVertexAttribPointer(index, attribute.count, attribute.type, GL_FALSE, bl.getStride(), (void *)(offset));
		glEnableVertexAttribArray(index);
		offset += attribute.size;
		index++;
	}

	
}

void VertexArray::bind()
{
	glBindVertexArray(id);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}

unsigned int VertexArray::getCount()
{
	return 0;
}
