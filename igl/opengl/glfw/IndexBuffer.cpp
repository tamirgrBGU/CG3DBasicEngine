/*
 * IndexBuffer.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: Casa
 */
#define GLEW_STATIC
#include "../gl.h"
#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	:m_Count(count){

	glGenBuffers(1,&m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::IndexBuffer(const IndexBuffer &ib) {
	
	int size;
	glGenBuffers(1, &m_RendererID);
	
	glBindBuffer(GL_COPY_READ_BUFFER, ib.m_RendererID);
	glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);

	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ELEMENT_ARRAY_BUFFER, 0, 0, size);

}

IndexBuffer::~IndexBuffer() {
	glDeleteBuffers(1,&m_RendererID);
}

void IndexBuffer::Bind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::Unbind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::IndexBuffer():m_Count(0) {

}

void IndexBuffer::Init(const unsigned int *data, unsigned int count) {
    m_Count = count;
    glGenBuffers(1,&m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    if(data && count > 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void IndexBuffer::ChageData(const void* data, unsigned int count)
{
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*count, data, GL_DYNAMIC_DRAW);
}