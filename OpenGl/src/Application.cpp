#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
#include <filesystem>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Tests/TestClearColor.h"
#include <chrono>

void FlipImageVertically(unsigned char* pixels, int width, int height) {
    int rowSize = width * 4; // 每行的字节数（4 字节 / 像素）

    // 临时缓冲区用于交换每行
    unsigned char* rowBuffer = new unsigned char[rowSize];

    // 反转图像的每一行
    for (int y = 0; y < height / 2; ++y) {
        // 计算当前行和对应的反转行的地址
        unsigned char* topRow = pixels + y * rowSize;
        unsigned char* bottomRow = pixels + (height - 1 - y) * rowSize;

        // 交换这两行
        memcpy(rowBuffer, topRow, rowSize);        // 将当前行复制到临时缓冲区
        memcpy(topRow, bottomRow, rowSize);       // 将反转行复制到当前行
        memcpy(bottomRow, rowBuffer, rowSize);    // 将临时缓冲区的行复制到反转行
    }

    delete[] rowBuffer; // 释放缓冲区内存
}
using namespace std;
namespace fs = filesystem;
int main() {
    auto beforeTime = std::chrono::steady_clock::now();
    float tmp = (sqrt(5.0f) + 1.0f) / 2.0f;
 
    //float length = 8.75f * 0.254f;
    const char* save_path = "output.png";
	string save_path_result = "output/output2.png";
    vector<string> textureNames ;
    string folderPath = "Texture";
    for (const auto& entry : filesystem::directory_iterator(folderPath))
    {
        // 只处理以 .png 结尾的文件
        if (entry.path().extension() == ".png")
        {
            textureNames.push_back(entry.path().string());  // 存储完整路径
        }
    }
    const char* color_path = "Texture/5#5000.png";
    //dimention in meters
    float L = tmp*2.0f;
    float M = L; 
    const int WIDTH = 1000;
    const int HEIGHT =1000;
    float k = 15.0f;
    float t = 11.187000f;
    int n_range = 16;
    int m_range = 16;
    float a0 = 0.434f;
    float a2 = 0.434f;
    float s0 =100.0f;
    float I = 4.0f;
    int type = 0;
    int currentTextureIndex = 0;
    int isError = 0;
    vector<string> typeList = {"Is","Bs","Ds a0=a2",
        "Rs  A=0 C=0 N!=M","Is + Is","Bs + Bs","Rs + Ds  A=0 C=0",
        "Bs + Is A!=C","Is+Ds A=C N=M","Bs + Bs + Ds A=C"};
    vector<string> errorList = {"No Error","Error"};

    color_path = textureNames[currentTextureIndex].c_str();
    
    if (!glfwInit()) {

        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pixel Drawing with Color Condition", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    // 初始化 GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    {
        float positions[] = {
            // positions   // texture
               -1.0,-1.0, 0.0f, 0.0f,//0
                1.0,-1.0, 1.0f, 0.0f,//1
                1.0, 1.0, 1.0f, 1.0f,//2
               -1.0, 1.0, 0.0f, 1.0f //3

        };//build buffer
        unsigned int indices[] = {
            0,1,2,
            0,3,2,
        };//设置索引缓冲区
        //顶点缓冲区
        
        VertexArray vao;
        VertexBuffer vb(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        vao.AddBuffer(vb, layout);
        IndexBuffer ib(indices, sizeof(indices));
        Shader shader("res/shader/Basic.shader");
        shader.Bind();
        Shader secondshader("res/shader/Second.shader");

        // 设置帧缓冲和纹理
        unsigned int framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        Texture textureColorBuffer("", 0, WIDTH, HEIGHT);

        secondshader.Bind();
        secondshader.SetUniform1i("renderedTexture", 0);
        //加载颜色纹理，已绑定

        Texture texture(color_path,1);
        texture.Bind();

        shader.Bind();
        shader.SetUniform1i("texture1", 1);

        //创建投影矩阵
        glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);//正交矩阵
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));//视图矩阵


        shader.Unbind();
        vao.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();

        // 需要指定GLSL版本, 也就是shader中的version
        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        glm::vec3 translationA(0.0f, 0.0f, 0.0f);
        test::TestClearColor test;
        bool flag = false;
        bool isDraw = false;
        
        while (!glfwWindowShouldClose(window)) {


            if (!flag) {
                //从这里开始把图像渲染到纹理，相当于一个渲染场景
                textureColorBuffer.FrameBind();
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                vao.Bind();
                shader.Bind();
                shader.SetUniform1f("k", k);
                shader.SetUniform1f("t", t);
                shader.SetUniform1f("L", L);
                shader.SetUniform1i("n_range", n_range);
                shader.SetUniform1i("m_range", m_range);
                shader.SetUniform1f("a0", a0);
                shader.SetUniform1f("a2", a2);
                shader.SetUniform1f("s0", s0);
                shader.SetUniform1f("I", I);
                shader.SetUniform1i("type", type);
                shader.SetUniform1i("isError", isError);

                renderer.Draw(vao, ib, shader);
                auto afterTime = std::chrono::steady_clock::now();

                std::cout << "总耗时:" << std::endl;
                //秒
                double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
                std::cout << duration_second << "秒" << std::endl;

                // 读取帧缓冲的内容并保存为图片
                unsigned char* pixels = new unsigned char[WIDTH * HEIGHT * 4];
                glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                stbi_write_png(save_path, WIDTH, HEIGHT, 4, pixels, WIDTH * 4);
                if (isDraw) {
					save_path_result = "output/T=" + to_string(t) + " K=" + to_string(k) + " L=" + to_string(L) + " I=" + to_string(I) +
                        " N="+to_string(n_range)+" M="+to_string(m_range)+" a0="+to_string(a0)+" a2="+to_string(a2)+"   "+errorList[isError] + ".png";
                    FlipImageVertically(pixels, WIDTH, HEIGHT);
                    stbi_write_png(save_path_result.c_str(), WIDTH, HEIGHT, 4, pixels, WIDTH * 4);
                    isDraw = false;
                }
                delete[] pixels;
                // 恢复默认帧缓冲
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                flag = true;
                
            }

            // 绘制纹理
            renderer.Clear();
            test.OnUpdate(0.0f);
            test.OnRender();
            ImGui_ImplOpenGL3_NewFrame();//渲染ImGUI帧缓冲
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            secondshader.Bind();
            {
                
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);//模型矩阵
                glm::mat4 mvp = proj * view * model;//模型视图矩阵
                secondshader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(vao, ib, secondshader);
            }

            {
            
                ImGui::Begin("Picture");                 

                test.OnImGuiRender();
                ImGui::SliderFloat3("TranslationA", &translationA.x, -1.0f, 1.0f);

                if (ImGui::SliderFloat("value K", &k, 0.5f, 25.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("value T", &t, 0.0f, 50.0f)) {
                    flag = false;
                }

                if (ImGui::SliderInt("value N", &n_range, 1, 50)) {
                    flag = false;
                }

                if (ImGui::SliderInt("value M", &m_range, 1, 50)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("value a0", &a0, 0.0f, 5.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("value a2", &a2, 0.0f, 5.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("value s0", &s0, 10.0f, 10000.0f)) {
                    flag = false;
                }
                
                if (ImGui::SliderFloat("value L", &L, 0.0f, 5.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("value I", &I, 0.0f, 350.0f)) {
                    flag = false;
                }

                if (ImGui::SliderInt(("type:" + typeList[type]).c_str(), &type, 0, 9)) {
                    flag = false;
                }

                if (ImGui::SliderInt("current texture", &currentTextureIndex, 0, textureNames.size() - 1)) {
                    flag = false;
                    texture.Updata(textureNames[currentTextureIndex]);
                    texture.Bind();
                    shader.Bind();
                    shader.SetUniform1i("texture1", 1);
                }
                
                if (ImGui::Button(errorList[isError].c_str())) {
                    flag = false;
                    isError = 1-isError;
                }
                if (ImGui::Button("Save")) {
                    flag = false;
                    isDraw = true;
                }
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;
}