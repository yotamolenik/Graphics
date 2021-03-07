/*
 * VertexBuffer.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: Casa
 */
#define GLEW_STATIC
#include "GL/glew.h"
#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
	glGenBuffers(1,&m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER,size, data, GL_STATIC_DRAW);
}


VertexBuffer::VertexBuffer(const VertexBuffer &vb) {
	int size;
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_COPY_READ_BUFFER, vb.m_RendererID);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);

    glBindBuffer(GL_COPY_WRITE_BUFFER, m_RendererID);
    glBufferData(GL_COPY_WRITE_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::copy()
{
	int size;
	glBindBuffer(GL_COPY_READ_BUFFER, m_RendererID);
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size);
   
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1,&m_RendererID);
}

void VertexBuffer::Bind()const {
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind()const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
