#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const void* data, unsigned int size);
	~IndexBuffer();

	void Bind()const;
	void unbind()const;
	inline unsigned int GetCount() const { return m_Count; };
};