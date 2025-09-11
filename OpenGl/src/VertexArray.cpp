#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));//生成顶点数组对象

}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));//删除顶点数组对象
}

void VertexArray::AddBuffer(VertexBuffer& vb, const VertexBufferLayout& layout) {
	Bind();//绑定顶点数组对象
	vb.Bind();//绑定顶点缓冲区对象
	const auto& elements = layout.GetElements();//获取元素
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];//获取每个元素
		GLCall(glEnableVertexAttribArray(i));//启用通用顶点属性数组，其中i为对象索引
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized ,
			layout.GetStride(), (const void*)offset));//定义通用顶点属性数据的数组
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type); //计算偏移量
	}

}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));//绑定顶点数组对象
}
void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));//解绑顶点数组对象
}