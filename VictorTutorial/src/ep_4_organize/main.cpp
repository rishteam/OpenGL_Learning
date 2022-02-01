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
{
    -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,    // [0] Lower left corner
    0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,     // [1] Lower right corner
    0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  // [2] Upper corner
    -0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f, // [3] Inner left
    0.5f / 2, 0.5f * float(sqrt(3)) / 6, 0.0f,  // [4] Inner right
    0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f      // [5] Inner down
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
    glVertexAttribPointer(0,   // attribute index
      3,                       // Number of component (here is vec3)
      GL_FLOAT,                // Type of each component
      GL_FALSE,                // Is normalized
      3 * sizeof(float),       // Stride (Offset to the next group of the attribute)
      (void*)0);               // Offset to the first attribute
    glEnableVertexAttribArray(0);

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