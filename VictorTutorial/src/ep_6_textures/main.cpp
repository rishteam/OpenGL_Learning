#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>

#include "Ref.h"
#include "Utils.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Utils.h"
#include "Texture.h"

GLfloat vertices[] =
{
    // COORDINATES       /     COLORS          /   TexCoord
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // 0 Lower left corner
    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // 1 Upper left corner
    0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // 2 Upper right corner
    0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // 3 Lower right corner
};

GLuint indices[] = {
    0, 2, 1, // Upper triangle
    0, 3, 2 // Lower triangle
};

void GLAPIENTRY MessageCallback( GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar* message,
                                 const void* userParam )
{
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    /* Load and create the shaders */
    Ref<Shader> shader = MakeRef<Shader>();
    shader->Load("default.vert", "default.frag");

    Ref<Texture> texture = MakeRef<Texture>("../assets/dev.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    texture->TexUnit(shader, "tex0", 0);

    /* Create VAO, VBO, and EBO */
    Ref<VertexArray> vao = MakeRef<VertexArray>();
    Ref<VertexBuffer> vbo = MakeRef<VertexBuffer>();
    Ref<IndexBuffer> ibo = MakeRef<IndexBuffer>();

    vao->BindVertexBuffer(vbo);
    vbo->Store(vertices, sizeof(vertices));

    vao->BindIndexBuffer(ibo);
    ibo->Store(indices, sizeof(indices));

    /* Setting Vertex Attribute Array */
    vao->BindVertexAttribute(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao->BindVertexAttribute(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao->BindVertexAttribute(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    /* Unbind things */
    vao->Unbind();
    IndexBuffer::Clear();
    VertexBuffer::Clear();

    GLuint scaleID = glGetUniformLocation(shader->GetID(), "scale");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //
        glClearColor(0.5f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        texture->Bind();
        shader->Activate();
        glUniform1f(scaleID, 1.f);

        vao->Bind();
        // Draw primitives, number of indices, type, index of indices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    shader->Delete();
    vao->Delete();
    vbo->Delete();
    ibo->Delete();

    // TODO: The Refs will delete after the following calls. Will this a bug?
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}