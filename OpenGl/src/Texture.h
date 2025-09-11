#pragma once
#include "Renderer.h"
#include <string>
class Texture {
private:
	unsigned int m_RendererID;
	const std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path="",unsigned int slot = 0,int width = 0, int height = 0);
	~Texture();
	void Updata(const std::string& path);
	void Bind() const;//wen bind the texture to a specific slot
	void Unbind() const;
	void FrameBind() const;
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};