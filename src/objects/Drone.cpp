#include "Drone.h"
#include <cmath>
#include <iostream>

static const float PI = 3.14159265358979f;


static const char* VERT = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vColor;
uniform mat4 mvp;
void main() {
    vColor      = aColor;
    gl_Position = mvp * vec4(aPos, 1.0);
}
)";

static const char* FRAG = R"(
#version 330 core
in  vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";


static void shadedColor(float nx, float ny, float nz,
                        float cr, float cg, float cb,
                        float& r,  float& g,  float& b)
{
    float lx=1, ly=2, lz=3;
    float ll = sqrt(lx*lx + ly*ly + lz*lz);
    lx/=ll; ly/=ll; lz/=ll;
    float nl = sqrt(nx*nx + ny*ny + nz*nz);
    if (nl > 0.0f) { nx/=nl; ny/=nl; nz/=nl; }
    float diff  = nx*lx + ny*ly + nz*lz;
    if (diff < 0) diff = 0;
    float light = 0.3f + 0.7f * diff;
    r = cr*light; g = cg*light; b = cb*light;
}



Drone::Drone()
    : position(0.0f, 5.0f, 0.0f)
    , front(0.0f, 0.0f, -1.0f)
    , worldUp(0.0f, 1.0f, 0.0f)
    , yaw(-90.0f), pitch(0.0f), roll(0.0f)
    , speed(8.0f), rotorAngle(0.0f)
    , bodyVAO(0),  bodyCount(0)
    , armVAO(0),   armCount(0)
    , rotorVAO(0), rotorCount(0)
    , guardVAO(0), guardCount(0)
    , shaderProgram(0)
{}

Drone::~Drone()
{
    glDeleteVertexArrays(1, &bodyVAO);
    glDeleteVertexArrays(1, &armVAO);
    glDeleteVertexArrays(1, &rotorVAO);
    glDeleteVertexArrays(1, &guardVAO);
    glDeleteProgram(shaderProgram);
}
void Drone::drawDepth(GLuint depthShaderProgram)
{
    
    glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
    glm::vec3 up      = glm::normalize(glm::vec3(rollMat * glm::vec4(worldUp, 0.0f)));
    glm::vec3 right   = glm::normalize(glm::cross(front, up));

    glm::mat4 base(1.0f);
    base[0] = glm::vec4(right,    0.0f);
    base[1] = glm::vec4(up,       0.0f);
    base[2] = glm::vec4(-front,   0.0f);
    base[3] = glm::vec4(position, 1.0f);

    glm::mat4 bodyModel = base * glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.14f, 0.35f));

    glUseProgram(depthShaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "model"),
                       1, GL_FALSE, glm::value_ptr(bodyModel));
    glBindVertexArray(bodyVAO);
    glDrawArrays(GL_TRIANGLES, 0, bodyCount);
    glBindVertexArray(0);
}

void Drone::build()
{
    shaderProgram = loadDroneShaders();

    
    auto bv = makeSphere(1.0f, 16, 16, 0.20f, 0.20f, 0.22f);
    bodyVAO   = uploadMesh(bv);
    bodyCount = (int)bv.size() / 6;

    
    auto av = makeCylinder(1.0f, 1.0f, 12, 0.28f, 0.28f, 0.30f);
    armVAO   = uploadMesh(av);
    armCount = (int)av.size() / 6;

    
    auto rv = makeDisc(1.0f, 20, 0.55f, 0.55f, 0.60f);
    rotorVAO   = uploadMesh(rv);
    rotorCount = (int)rv.size() / 6;

    
    auto gv = makeTorus(1.0f, 0.06f, 28, 8, 0.25f, 0.25f, 0.27f);
    guardVAO   = uploadMesh(gv);
    guardCount = (int)gv.size() / 6;
}


void Drone::processInput(GLFWwindow* window, float deltaTime)
{
    float vel      = speed * deltaTime;
    float rotSpeed = 60.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) vel *= 2.0f;

    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += front   * vel;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= front   * vel;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= right   * vel;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += right   * vel;
    // Inside Drone::processInput(...)
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) // Changed from SPACE
        position += worldUp * speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= worldUp * speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) position -= worldUp * vel;

    if (glfwGetKey(window, GLFW_KEY_LEFT)  == GLFW_PRESS) yaw   -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw   += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_UP)    == GLFW_PRESS) pitch += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN)  == GLFW_PRESS) pitch -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q)     == GLFW_PRESS) roll  -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_E)     == GLFW_PRESS) roll  += rotSpeed;

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateFront();

    rotorAngle += 720.0f * deltaTime;
    if (rotorAngle > 360.0f) rotorAngle -= 360.0f;
}

void Drone::updateFront()
{
    glm::vec3 d;
    d.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    d.y = sin(glm::radians(pitch));
    d.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(d);
}

glm::vec3 Drone::getPosition() const { return position; }
glm::vec3 Drone::getFront()    const { return front;    }
float     Drone::getRoll()     const { return roll;     }


void Drone::draw(const glm::mat4& view, const glm::mat4& proj, const LightSet& lights)
{
    
    glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), glm::radians(roll), front);
    glm::vec3 up      = glm::normalize(glm::vec3(rollMat * glm::vec4(worldUp, 0.0f)));
    glm::vec3 right   = glm::normalize(glm::cross(front, up));

    
    glm::mat4 base(1.0f);
    base[0] = glm::vec4(right,   0.0f);
    base[1] = glm::vec4(up,      0.0f);
    base[2] = glm::vec4(-front,  0.0f);
    base[3] = glm::vec4(position, 1.0f);

    
    glm::mat4 bodyModel = base * glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.14f, 0.35f));
    drawPart(bodyVAO, bodyCount, bodyModel, view, proj);

    
    float angles[4] = { 45.0f, 135.0f, 225.0f, 315.0f };
    float armLen = 0.55f;

    for (int i = 0; i < 4; i++)
    {
        float rad = glm::radians(angles[i]);
        glm::vec3 localDir(cos(rad), 0.0f, sin(rad));
        glm::vec3 worldDir = right * localDir.x + (-front) * localDir.z;

        
        glm::vec3 armCenter = position + worldDir * (armLen * 0.5f);
        glm::vec3 cylDefault(0.0f, 1.0f, 0.0f);
        glm::vec3 axis = glm::cross(cylDefault, worldDir);
        float     ang  = acos(glm::clamp(glm::dot(cylDefault, worldDir), -1.0f, 1.0f));

        glm::mat4 armModel = glm::translate(glm::mat4(1.0f), armCenter);
        if (glm::length(axis) > 0.001f)
            armModel = glm::rotate(armModel, ang, glm::normalize(axis));
        armModel = glm::scale(armModel, glm::vec3(0.035f, armLen * 0.5f, 0.035f));
        drawPart(armVAO, armCount, armModel, view, proj);

        
        glm::vec3 rotorPos = position + worldDir * armLen + up * 0.06f;

        
        glm::mat4 rotorModel = glm::translate(glm::mat4(1.0f), rotorPos);
        rotorModel = glm::rotate(rotorModel, glm::radians(rotorAngle + i * 90.0f), up);
        rotorModel = glm::scale(rotorModel, glm::vec3(0.22f, 0.01f, 0.22f));
        drawPart(rotorVAO, rotorCount, rotorModel, view, proj);

        
        glm::mat4 guardModel = glm::translate(glm::mat4(1.0f), rotorPos);
        glm::vec3 torusUp(0.0f, 1.0f, 0.0f);
        glm::vec3 gAxis = glm::cross(torusUp, up);
        float     gAng  = acos(glm::clamp(glm::dot(torusUp, up), -1.0f, 1.0f));
        if (glm::length(gAxis) > 0.001f)
            guardModel = glm::rotate(guardModel, gAng, glm::normalize(gAxis));
        guardModel = glm::scale(guardModel, glm::vec3(0.24f, 0.24f, 0.24f));
        drawPart(guardVAO, guardCount, guardModel, view, proj);
    }
}

void Drone::drawPart(GLuint vao, int count,
                     const glm::mat4& model,
                     const glm::mat4& view,
                     const glm::mat4& proj)
{
    glUseProgram(shaderProgram);
    glm::mat4 mvp = proj * view * model;
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}


std::vector<float> Drone::makeSphere(float r, int stacks, int slices,
                                     float cr, float cg, float cb)
{
    std::vector<float> grid;
    for (int i = 0; i <= stacks; i++) {
        float theta = i * PI / stacks;
        float sinT = sin(theta), cosT = cos(theta);
        for (int j = 0; j <= slices; j++) {
            float phi = j * 2.0f * PI / slices;
            float x = r * sinT * cos(phi);
            float y = r * cosT;
            float z = r * sinT * sin(phi);
            float ro, go, bo;
            shadedColor(x, y, z, cr, cg, cb, ro, go, bo);
            grid.insert(grid.end(), {x, y, z, ro, go, bo});
        }
    }
    std::vector<float> tris;
    auto push = [&](int idx) {
        for (int k = 0; k < 6; k++) tris.push_back(grid[idx*6+k]);
    };
    for (int i = 0; i < stacks; i++)
        for (int j = 0; j < slices; j++) {
            int p1 = i*(slices+1)+j, p2 = i*(slices+1)+j+1;
            int p3 = (i+1)*(slices+1)+j, p4 = (i+1)*(slices+1)+j+1;
            push(p1); push(p2); push(p3);
            push(p2); push(p4); push(p3);
        }
    return tris;
}

std::vector<float> Drone::makeCylinder(float r, float h, int slices,
                                       float cr, float cg, float cb)
{
    std::vector<float> v;
    float halfH = h * 0.5f;
    for (int i = 0; i < slices; i++) {
        float a0 = i     * 2.0f * PI / slices;
        float a1 = (i+1) * 2.0f * PI / slices;
        float x0=cos(a0)*r, z0=sin(a0)*r;
        float x1=cos(a1)*r, z1=sin(a1)*r;
        float nxm=(cos(a0)+cos(a1))*0.5f, nzm=(sin(a0)+sin(a1))*0.5f;
        float ro, go, bo;
        
        shadedColor(nxm, 0, nzm, cr, cg, cb, ro, go, bo);
        v.insert(v.end(), {x0,-halfH,z0,ro,go,bo, x1,-halfH,z1,ro,go,bo, x1,halfH,z1,ro,go,bo,
                           x0,-halfH,z0,ro,go,bo, x1, halfH,z1,ro,go,bo, x0,halfH,z0,ro,go,bo});
        
        shadedColor(0,1,0, cr,cg,cb, ro,go,bo);
        v.insert(v.end(), {0,halfH,0,ro,go,bo, x0,halfH,z0,ro,go,bo, x1,halfH,z1,ro,go,bo});
        
        shadedColor(0,-1,0, cr,cg,cb, ro,go,bo);
        v.insert(v.end(), {0,-halfH,0,ro,go,bo, x1,-halfH,z1,ro,go,bo, x0,-halfH,z0,ro,go,bo});
    }
    return v;
}

std::vector<float> Drone::makeDisc(float r, int slices,
                                   float cr, float cg, float cb)
{
    std::vector<float> v;
    float ro, go, bo;
    shadedColor(0, 1, 0, cr, cg, cb, ro, go, bo);
    for (int i = 0; i < slices; i++) {
        float a0 = i     * 2.0f * PI / slices;
        float a1 = (i+1) * 2.0f * PI / slices;
        v.insert(v.end(), {0,0,0,ro,go,bo,
                           (float) cos(a0)*r, 0, (float) sin(a0)*r, ro,go,bo,
                           (float) cos(a1)*r, 0, (float) sin(a1)*r, ro,go,bo});
    }
    return v;
}

std::vector<float> Drone::makeTorus(float mainR, float tubeR,
                                    int mainSegs, int tubeSegs,
                                    float cr, float cg, float cb)
{
    std::vector<float> v;
    for (int i = 0; i < mainSegs; i++) {
        float a0 = i     * 2.0f * PI / mainSegs;
        float a1 = (i+1) * 2.0f * PI / mainSegs;
        for (int j = 0; j < tubeSegs; j++) {
            float b0 = j     * 2.0f * PI / tubeSegs;
            float b1 = (j+1) * 2.0f * PI / tubeSegs;

            auto pt = [&](float a, float b, float& px, float& py, float& pz,
                                            float& nx, float& ny, float& nz) {
                px = (mainR + tubeR * cos(b)) * cos(a);
                py =          tubeR * sin(b);
                pz = (mainR + tubeR * cos(b)) * sin(a);
                nx = cos(b) * cos(a);
                ny = sin(b);
                nz = cos(b) * sin(a);
            };

            float p0x,p0y,p0z,n0x,n0y,n0z;
            float p1x,p1y,p1z,n1x,n1y,n1z;
            float p2x,p2y,p2z,n2x,n2y,n2z;
            float p3x,p3y,p3z,n3x,n3y,n3z;
            pt(a0,b0, p0x,p0y,p0z, n0x,n0y,n0z);
            pt(a1,b0, p1x,p1y,p1z, n1x,n1y,n1z);
            pt(a1,b1, p2x,p2y,p2z, n2x,n2y,n2z);
            pt(a0,b1, p3x,p3y,p3z, n3x,n3y,n3z);

            float r0,g0,b0c, r1,g1,b1c, r2,g2,b2c, r3,g3,b3c;
            shadedColor(n0x,n0y,n0z, cr,cg,cb, r0,g0,b0c);
            shadedColor(n1x,n1y,n1z, cr,cg,cb, r1,g1,b1c);
            shadedColor(n2x,n2y,n2z, cr,cg,cb, r2,g2,b2c);
            shadedColor(n3x,n3y,n3z, cr,cg,cb, r3,g3,b3c);

            v.insert(v.end(), {p0x,p0y,p0z,r0,g0,b0c,
                                p1x,p1y,p1z,r1,g1,b1c,
                                p2x,p2y,p2z,r2,g2,b2c,
                                p0x,p0y,p0z,r0,g0,b0c,
                                p2x,p2y,p2z,r2,g2,b2c,
                                p3x,p3y,p3z,r3,g3,b3c});
        }
    }
    return v;
}


GLuint Drone::uploadMesh(const std::vector<float>& verts)
{
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return vao;
}

GLuint Drone::loadDroneShaders()
{
    return LoadShaders("shaders/drone.vert", "shaders/drone.frag");
}