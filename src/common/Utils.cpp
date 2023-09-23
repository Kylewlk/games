
#include <fstream>
#include <iostream>

#include "App.h"
#include "EventSystem.h"
#include "MathHelp.h"
#include "Utils.h"
#include "Shader.h"

const char* glCodeToString(GLenum error)
{
    switch (error)
    {
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    default:
        return "<<unknow>>";
    }
}

std::string readFileToString(const std::string& filePath)
{
    std::string text;
    std::ifstream stream(filePath, std::ios::binary | std::ios::in);
    if (stream.is_open())
    {
        std::streampos start = stream.tellg();
        stream.seekg(0, std::ios::end);
        std::streampos end = stream.tellg();
        stream.seekg(0, std::ios::beg);
        auto fileSize = end - start;
        text.resize(uint32_t(fileSize));
        stream.read(text.data(), fileSize);
        stream.close();
    }
    else
    {
        std::string msg = "failed to open file!, File Path: " + filePath;
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }
    return text;
}

ByteBuffer readFile(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        std::string msg = "failed to open file!, File Path: " + filePath;
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }
    auto fileSize = file.tellg();
    ByteBuffer buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

void writeToFile(const std::string& filePath, const std::string& str)
{
    std::ofstream file(filePath, std::ios::ate|std::ios::binary);
    if (!file.is_open())
    {
        std::string msg = "failed to create file!, File Path: " + filePath;
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }

    file.write(str.data(), (std::streamsize)str.size());
    file.flush();
    file.close();
}

void writeToFile(const std::string& filePath, const ByteBuffer& data)
{
    std::ofstream file(filePath, std::ios::ate|std::ios::binary);
    if (!file.is_open())
    {
        std::string msg = "failed to create file!, File Path: " + filePath;
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }

    file.write(data.data(), (std::streamsize)data.size());
    file.flush();
    file.close();
}

struct GLData
{
    GLuint vao{0}, vbo{0}, ebo{0};
    uint32_t currentVboSize{0};
    uint32_t currentEboSize{0};
    CustomEventListenerRef cleanListener;

    GLData()
    {
        cleanListener = CustomEventListener::create(CustomEvent::exitSystemEvent);
        cleanListener->onCustomEvent = [this](const CustomEvent* e) {
            if (this->vao != 0)
            {
                glDeleteVertexArrays(1, &vao);
                this->vao = 0;
            }
            if (this->vbo != 0)
            {
                glDeleteBuffers(1, &vbo);
            }
            if (this->ebo != 0)
            {
                glDeleteBuffers(1, &ebo);
            }
        };
        EventSystem::get()->subscribe(cleanListener);
    }

    void updateVBO(void* data, size_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (this->currentVboSize >= size)
        {
            if (data != nullptr)
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLuint)size, data);
            }
        }
        else
        {
            glBufferData(GL_ARRAY_BUFFER, (GLuint)size, data, GL_DYNAMIC_DRAW);
        }
        this->currentVboSize = size;
    }

    void updateEBO(void* data, size_t size)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        if (this->currentEboSize >= size)
        {
            if (data != nullptr)
            {
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (GLuint)size, data);
            }
        }
        else
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLuint)size, data, GL_DYNAMIC_DRAW);
        }
        this->currentEboSize = size;
    }

};

struct GLShader
{
    GLShader(const char* vert, const char* frag)
        : vert(vert), frag(frag)
    {
        cleanListener = CustomEventListener::create(CustomEvent::exitSystemEvent);
        cleanListener->onCustomEvent = [this](const CustomEvent* e) {
            shader.reset();
        };
        EventSystem::get()->subscribe(cleanListener);
    }

    ShaderRef& getShader()
    {
        if (this->shader == nullptr)
        {
            this->shader = Shader::createByPath(vert, frag);
        }
        return this->shader;
    }

private:
    ShaderRef shader;
    CustomEventListenerRef cleanListener;
    const char* vert{nullptr};
    const char* frag{nullptr};
};

void drawQuad()
{
    static GLData glData;
    auto& vao = glData.vao;

    if (vao == 0)
    {
        auto& vbo = glData.vbo;
        // clang-format off
        const float vertices[] = {
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 1.0f, 0.0f
        };
        // clang-format on
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*) (sizeof(float) * 2));
        glEnableVertexAttribArray(1);
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static GLData& getSolid2DData()
{
    static GLData glData;
    if (glData.vao == 0)
    {
        auto& vao = glData.vao;
        auto& vbo = glData.vbo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2) , (void*) nullptr);
        glEnableVertexAttribArray(0);
    }
    return glData;
}

static GLData& getSolid3DData()
{
    static GLData glData;
    if (glData.vao == 0)
    {
        auto& vao = glData.vao;
        auto& vbo = glData.vbo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3) , (void*) nullptr);
        glEnableVertexAttribArray(0);
    }
    return glData;
}

static ShaderRef& getSolidShader()
{
    static GLShader shader("asset/shader/solid.vert", "asset/shader/solid.frag");
    return shader.getShader();
}

static ShaderRef& getSolidGouraudShader()
{
    static GLShader shader("asset/shader/solidGouraud.vert", "asset/shader/solidGouraud.frag");
    return shader.getShader();
}

void drawPoints(math::Vec3* points, int count, const math::Mat4& mat, const math::Vec4& color, float pointSize)
{
    auto& glData = getSolid3DData();
    auto& shader = getSolidShader();
    glData.updateVBO(points, sizeof(math::Vec3)*count);

    shader->use();
    shader->setUniform("mvp", mat);
    shader->setUniform("color", color);

    glBindVertexArray(glData.vao);
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, count);
}

void drawPoints(math::Vec2* points, int count, const math::Mat4& mat, const math::Vec4& color, float pointSize)
{
    auto& glData = getSolid2DData();
    glData.updateVBO(points, sizeof(math::Vec2)*count);

    auto& shader = getSolidShader();
    shader->use();
    shader->setUniform("mvp", mat);
    shader->setUniform("color", color);

    glBindVertexArray(glData.vao);
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, count);
}

void drawLines(math::Vec3* points, int count, LineType type, const math::Mat4& mat, const math::Vec4& color, float width)
{
    auto& glData = getSolid3DData();
    glData.updateVBO(points, sizeof(math::Vec3)*count);

    auto& shader = getSolidShader();
    shader->use();
    shader->setUniform("mvp", mat);
    shader->setUniform("color", color);

    glBindVertexArray(glData.vao);
    glLineWidth(width);
    if (type == LineType::lines)
    {
        glDrawArrays(GL_LINES, 0, count);
    }
    else if(type == LineType::strip)
    {
        glDrawArrays(GL_LINE_STRIP, 0, count);
    }
    else
    {
        glDrawArrays(GL_LINE_LOOP, 0, count);
    }
}

void drawLines(math::Vec2* points, int count, LineType type, const math::Mat4& mat, const math::Vec4& color, float width)
{
    auto& glData = getSolid2DData();
    glData.updateVBO(points, sizeof(math::Vec2)*count);

    auto& shader = getSolidShader();
    shader->use();
    shader->setUniform("mvp", mat);
    shader->setUniform("color", color);

    glBindVertexArray(glData.vao);
    glLineWidth(width);
    if (type == LineType::lines)
    {
        glDrawArrays(GL_LINES, 0, count);
    }
    else if(type == LineType::strip)
    {
        glDrawArrays(GL_LINE_STRIP, 0, count);
    }
    else
    {
        glDrawArrays(GL_LINE_LOOP, 0, count);
    }
}

void drawAxis(const math::Mat4& mat, float size, float width, bool is3D)
{
    static GLData glData;

    if (glData.vao == 0)
    {
        auto& vao = glData.vao;
        auto& vbo = glData.vbo;

        // clang-format off
        const float vertices[] = {
            0, 0, 0, 1, 0, 0, // x
            1, 0, 0, 1, 0, 0, // x
            0, 0, 0, 0, 1, 0, // y
            0, 1, 0, 0, 1, 0, // y
            0, 0, 0, 0, 0, 1, // z
            0, 0, 1, 0, 0, 1, // z
        };
        // clang-format on
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*) (sizeof(float) * 3));
        glEnableVertexAttribArray(1);
    }

    ShaderRef& shader = getSolidGouraudShader();
    shader->use();
    shader->setUniform("mvp", mat * math::scale({size, size, size}));
    shader->setUniform("color", math::Vec4{1, 1, 1, 1});

    glBindVertexArray(glData.vao);
    glLineWidth(width);
    glDrawArrays(GL_LINES, 0, is3D ? 6 : 4);
}

void drawAxis2D(const math::Mat4& mat, float size, float width)
{
    drawAxis(mat, size, width, false);
}

void drawAxis3D(const math::Mat4& mat, float size, float width)
{
    drawAxis(mat, size, width, true);
}

void drawString(math::Vec2 pos, const math::Vec4& color, std::string_view str)
{
    auto dl = ImGui::GetForegroundDrawList();
    auto col =  ImGui::ColorConvertFloat4ToU32({color.x, color.y, color.z, color.w});
    const auto& mainScreenPos = App::get()->getMainScenePos();
    dl->AddText(ImVec2(pos.x + mainScreenPos.x, pos.y + mainScreenPos.y), col, str.data(), str.data() + str.size());
}

void drawIndicator(math::Vec2* poses, int count, const math::Vec4& color)
{
    auto dl = ImGui::GetForegroundDrawList();
    auto col =  ImGui::ColorConvertFloat4ToU32({color.x, color.y, color.z, color.w});
    const auto& mainScreenPos = App::get()->getMainScenePos();

    for (int i = 0; i < count; ++i)
    {
        auto& pos = poses[i];
        dl->AddText(ImVec2(pos.x + mainScreenPos.x, pos.y + mainScreenPos.y), col, std::to_string(i).c_str());
    }
}

namespace json
{
    math::Vec2 vec2(Json json)
    {
        return math::Vec2{json["x"], json["y"]};
    }

    math::Vec3 vec3(Json json)
    {
        return math::Vec3{json["x"], json["y"], json["z"]};
    }

    math::Vec4 vec4(Json json)
    {
        return math::Vec4{json["x"], json["y"], json["z"], json["w"]};
    }

    Json to(const math::Vec2& v)
    {
        Json j;
        j["x"] = v.x;
        j["y"] = v.y;
        return j;
    }

    Json to(const math::Vec3& v)
    {
        Json j;
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
        return j;
    }

    Json to(const math::Vec4& v)
    {
        Json j;
        j["x"] = v.x;
        j["y"] = v.y;
        j["z"] = v.z;
        j["w"] = v.w;
        return j;
    }
}