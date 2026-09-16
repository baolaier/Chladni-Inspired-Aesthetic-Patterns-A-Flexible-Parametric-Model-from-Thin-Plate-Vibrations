#pragma once	
#include <iostream>
struct Parameters {
    
    const int WIDTH = 1000;
    const int HEIGHT = 1000;
    float k;
    float t;
    float L;
    int n_range;
    int m_range;
    float a0;
    float a2;
    float s0 = 50.0f;
    float I;
    int type = 0;
    int currentTextureIndex = 0;
    int isError = 0;
    std::string file_dir = "output";
    std::string color_path = "Texture/0#5000.png";
    Parameters(int type = 0,
        float t = 11.187f,
        float k = 15.0f,
        float L = 3.236f,
        float I = 4.0f,
        int N = 16,
        int M = 16,
        float a0 = 0.434f,
        float a2 = 0.434f) :
        type(type), t(t), k(k), L(L), 
        I(I), n_range(N), m_range(M), a0(a0), a2(a2){}
};
