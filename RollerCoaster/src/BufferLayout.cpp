#include "BufferLayout.h"
//This Class was made with help from:
//The Cherno opengl youtube series

BufferLayout::BufferLayout()
{
	stride = 0;
}


unsigned int BufferLayout::getStride() const
{
	return stride;
}

std::vector<Attribute> BufferLayout::getAttributes() const
{
	return attribVector;
}

int BufferLayout::getNumOfAttr()
{
	return attribVector.size();
}


