#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct Attribute {
	unsigned int type;
	unsigned int count;
	unsigned long long size;
	
};

class BufferLayout
{
private:
	std::vector<Attribute> attribVector;
	unsigned int stride;


public:
	BufferLayout();

	
	unsigned int getStride() const;
	std::vector<Attribute> getAttributes() const;
	int getNumOfAttr();

	template <typename T>
	void addAttribute(unsigned int count) 
	{
		static_assert(false);
	}

	template<>
	void addAttribute<float>(unsigned int count)
	{
		attribVector.push_back({ GL_FLOAT, count, (count * sizeof(float)) });
		stride += count * sizeof(float);
	}

	template<>
	void addAttribute<int>(unsigned int count)
	{
		attribVector.push_back({ GL_INT, count, (count * sizeof(int)) });
		stride += count * sizeof(int);
	}

	



};

