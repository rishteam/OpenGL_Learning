#include <iostream>
#include <cmath>
#include <functional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include "shader.h"
#include "VertexArray.h"
#include "camera.h"
#include "player.h"
#include "Texture2D.h"

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}

#include "vertices.h"

glm::vec3 lightPos(1.2f, 1.0f, 2.f);

const uint32_t screenWidth = 1280, screenHeight = 720;

sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));

glm::vec3 lightColor(1.f, 1.f, 1.f);
glm::vec3 objectColor(1, 0.5, 0.31);
glm::vec3 lightDir(1.f, -2.f, 1.f);

glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 pointLightPositions[] = {
        glm::vec3( 1.f,  0.0f,  0.0f),
        glm::vec3( 0.f, 1.f, 0.f),
        glm::vec3(0.0f,  .0f, 1.f),
        glm::vec3( 1.0f,  1.0f, 1.0f)
};

glm::vec3 pointLightColor[] = {

        glm::vec3( 1.f,  1.0f,  1.0f),
        glm::vec3( 1.f,  1.0f,  1.0f),
        glm::vec3( 1.f,  1.0f,  1.0f),
        glm::vec3( 1.f,  1.0f,  1.0f)
};

int main()
{
    std::cout << std::filesystem::current_path();
    window.setVerticalSyncEnabled(true);
    // Load OpenGL functions using glad
    if (!gladLoadGL())
    {
        printf("Something went wrong!\n");
        exit(-1);
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::SFML::Init(window);
    window.setActive(true);
    init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader("shader/8.6.Multiple_lights/lighting.vs", "shader/8.6.Multiple_lights/lighting.fs");
    Shader shader2("shader/8.6.Multiple_lights/light_cube.vs", "shader/8.6.Multiple_lights/light_cube.fs");
    VertexArray vertexArray;
    BufferLayout layout = {
            {ShaderDataType::Float3, "aPos"},
            {ShaderDataType::Float3, "aNormal"},
            {ShaderDataType::Float2, "aTexCoords"}
    };
    VertexBuffer vertex(vertices, sizeof(vertices));
    vertex.setLayout(layout);
    vertexArray.addVertexBuffer(&vertex);
    const std::string path = "assets/box.png";
    auto LoadTexture = [&](std::string path) {
        sf::Image img;
        img.loadFromFile(path);
        img.flipVertically();
        // generate a texture
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // texture parameter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)img.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);
        return texture;
    };

    uint32_t texture = LoadTexture("assets/box.png");
    uint32_t texture2 = LoadTexture("assets/boxOutline.png");

    FirstPersonView fpsView;

    sf::Clock stClk;
    sf::Clock deltaClock;
    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                running = false;
            else if (event.type == sf::Event::Resized)
                glViewport(0, 0, event.size.width, event.size.height);

            fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("6.2.cube");
        // background color
        static float bgColor[4] = {0, 0, 0, 0};
        ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
        ImGui::End();

        ImGui::Begin("Matrix");
        //
        glm::mat4 model(1.f), view(1.f), projection(1.f);
//        ImGui::Text("Model");
        static float rotate_x = 0.0f, rotate_y = 0.f, rotate_z = 0.f;
        static glm::vec3 obTrans = {-1.f, 1.f, 1.f};
        float step = 100 * stClk.getElapsedTime().asSeconds();
//        ImGui::DragFloat("Rotate x", &rotate_x, 1.0, -360, 360);
//        ImGui::DragFloat("Rotate y", &rotate_y, 1.0 , - 360, 360);
//        ImGui::DragFloat("Rotate z", &rotate_z, 1.0, - 360, 360);
//        ImGui::DragFloat("Pos x", &obTrans.x, .1);
//        ImGui::DragFloat("Pos y", &obTrans.y, .1);
//        ImGui::DragFloat("pos z", &obTrans.z, .1);
//        model = glm::translate(model, obTrans);
//        model = glm::rotate(model, glm::radians(rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
//        model = glm::rotate(model, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));
        ImGui::Separator();
        static float trans[3] = {0.0f, 0.0f, 0.0f};
        ImGui::Text("View");
        ImGui::SliderFloat3("Translate", trans, -100.f, 100.f);
        view = glm::translate(view, glm::vec3(trans[0], trans[1], trans[2]));

        ImGui::PushItemWidth(100);
        static glm::vec3 materialSpecular(0.5, 0.5, 0.5);
        ImGui::Text("materialSpecular");
        ImGui::DragFloat("x##materialSpecular", &materialSpecular.x, 0.1); ImGui::SameLine();
        ImGui::DragFloat("y##materialSpecular", &materialSpecular.y, 0.1); ImGui::SameLine();
        ImGui::DragFloat("z##materialSpecular", &materialSpecular.z, 0.1);

        ImGui::PopItemWidth();

        static float materialShininess = 32.f;
        ImGui::Text("materialShininess");
        ImGui::DragFloat("materialShininess", &materialShininess, 0.5f, 0, 256);

        ImGui::End();
        ImGui::Begin("Camera");
        ImGui::Text("View");
        view = fpsView.getViewMatrix();
        ImGui::DragFloat3("Camera", glm::value_ptr(fpsView.m_camera.m_pos));
        ImGui::DragFloat("Speed", &fpsView.cameraSpeed);
        ImGui::DragFloat("Sensitivity", &fpsView.sensitivity);
        // Projection matrix
        ImGui::Text("Projection");
        ImGui::DragFloat("FOV", &fpsView.m_camera.m_fov, 1.0f, 0.f, 360.f);
        ImGui::DragFloat("Near", &fpsView.m_camera.m_near, 0.01);
        ImGui::DragFloat("Far", &fpsView.m_camera.m_far, 0.1);
        ImGui::Text("fpsView.m_camera.m_screenRatio = %f", fpsView.m_camera.m_screenRatio);
        projection = fpsView.getProjectionMatrix();

        ImGui::End();

        ImGui::Begin("Light Attribute");

        static glm::vec3 dirLight_direction(-0.2, -.1, -0.3);
        static glm::vec3 dirLight_ambient(0.05f, 0.05f, 0.05f);
        static glm::vec3 dirLight_diffuse(0.4f, 0.4f, 0.4f);
        static glm::vec3 dirLight_specular(0.5f, 0.5f, 0.5f);
        ImGui::PushItemWidth(100);
        if(ImGui::CollapsingHeader("Direct Light")) {

            ImGui::Text("Direct Direction");
            ImGui::DragFloat("x##Direct Direction", &dirLight_direction.x); ImGui::SameLine();
            ImGui::DragFloat("y##Direct Direction", &dirLight_direction.y); ImGui::SameLine();
            ImGui::DragFloat("z##Direct Direction", &dirLight_direction.z);

            ImGui::Text("Direct Ambient");
            ImGui::DragFloat("x##Direct Ambient", &dirLight_ambient.x); ImGui::SameLine();
            ImGui::DragFloat("y##Direct Ambient", &dirLight_ambient.y); ImGui::SameLine();
            ImGui::DragFloat("z##Direct Ambient", &dirLight_ambient.z);

            ImGui::Text("Direct Diffuse");
            ImGui::DragFloat("x##Direct Diffuse", &dirLight_diffuse.x); ImGui::SameLine();
            ImGui::DragFloat("y##Direct Diffuse", &dirLight_diffuse.y); ImGui::SameLine();
            ImGui::DragFloat("z##Direct Diffuse", &dirLight_diffuse.z);

            ImGui::Text("Direct Specular");
            ImGui::DragFloat("x##Direct Specular", &dirLight_specular.x); ImGui::SameLine();
            ImGui::DragFloat("y##Direct Specular", &dirLight_specular.y); ImGui::SameLine();
            ImGui::DragFloat("z##Direct Specular", &dirLight_specular.z);
        }

        static glm::vec3 pointLight_ambient(0.05f, 0.05f, 0.05f);
        static glm::vec3 pointLight_diffuse(0.8f, 0.8f, 0.8f);
        static glm::vec3 pointLight_specular(1.f, 1.f, 1.f);
        static float pointLight_constant = 1.0f;
        static float pointLight_linear = 0.09f;
        static float pointLight_quadratic = 0.032f;
        if(ImGui::CollapsingHeader("Point Lights")) {

            if(ImGui::CollapsingHeader("Point 1")) {

                ImGui::DragFloat("x##Point 1", &pointLightPositions[0].x, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("y##Point 1", &pointLightPositions[0].y, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("z##Point 1", &pointLightPositions[0].z, 0.1f);
                ImGui::ColorEdit3("color##Point 1", glm::value_ptr(pointLightColor[0]));
            }

            if(ImGui::CollapsingHeader("Point 2")) {

                ImGui::DragFloat("x##Point 2", &pointLightPositions[1].x, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("y##Point 2", &pointLightPositions[1].y, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("z##Point 2", &pointLightPositions[1].z, 0.1f);
                ImGui::ColorEdit3("color##Point 2", glm::value_ptr(pointLightColor[1]));
            }

            if(ImGui::CollapsingHeader("Point 3")) {

                ImGui::DragFloat("x##Point 3", &pointLightPositions[2].x, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("y##Point 3", &pointLightPositions[2].y, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("z##Point 3", &pointLightPositions[2].z, 0.1f);
                ImGui::ColorEdit3("color##Point 3", glm::value_ptr(pointLightColor[2]));
            }

            if(ImGui::CollapsingHeader("Point 4")) {

                ImGui::DragFloat("x##Point 4", &pointLightPositions[3].x, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("y##Point 4", &pointLightPositions[3].y, 0.1f); ImGui::SameLine();
                ImGui::DragFloat("z##Point 4", &pointLightPositions[3].z, 0.1f);
                ImGui::ColorEdit3("color##Point 4", glm::value_ptr(pointLightColor[3]));
            }


            ImGui::Text("Point Ambient");
            ImGui::DragFloat("x##Point Ambient", &pointLight_ambient.x); ImGui::SameLine();
            ImGui::DragFloat("y##Point Ambient", &pointLight_ambient.y); ImGui::SameLine();
            ImGui::DragFloat("z##Point Ambient", &pointLight_ambient.z);

            ImGui::Text("Point Diffuse");
            ImGui::DragFloat("x##Point Diffuse", &pointLight_diffuse.x); ImGui::SameLine();
            ImGui::DragFloat("y##Point Diffuse", &pointLight_diffuse.y); ImGui::SameLine();
            ImGui::DragFloat("z##Point Diffuse", &pointLight_diffuse.z);

            ImGui::Text("Point Specular");
            ImGui::DragFloat("x##Point Specular", &pointLight_specular.x); ImGui::SameLine();
            ImGui::DragFloat("y##Point Specular", &pointLight_specular.y); ImGui::SameLine();
            ImGui::DragFloat("z##Point Specular", &pointLight_specular.z);

            ImGui::Text("Point Constant");
            ImGui::DragFloat("constant##Point Constant", &pointLight_constant);
            ImGui::Text("Point Linear");
            ImGui::DragFloat("Linear##Point Linear", &pointLight_linear);
            ImGui::Text("Point Quadratic");
            ImGui::DragFloat("Quadratic##Point Quadratic", &pointLight_quadratic);
        }

        static glm::vec3 spotLight_ambient(0.2f, 0.2f, 0.2f);
        static glm::vec3 spotLight_diffuse(0.8f, 0.8f, 0.8f);
        static glm::vec3 spotLight_specular(1.f, 1.f, 1.f);
        static float spotLight_constant = 1.0f;
        static float spotLight_linear = 0.09f;
        static float spotLight_quadratic = 0.032f;
        static float spotLight_cutOff = 12.5f;
        static float spotLight_outerCutOff = 15.f;
        if(ImGui::CollapsingHeader("Spot Light")) {

            ImGui::Text("Spot Ambient");
            ImGui::DragFloat("x##Spot Ambient", &spotLight_ambient.x); ImGui::SameLine();
            ImGui::DragFloat("y##Spot Ambient", &spotLight_ambient.y); ImGui::SameLine();
            ImGui::DragFloat("z##Spot Ambient", &spotLight_ambient.z);

            ImGui::Text("Spot Diffuse");
            ImGui::DragFloat("x##Spot Diffuse", &spotLight_specular.x); ImGui::SameLine();
            ImGui::DragFloat("y##Spot Diffuse", &spotLight_specular.y); ImGui::SameLine();
            ImGui::DragFloat("z##Spot Diffuse", &spotLight_specular.z);

            ImGui::Text("Spot Specular");
            ImGui::DragFloat("x##Spot Specular", &spotLight_specular.x); ImGui::SameLine();
            ImGui::DragFloat("y##Spot Specular", &spotLight_specular.y); ImGui::SameLine();
            ImGui::DragFloat("z##Spot Specular", &spotLight_specular.z);

            ImGui::Text("Spot Constant");
            ImGui::DragFloat("constant##Point Constant", &spotLight_constant);
            ImGui::Text("Spot Linear");
            ImGui::DragFloat("constant##Spot Linear", &spotLight_linear);
            ImGui::Text("Spot Quadratic");
            ImGui::DragFloat("constant##Spot Quadratic", &spotLight_quadratic);
            ImGui::Text("Spot CutOff");
            ImGui::DragFloat("constant##Spot CutOff", &spotLight_cutOff);
            ImGui::Text("Spot OuterCutOff");
            ImGui::DragFloat("constant##Spot OuterCutOff", &spotLight_outerCutOff);
        }

        ImGui::PopItemWidth();
        ImGui::End();

        glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        texture.bind();
        fpsView.update(dt);
        // Upload uniforms
        shader.bind();
//        shader.setMat4("vModel", model);
        shader.setMat4("vView", view);
        shader.setMat4("vProjection", projection);
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", materialShininess);
        shader.setFloat3("viewPos", fpsView.m_camera.m_front);

        shader.setFloat3("dirLight.direction", dirLight_direction);
        shader.setFloat3("dirLight.ambient", dirLight_ambient);
        shader.setFloat3("dirLight.diffuse", dirLight_diffuse);
        shader.setFloat3("dirLight.specular", dirLight_specular);
        // point 1
        shader.setFloat3("pointLight[0].position", pointLightPositions[0]);
        shader.setFloat3("pointLight[0].ambient", pointLight_ambient);
        shader.setFloat3("pointLight[0].diffuse", pointLight_diffuse);
        shader.setFloat3("pointLight[0].specular", pointLight_specular);
        shader.setFloat("pointLight[0].constant", pointLight_constant);
        shader.setFloat("pointLight[0].linear", pointLight_linear);
        shader.setFloat("pointLight[0].quadratic", pointLight_quadratic);
        // point 2
        shader.setFloat3("pointLight[1].position", pointLightPositions[1]);
        shader.setFloat3("pointLight[1].ambient", pointLight_ambient);
        shader.setFloat3("pointLight[1].diffuse", pointLight_diffuse);
        shader.setFloat3("pointLight[1].specular", pointLight_specular);
        shader.setFloat("pointLight[1].constant", pointLight_constant);
        shader.setFloat("pointLight[1].linear", pointLight_linear);
        shader.setFloat("pointLight[1].quadratic", pointLight_quadratic);
        // point 3
        shader.setFloat3("pointLight[2].position", pointLightPositions[2]);
        shader.setFloat3("pointLight[2].ambient", pointLight_ambient);
        shader.setFloat3("pointLight[2].diffuse", pointLight_diffuse);
        shader.setFloat3("pointLight[2].specular", pointLight_specular);
        shader.setFloat("pointLight[2].constant", pointLight_constant);
        shader.setFloat("pointLight[2].linear", pointLight_linear);
        shader.setFloat("pointLight[2].quadratic", pointLight_quadratic);
        // point 4
        shader.setFloat3("pointLight[3].position", pointLightPositions[3]);
        shader.setFloat3("pointLight[3].ambient", pointLight_ambient);
        shader.setFloat3("pointLight[3].diffuse", pointLight_diffuse);
        shader.setFloat3("pointLight[3].specular", pointLight_specular);
        shader.setFloat("pointLight[3].constant", pointLight_constant);
        shader.setFloat("pointLight[3].linear", pointLight_linear);
        shader.setFloat("pointLight[3].quadratic", pointLight_quadratic);

        shader.setFloat3("spotLight.position", fpsView.m_camera.m_pos);
        shader.setFloat3("spotLight.direction", fpsView.m_camera.m_front);
        shader.setFloat3("spotLight.ambient", spotLight_ambient);
        shader.setFloat3("spotLight.diffuse", spotLight_diffuse);
        shader.setFloat3("spotLight.specular", spotLight_specular);
        shader.setFloat("spotLight.constant", spotLight_constant);
        shader.setFloat("spotLight.linear", spotLight_linear);
        shader.setFloat("spotLight.quadratic", spotLight_quadratic);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(spotLight_cutOff)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(spotLight_outerCutOff)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        vertexArray.bind();

        for(int i = 0 ; i < 10 ; i++) {

            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("vModel", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glDrawArrays(GL_TRIANGLES, 0, 36);

//        glBindTexture(GL_TEXTURE_2D, 0);

        shader2.bind();
        shader2.setMat4("vView", view);
        shader2.setMat4("vProjection", projection);

        for(int i = 0 ; i < 4 ; i++) {

            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            shader.setMat4("vModel", model);
            shader2.setFloat3("lightColor", pointLightColor[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        vertexArray.unbind();
        shader2.unbind();
//        shader2.bind();
        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

    return 0;
}