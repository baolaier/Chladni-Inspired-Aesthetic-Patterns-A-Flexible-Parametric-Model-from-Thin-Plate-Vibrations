#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_RendererID);//生成缓冲区的名称
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);//绑定缓冲区
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);// 创建并初始化缓冲区对象的数据存储
}
VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind()const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));//绑定缓冲区
}
void VertexBuffer::unbind()const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}