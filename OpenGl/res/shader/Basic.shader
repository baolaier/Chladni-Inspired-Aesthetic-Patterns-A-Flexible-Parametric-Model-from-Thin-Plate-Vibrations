#shader vertex
#version 330 core

layout(location =0) in vec2 position; 
layout(location =1) in vec2 atexCoord;


out vec4 p;
out vec2 texCoord;
void main()  
{  
    texCoord = atexCoord;
    p=vec4(position.x,position.y,1.0f,1.0f);
    gl_Position =p;
};


#shader fragment
#version 330  core

#define EPSILON 1e-6
#define MAX_ITERATIONS 1000
#define PI 3.14159265359

bool point_in_triangle(vec2 p, vec2 a, vec2 b, vec2 c) 
{
    vec2 v0 = b - a;
    vec2 v1 = c - a;
    vec2 v2 = p - a;
    
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    
    return v >= -1e-5 && w >= -1e-5 && (v + w) <= 1.0 + 1e-5;
}
vec2 map_line(vec2 p,float L,float theta){
    float t =L/2.0f;
    vec2 temp1=vec2(t,t);
    vec2 temp2=vec2(t,2*t);

    vec2 temp3=vec2(2*t*cos(theta),t);
    vec2 temp4=vec2(t*cos(theta),t*sin(theta)+t);

    float landa = (temp3.x-p.x)/(temp3.x-temp4.x);
    return vec2(t,landa*t+t);
}
vec2 map(vec2 p,float L,float theta){
    vec2 line1;
    vec2 line2;
    float t =L/2.0f;
    
    line1.y = p.y;
    line1.x = (t+t*sin(theta)-line1.y)/tan(2*theta)+t*cos(theta);

    line2.y = p.y;
    line2.x = (t+t*sin(theta)-line2.y)/tan(theta)+t*cos(theta);

    if(abs(line2.x+line1.x-2*p.x)<1e-6)return p;
    float landa = (line2.x-p.x)/(line2.x-line1.x);

    vec2 new_p1,new_p2;
    new_p1=line1;
    new_p2=map_line(line2,L,theta);
    return new_p1*landa+new_p2*(1-landa);
}
float CHLADNI(float x, float y, float L, float M,float k,float time,int n_range,int m_range,float a0,float a2,float s0,float I,int type) {
	float sum_result = 0.0f;

    float A = a0;
    float B = 1.0f;
    float C = a2;
    float D = 1.0f;
    float E = 4.0f;
    float F = 0.5f;
    float G = 1.0f;
    float H = 0.5f;
    float J = 4.0f;
	float funcT,X_n,Y_m;
    float fai1=acos(A/sqrt(A*A+B*B));
    float fai2=acos(C/sqrt(C*C+D*D));

    float alfa;
    float beta;
    for (int n = 1; n <= n_range; n++) {
        for (int m = 1; m <= m_range; m++) {
            switch(type){
                case 0:
                //Is
                    alfa = (fai1+(float(m)) * PI) / L;
                    beta = 2.0f*(fai2+(float(n)) * PI) / L;
                    break;
                case 1:
                //Bs
                    alfa = ((float(m)) * PI) / L;
                    beta = 2.0f*((float(n)) * PI) / L;
                    break;
                case 2:
                //Ds A=C
                    alfa = ((float(m)) * PI) / L;
                    beta = ((float(n)) * PI) / L;
                    break;
                case 3:
                //Rs  A=0 C=0 N!=M
                    alfa = 2.0f*((float(m)) * PI) / L;
                    beta = 2.0f*((float(n)) * PI) / L;
                    break;
                case 4:
                //Is + Is
                    alfa = 2.0f*(fai1+(float(m)) * PI) / L;
                    beta = 2.0f*(fai2+(float(n)) * PI) / L;
                    break;
                case 5:
                //Bs + Bs
                    alfa = 2.0f*((float(m)) * PI) / L;
                    beta = 2.0f*((float(n)) * PI) / L;
                    break;
                case 6:
                //Rs + Ds  A=0 C=0
                    alfa = 2.0f*((float(m)) * PI) / L;
                    beta = 2.0f*((float(n)) * PI) / L;
                    break;
                case 7:
                //Bs + Is A!=C
                    alfa = 2.0f*((float(m)) * PI) / L;
                    beta = 2.0f*(fai2+(float(n)) * PI) / L;
                    break;
                case 8:
                //Is+Ds A=C N=M
                    alfa = 2.0f*(fai1+(float(m)) * PI) / L;
                    beta = 2.0f*(fai2+(float(n)) * PI) / L;
                    break;
                case 9:
                //Bs + Bs + Ds A=C
                    alfa = 2.0f*((float(m)) * PI) / L;
                    beta = 2.0f*((float(n)) * PI) / L;
                    break;
            }
            //T(t)
            funcT = I*cos((alfa*alfa+beta*beta)/k/k*time) + J*sin((alfa*alfa+beta*beta)/k/k*time);


            //增加一点内容
            //float vv = log(F/E)/L;
            //float deta = log(H/G)/L;
            //
            //float X_ = E*exp(vv*x)+F*exp(-vv*x);
            //float Y_ = G*exp(deta*y)+H*exp(-deta*y);
            //
            //
            X_n =A*cos(alfa * x)+B*sin(alfa * x);
            Y_m =C*cos(beta * y)+D*sin(beta * y);

            sum_result += (X_n * Y_m)*funcT;
        }
    }
    return sum_result/10000.0f/s0;

}


in vec4 p;
in vec2 texCoord;
out vec4 color; 
uniform sampler2D texture1;
uniform float k;
uniform float t;
uniform float L;
uniform int n_range;
uniform int m_range;
uniform float a0;
uniform float a2;
uniform float s0;
uniform float I;
uniform int type;
uniform int isError;
void main()  
{
    float color_num=20.0f;
    float i=1/color_num;
    float M = L; 
    float theta = PI/5.0f;
    vec2 p1 = vec2(0.0f,L/2.0f);
    vec2 p2 = vec2(L/2.0f,L/2.0f);
    vec2 p3 = vec2(L/2.0f*cos(theta),L/2.0f*sin(theta)+L/2.0f);
    vec2 p4 = vec2(2.0f*L/2.0f*cos(theta),L/2.0f);
    

    float a =(p.x+1.0f)/2*L;
    float b =(p.y+1.0f)/2*M;
    
    float a_t =a;
    float b_t =b;

    float z = CHLADNI(a_t,b_t,L,M,k,t,n_range,m_range,a0,a2,s0,I,type);

    if(isError==1){
        float addFunc = (a/L*(1-a/L)*b/M*(1-b/M))*(sin(8.0f*a/L)+cos(5.0f*b/M));
        z+=addFunc/2000.0f;
    }
    
    float range =1000.0f; // 假设 z 的范围是 0 到 2000
    float segment = range / 20.0f;
    
    int index = int((z*1000000+500.0f) / segment);
    switch (index) {
        case 0:  color = texture(texture1, vec2( 0.0,0.2)); break; 
        case 1:  color = texture(texture1, vec2( 1*i,0.2)); break; 
        case 2:  color = texture(texture1, vec2( 2*i,0.2)); break; 
        case 3:  color = texture(texture1, vec2( 3*i,0.2)); break; 
        case 4:  color = texture(texture1, vec2( 4*i,0.2)); break; 
        case 5:  color = texture(texture1, vec2( 5*i,0.2)); break; 
        case 6:  color = texture(texture1, vec2( 6*i,0.2)); break; 
        case 7:  color = texture(texture1, vec2( 7*i,0.2)); break; 
        case 8:  color = texture(texture1, vec2( 8*i,0.2)); break; 
        case 9:  color = texture(texture1, vec2( 9*i,0.2)); break; 
        case 10: color = texture(texture1, vec2(10*i,0.2)); break; 
        case 11: color = texture(texture1, vec2(11*i,0.2)); break; 
        case 12: color = texture(texture1, vec2(12*i,0.2)); break; 
        case 13: color = texture(texture1, vec2(13*i,0.2)); break; 
        case 14: color = texture(texture1, vec2(14*i,0.2)); break; 
        case 15: color = texture(texture1, vec2(15*i,0.2)); break; 
        case 16: color = texture(texture1, vec2(16*i,0.2)); break; 
        case 17: color = texture(texture1, vec2(17*i,0.2)); break;
        case 18: color = texture(texture1, vec2(18*i,0.2)); break;
        case 19: color = texture(texture1, vec2(19*i,0.2)); break;
        default: color = texture(texture1, vec2(20*i,0.2)); break;
    }
    //if(b-0.5f*a-0.5f*L>0.0f)color =vec4(0.0f,0.0f,0.0f,1.0f);
    //if(b+0.5f*a-0.5f*L<0.0f)color =vec4(0.0f,0.0f,0.0f,1.0f);
    //if(!point_in_triangle(vec2(a,b),p1,p2,p3))color =vec4(0.0f,0.0f,0.0f,1.0f);
    //if(!point_in_triangle(vec2(a,b),p2,p3,p4))color =vec4(0.0f,0.0f,0.0f,1.0f);
    //if(!(point_in_triangle(vec2(a,b),p1,p2,p3)||point_in_triangle(vec2(a,b),p2,p3,p4)))color =vec4(0.0f,0.0f,0.0f,1.0f);
};