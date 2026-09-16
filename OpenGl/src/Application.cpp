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
#include "./stb_image/stb_image.h"
#include "./stb_image/stb_image_write.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Parameters.h"
#include <chrono>

void FlipImageVertically(unsigned char* pixels, int width, int height) {
    int rowSize = width * 4;
    unsigned char* rowBuffer = new unsigned char[rowSize];
    for (int y = 0; y < height / 2; ++y) {
        unsigned char* topRow = pixels + y * rowSize;
        unsigned char* bottomRow = pixels + (height - 1 - y) * rowSize;
        memcpy(rowBuffer, topRow, rowSize);
        memcpy(topRow, bottomRow, rowSize);
        memcpy(bottomRow, rowBuffer, rowSize);
    }
    delete[] rowBuffer;
}
int main(int argc, char* argv[]) {
    auto beforeTime = std::chrono::steady_clock::now();
    bool openui = true;
    Parameters params;
    if (argc != 1) {
        if (argc == 5 && std::string(argv[1]) == "--cli") {
            params.type = std::stoi(argv[2]);
            params.color_path = argv[3];
            params.file_dir = argv[4];
            openui = false;
        }
        else {
            std::cerr << "Error: invalid arguments." << std::endl;
            std::cerr << "Usage: " << argv[0] << " [--cli type texture_path save_dir]" << std::endl;
            return -1;
        }
    }
    
    const char* save_path = "output.png";
	std::string save_path_result = "output/output2.png";
    std::vector<std::string> textureNames ;
    std::string folderPath = "Texture";
    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (entry.path().extension() == ".png")
        {
            textureNames.push_back(entry.path().string());
        }
    }
    std::vector<std::string> typeList = {"Is","Bs","Ds",
        "Rs","Is + Is","Bs + Bs","Rs + Ds",
        "Bs + Is","Is + Ds","Bs + Bs + Ds"};
    std::vector<std::string> errorList = {"Without pert","With pert"};
    
    if (!glfwInit()) {

        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    if(!openui)glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(params.WIDTH, params.HEIGHT, "Pixel Drawing with Color Condition", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    {
        float positions[] = {
               -1.0,-1.0, 0.0f, 0.0f,//0
                1.0,-1.0, 1.0f, 0.0f,//1
                1.0, 1.0, 1.0f, 1.0f,//2
               -1.0, 1.0, 0.0f, 1.0f //3

        };//build buffer
        unsigned int indices[] = {
            0,1,2,
            0,3,2,
        };
        
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

        unsigned int framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        Texture textureColorBuffer("", 0, params.WIDTH, params.HEIGHT);

        secondshader.Bind();
        secondshader.SetUniform1i("renderedTexture", 0);

        Texture texture(params.color_path,1);
        texture.Bind();

        shader.Bind();
        shader.SetUniform1i("texture1", 1);

        glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        shader.Unbind();
        vao.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();

        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);
       
        bool show_another_window = false;

        bool flag = false;
        bool isDraw = false;
        bool a0a2lock = false;
        bool a0a2zero = false;
        bool NMlock = false;
        

        auto drawPicture = [&]() {
            if (!flag) {
                textureColorBuffer.FrameBind();
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                vao.Bind();
                shader.Bind();
                shader.SetUniform1f("k", params.k);
                shader.SetUniform1f("t", params.t);
                shader.SetUniform1f("L", params.L);
                shader.SetUniform1i("n_range", params.n_range);
                shader.SetUniform1i("m_range", params.m_range);
                shader.SetUniform1f("a0", params.a0);
                shader.SetUniform1f("a2", params.a2);
                shader.SetUniform1f("s0", params.s0);
                shader.SetUniform1f("I", params.I);
                shader.SetUniform1i("type", params.type);
                shader.SetUniform1i("isError", params.isError);

                renderer.Draw(vao, ib, shader);
                auto afterTime = std::chrono::steady_clock::now();

                std::cout << "All time: ";
                double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
                std::cout << duration_second << "s" << std::endl;

                unsigned char* pixels = new unsigned char[params.WIDTH * params.HEIGHT * 4];
                glReadPixels(0, 0, params.WIDTH, params.HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
                stbi_write_png(save_path, params.WIDTH, params.HEIGHT, 4, pixels, params.WIDTH * 4);
                if (isDraw) {
                    save_path_result = params.file_dir + "/t=" + std::to_string(params.t) + " eta=" + std::to_string(params.k) + " L=" + std::to_string(params.L) + " I=" + std::to_string(params.I) +
                        " P=" + std::to_string(params.n_range) + " Q=" + std::to_string(params.m_range) + " A0=" + std::to_string(params.a0) + " A2=" + std::to_string(params.a2) + "   " + errorList[params.isError] + ".png";
                    FlipImageVertically(pixels, params.WIDTH, params.HEIGHT);
                    stbi_write_png(save_path_result.c_str(), params.WIDTH, params.HEIGHT, 4, pixels, params.WIDTH * 4);
                    isDraw = false;
                    std::cout << "Successful save picture" << std::endl;
                }
                delete[] pixels;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                flag = true;
            }
        };
        while (!openui && std::cin >> params.t >> params.k >> params.L >> params.I >> params.n_range >> params.m_range >> params.a0 >> params.a2) {
            isDraw = true;
            drawPicture();
            flag = false;
            isDraw = true;
        }

        while (openui&&!glfwWindowShouldClose(window)) {
            drawPicture();
            renderer.Clear();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            secondshader.Bind();
            {
                
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
                glm::mat4 mvp = proj * view * model;
                secondshader.SetUniformMat4f("u_MVP", mvp);
                renderer.Draw(vao, ib, secondshader);
            }
            {
            
                ImGui::Begin("Chladni Aesthetic Patterns");
                if (ImGui::SliderFloat("Value t", &params.t, 0.0f, 50.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("Value eta", &params.k, 0.5f, 25.0f)) {
                    flag = false;
                }           

                if (ImGui::SliderFloat("Value L", &params.L, 0.0f, 5.0f)) {
                    flag = false;
                }

                if (ImGui::SliderFloat("Value I", &params.I, 0.0f, 350.0f)) {
                    flag = false;
                }

                if (ImGui::SliderInt("Value P", &params.n_range, 1, 50)) {
                    if (NMlock) {
                        params.m_range = params.n_range;
                    }
                    flag = false;
                }

                if (ImGui::SliderInt("Value Q", &params.m_range, 1, 50)) {
                    if (NMlock) {
                        params.n_range = params.m_range;
                    }
                    flag = false;
                }

                if (ImGui::SliderFloat("Value A0", &params.a0, 0.0f, 5.0f)) {
                    if (a0a2lock) {
                        params.a2 = params.a0;
                    }
                    if (a0a2zero) {
                        params.a0 = 0;
                    }
                    flag = false;
                }

                if (ImGui::SliderFloat("Value A2", &params.a2, 0.0f, 5.0f)) {
                    if (a0a2lock) {
                        params.a0 = params.a2;
                    }
                    if (a0a2zero) {
                        params.a2 = 0;
                    }
                    flag = false;
                }

                if (ImGui::SliderFloat("Slice height", &params.s0, 10.0f, 1000.0f)) {
                    flag = false;
                }

                if (ImGui::BeginCombo(
                    "type",
                    typeList[params.type].c_str())) {

                    for (int i = 0; i < typeList.size(); ++i) {
                        bool selected = (params.type == i);

                        if (ImGui::Selectable(
                            typeList[i].c_str(),
                            selected)) {

                            switch (i) {
                                case(2): {
                                    a0a2lock = true;
                                    NMlock = true;
                                    params.a0 = params.a2;
                                    params.n_range = params.m_range;
                                    break;
                                }
                                case(3): {
                                    a0a2zero = true;
                                    params.a0 = 0;
                                    params.a2 = 0;
                                    break;
                                }
                                case(6): {
                                    a0a2zero = true;
                                    NMlock = true;
                                    params.a0 = 0;
                                    params.a2 = 0;
                                    params.n_range = params.m_range;
                                    break;
                                }
                                case(8): {
                                    a0a2lock = true;
                                    NMlock = true;
                                    params.a0 = params.a2;
                                    params.n_range = params.m_range;
                                    break;
                                }
                                case(9): {
                                    a0a2lock = true;
                                    NMlock = true;
                                    params.a0 = params.a2;
                                    params.n_range = params.m_range;
                                    break;
                                }
                                      
                                default: {
                                    a0a2lock = false;
                                    a0a2zero = false;
                                    NMlock = false;
                                    break;
                                }
                            }

                            params.type = i;
                            flag = false;
                        }

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::BeginCombo("current texture",
                    textureNames[params.currentTextureIndex].c_str())) {

                    for (int i = 0; i < textureNames.size(); ++i) {
                        bool selected = (params.currentTextureIndex == i);

                        if (ImGui::Selectable(textureNames[i].c_str(), selected)) {
                            params.currentTextureIndex = i;

                            flag = false;
                            texture.Updata(textureNames[i]);
                            texture.Bind();
                            shader.Bind();
                            shader.SetUniform1i("texture1", 1);
                        }

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::Button(errorList[params.isError].c_str())) {
                    flag = false;
                    params.isError = 1- params.isError;
                }
                if (ImGui::Button("Save result")) {
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