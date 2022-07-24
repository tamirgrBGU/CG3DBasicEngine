/*
 * VertexArray.cpp

 *
 *  Created on: 22 de dez de 2017
 *      Author: Casa
 */
#define GLEW_STATIC
#include "VertexArray.hpp"
#include "../gl.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1,&m_RendererID);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1,&m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb,int attribNum,int count, int type)
{
		//vb.Bind();
		glEnableVertexAttribArray(attribNum);
		glVertexAttribPointer(attribNum,count,type,GL_FALSE, 0,0);
}

void VertexArray::Bind() const {
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

