#pragma once
#include <GL/glew.h>
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();//宏命令，当执行到错误程序时产生中断，__debugbreak()为中断指令
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x,__FILE__,__LINE__))//#x把函数转化为string

//清空错误显示
void GLClearError();

//获取当前错误
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
public:
    void Clear()const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)const;
};