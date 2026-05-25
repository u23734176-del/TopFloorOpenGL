#include "Decoration.h"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

static const float DECO_PI = 3.14159265358979323846f;


DecorationType::DecorationType()
    : vao(0), vbo(0), shader(0), vertexCount(0), built(false) {}

DecorationType::~DecorationType()
{
    if (vbo)
        glDeleteBuffers(1, &vbo);
    if (vao)
        glDeleteVertexArrays(1, &vao);
}



void DecorationType::appendBox(std::vector<float> &d,
                               const glm::vec3 &c, const glm::vec3 &h)
{
    
    struct Face
    {
        glm::vec3 n;
        glm::vec3 a, b, cc, dd;
    };
    glm::vec3 p[8] = {
        {c.x - h.x, c.y - h.y, c.z + h.z}, {c.x + h.x, c.y - h.y, c.z + h.z}, {c.x + h.x, c.y + h.y, c.z + h.z}, {c.x - h.x, c.y + h.y, c.z + h.z}, 
        {c.x - h.x, c.y - h.y, c.z - h.z},
        {c.x + h.x, c.y - h.y, c.z - h.z},
        {c.x + h.x, c.y + h.y, c.z - h.z},
        {c.x - h.x, c.y + h.y, c.z - h.z} 
    };
    Face faces[6] = {
        {{0, 0, 1}, p[0], p[1], p[2], p[3]},  
        {{0, 0, -1}, p[5], p[4], p[7], p[6]}, 
        {{-1, 0, 0}, p[4], p[0], p[3], p[7]}, 
        {{1, 0, 0}, p[1], p[5], p[6], p[2]},  
        {{0, 1, 0}, p[3], p[2], p[6], p[7]},  
        {{0, -1, 0}, p[4], p[5], p[1], p[0]}  
    };
    auto v = [&](const glm::vec3 &pos, const glm::vec3 &n)
    {
        d.push_back(pos.x);
        d.push_back(pos.y);
        d.push_back(pos.z);
        d.push_back(n.x);
        d.push_back(n.y);
        d.push_back(n.z);
    };
    for (int f = 0; f < 6; ++f)
    {
        const Face &fc = faces[f];
        v(fc.a, fc.n);
        v(fc.b, fc.n);
        v(fc.cc, fc.n);
        v(fc.a, fc.n);
        v(fc.cc, fc.n);
        v(fc.dd, fc.n);
    }
}


void DecorationType::appendCylinder(std::vector<float> &d,
                                    const glm::vec3 &base, float radius,
                                    float height, int segments)
{
    auto v = [&](const glm::vec3 &pos, const glm::vec3 &n)
    {
        d.push_back(pos.x);
        d.push_back(pos.y);
        d.push_back(pos.z);
        d.push_back(n.x);
        d.push_back(n.y);
        d.push_back(n.z);
    };
    for (int i = 0; i < segments; ++i)
    {
        float a0 = 2.0f * DECO_PI * (float)i / (float)segments;
        float a1 = 2.0f * DECO_PI * (float)(i + 1) / (float)segments;
        glm::vec3 n0(cosf(a0), 0.0f, sinf(a0));
        glm::vec3 n1(cosf(a1), 0.0f, sinf(a1));
        glm::vec3 b0 = base + glm::vec3(n0.x * radius, 0.0f, n0.z * radius);
        glm::vec3 b1 = base + glm::vec3(n1.x * radius, 0.0f, n1.z * radius);
        glm::vec3 t0 = base + glm::vec3(n0.x * radius, height, n0.z * radius);
        glm::vec3 t1 = base + glm::vec3(n1.x * radius, height, n1.z * radius);
        
        v(b0, n0);
        v(b1, n1);
        v(t1, n1);
        v(b0, n0);
        v(t1, n1);
        v(t0, n0);
    }
}


void DecorationType::genGazebo(std::vector<float> &d)
{
    
    appendBox(d, glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(1.5f, 0.1f, 1.5f)); 
    float px[4] = {-1.3f, 1.3f, 1.3f, -1.3f};
    float pz[4] = {-1.3f, -1.3f, 1.3f, 1.3f};
    for (int i = 0; i < 4; ++i)
        appendBox(d, glm::vec3(px[i], 1.1f, pz[i]), glm::vec3(0.1f, 1.0f, 0.1f)); 
    appendBox(d, glm::vec3(0.0f, 2.2f, 0.0f), glm::vec3(1.7f, 0.15f, 1.7f));      
}

void DecorationType::genBridge(std::vector<float> &d)
{
    appendBox(d, glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(2.5f, 0.1f, 1.0f));   
    appendBox(d, glm::vec3(0.0f, 0.8f, 0.9f), glm::vec3(2.5f, 0.4f, 0.08f));  
    appendBox(d, glm::vec3(0.0f, 0.8f, -0.9f), glm::vec3(2.5f, 0.4f, 0.08f)); 
}

void DecorationType::genTree(std::vector<float> &d)
{
    appendCylinder(d, glm::vec3(0.0f, 0.0f, 0.0f), 0.15f, 1.2f, 8);         
    appendBox(d, glm::vec3(0.0f, 1.8f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f)); 
}

void DecorationType::genBench(std::vector<float> &d)
{
    appendBox(d, glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.9f, 0.05f, 0.3f));    
    appendBox(d, glm::vec3(0.0f, 0.7f, -0.25f), glm::vec3(0.9f, 0.25f, 0.05f)); 
    appendBox(d, glm::vec3(-0.7f, 0.2f, 0.0f), glm::vec3(0.05f, 0.2f, 0.3f));   
    appendBox(d, glm::vec3(0.7f, 0.2f, 0.0f), glm::vec3(0.05f, 0.2f, 0.3f));    
}

void DecorationType::genBillboard(std::vector<float> &d)
{
    appendBox(d, glm::vec3(-0.5f, 0.75f, 0.0f), glm::vec3(0.06f, 0.75f, 0.06f)); 
    appendBox(d, glm::vec3(0.5f, 0.75f, 0.0f), glm::vec3(0.06f, 0.75f, 0.06f));  
    appendBox(d, glm::vec3(0.0f, 1.6f, 0.0f), glm::vec3(1.0f, 0.6f, 0.05f));     
}

void DecorationType::genPathwayLight(std::vector<float> &d)
{
    appendCylinder(d, glm::vec3(0.0f, 0.0f, 0.0f), 0.05f, 0.8f, 6);            
    appendBox(d, glm::vec3(0.0f, 0.9f, 0.0f), glm::vec3(0.12f, 0.12f, 0.12f)); 
}

void DecorationType::upload(const std::vector<float> &data)
{
    vertexCount = (int)(data.size() / 6);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void DecorationType::build(DecorationKind kind)
{
    if (built)
        return; 
    shader = LoadShaders("basic.vert", "basic.frag");

    std::vector<float> data;
    switch (kind)
    {
    case DecorationKind::GAZEBO:
        genGazebo(data);
        break;
    case DecorationKind::BRIDGE:
        genBridge(data);
        break;
    case DecorationKind::TREE:
        genTree(data);
        break;
    case DecorationKind::BENCH:
        genBench(data);
        break;
    case DecorationKind::BILLBOARD:
        genBillboard(data);
        break;
    case DecorationKind::PATHWAY_LIGHT:
        genPathwayLight(data);
        break;
    }
    upload(data);
    built = true;
}

void DecorationType::draw(const glm::mat4 &model,
                          const glm::vec3 &color,
                          const glm::mat4 &view,
                          const glm::mat4 &proj) const
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(glGetUniformLocation(shader, "objectColor"), 1, &color[0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void DecorationType::drawDepth(const glm::mat4 &model, GLuint depthShaderProgram) const
{
    
    
    glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "model"),
                       1, GL_FALSE, &model[0][0]);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}





Decoration::Decoration(DecorationType *shared) : type(shared) {}

void Decoration::build()
{
    
    
}

void Decoration::draw(const glm::mat4 &view,
                      const glm::mat4 &proj,
                      const LightSet &lights)
{
    if (type)
    {
        type->draw(getModelMatrix(), getColor(), view, proj);
    }
}

void Decoration::drawDepth(GLuint depthShaderProgram)
{
    if (type)
    {
        type->drawDepth(getModelMatrix(), depthShaderProgram);
    }
}





DecorationFactory *DecorationFactory::instance = nullptr;

DecorationFactory::DecorationFactory() {}

DecorationFactory::~DecorationFactory()
{
    for (auto &kv : types)
        delete kv.second;
    types.clear();
}

DecorationFactory *DecorationFactory::getInstance()
{
    if (!instance)
        instance = new DecorationFactory();
    return instance;
}

DecorationType *DecorationFactory::getType(DecorationKind kind)
{
    auto it = types.find(kind);
    if (it != types.end())
        return it->second;

    DecorationType *t = new DecorationType();
    t->build(kind); 
    types[kind] = t;
    return t;
}

Decoration *DecorationFactory::create(DecorationKind kind, const glm::vec3 &position)
{
    DecorationType *t = getType(kind);
    Decoration *deco = new Decoration(t);
    deco->setPosition(position);
    return deco;
}