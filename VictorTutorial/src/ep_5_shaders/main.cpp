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

GLfloat vertices[] =
      { //               COORDINATES                  /     COLORS              //
        -0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f,    // [0] Lower left corner
        0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f,     // [1] Lower right corner
        0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f,     // [2] Upper corner
        -0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f,    // [3] Inner left
        0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f,     // [4] Inner right
        0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f      // [5] Inner down
};

GLuint indices[] = {
    0, 3, 5,
    3, 2, 4,
    5, 4, 1
};

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    /* Load and create the shaders */
    Ref<Shader> shader = MakeRef<Shader>();
    shader->Load("default.vert", "default.frag");

    /* Create VAO, VBO, and EBO */
    Ref<VertexArray> vao = MakeRef<VertexArray>();
    Ref<VertexBuffer> vbo = MakeRef<VertexBuffer>();
    Ref<IndexBuffer> ibo = MakeRef<IndexBuffer>();

    vao->BindVertexBuffer(vbo);
    vbo->Store(vertices, sizeof(vertices));

    vao->BindIndexBuffer(ibo);
    ibo->Store(indices, sizeof(indices));

    /* Setting Vertex Attribute Array */
    vao->BindVertexAttribute(0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    vao->BindVertexAttribute(1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    /* Unbind things */
    vao->Unbind();
    IndexBuffer::Clear();
    VertexBuffer::Clear();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //
        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->Activate();
        vao->Bind();
        // Draw primitives, number of indices, type, index of indices
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

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