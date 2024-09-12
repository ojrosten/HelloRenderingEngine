////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "TestingUtilities/GLFWWrappers.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <format>
#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

template<class GetStatus>
GLint check_status(GLuint shaderID, GLenum status, GetStatus getStatus) {
    GLint success{};
    getStatus(shaderID, status, &success);
    return success;
}

template<class GetStatus, class GetInfoLog>
void check_success(GLuint shaderID, std::string_view name, std::string_view buildStage, GLenum status, GetStatus getStatus, GetInfoLog getInfoLog) {
    if(!check_status(shaderID, status, getStatus)) {
        GLchar infoLog[512]{};
        getInfoLog(shaderID, 512, NULL, infoLog);
        throw std::runtime_error{std::format("ERROR::SHADER::{}::{}_FAILED\n{}\n", name, buildStage, infoLog)};
    }
}

void check_compilation_success(GLuint shaderID, std::string_view shaderType) {
    check_success(shaderID, shaderType, "COMPILATION", GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
}

void check_linking_success(GLuint shaderID) {
    check_success(shaderID, "PROGRAM", "LINKING", GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);
}

class resource_handle {
    GLuint m_Index{};
public:
    resource_handle() = default;

    explicit resource_handle(GLuint index) : m_Index{index} {}

    resource_handle(resource_handle&& other) noexcept : m_Index{std::exchange(other.m_Index, 0)} {}

    resource_handle& operator=(resource_handle&& other) noexcept
    {
        std::ranges::swap(m_Index, other.m_Index);
        return *this;
    }

    [[nodiscard]]
    GLuint index() const { return m_Index; }
};

class shader_resource {
    resource_handle m_Handle{};
public:
    shader_resource(GLenum shaderSpecies)
        : m_Handle{glCreateShader(shaderSpecies)}
    {}

    shader_resource(shader_resource&&) noexcept = default;

    shader_resource& operator=(shader_resource&&) noexcept = default;

    ~shader_resource() { glDeleteShader(m_Handle.index()); }

    [[nodiscard]]
    const resource_handle& handle() const { return m_Handle; }
};

class shader_compiler {
    shader_resource m_Resource;
public:
    shader_compiler(GLenum shaderSpecies, const char* source)
        : m_Resource{shaderSpecies}
    {
        const auto index{m_Resource.handle().index()};
        glShaderSource(index, 1, &source, NULL);
        glCompileShader(index);
        check_compilation_success(index, "TO DO");
    }

    [[nodiscard]]
    const shader_resource& resource() const { return m_Resource; }
};

int main()
{
    try
    {
        demo::glfw_manager manager{};
        auto w{manager.create_window()};

        if(!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};

        // build and compile our shader program
        shader_compiler vertexShader{GL_VERTEX_SHADER, vertexShaderSource}, fragmentShader{GL_FRAGMENT_SHADER, fragmentShaderSource};

        // link shaders
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader.resource().handle().index());
        glAttachShader(shaderProgram, fragmentShader.resource().handle().index());
        glLinkProgram(shaderProgram);
        check_linking_success(shaderProgram);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left  
             0.5f, -0.5f, 0.0f, // right 
             0.0f,  0.5f, 0.0f  // top   
        };

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(&w.get());
            glfwPollEvents();
        }

        // de-allocate all resources once they've outlived their purpose:
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }
    catch(...)
    {
        std::cerr << "Unrecognized error\n";
    }
}


