/*
 * VertexBuffer.hpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: Casa
 */

#ifndef SRC_VERTEXBUFFER_HPP_
#define SRC_VERTEXBUFFER_HPP_

class VertexBuffer{
private:

    unsigned int m_RendererID;
	bool isDynamic;
public:
	VertexBuffer(const VertexBuffer &vb);
	VertexBuffer(const void* data, unsigned int size,bool dynamic = false);
	~VertexBuffer();
	
	void ChageData(const void* data, unsigned int size);
	void Bind() const;
	void Unbind() const;
	void copy();

    void Init(const void *data, unsigned int size, bool dynamic);

    VertexBuffer();
};







#endif /* SRC_VERTEXBUFFER_HPP_ */
