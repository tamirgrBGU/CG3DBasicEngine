#ifndef SRC_VERTEXARRAY_HPP_
#define SRC_VERTEXARRAY_HPP_
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

class VertexArray{
private:
	unsigned int m_RendererID;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb,int attribNum,int count,int type);

	void Bind() const;
	void Unbind() const;
};


#endif
