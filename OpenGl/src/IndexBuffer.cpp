#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_RendererID);//生成缓冲区的名称
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);//绑定缓冲区
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);// 创建并初始化缓冲区对象的数据存储
    m_Count = size / sizeof(unsigned int);//计算索引数量
}
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind()const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));//绑定缓冲区
}
void IndexBuffer::unbind()const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}