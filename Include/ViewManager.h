#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#define GLM_SWIZZLE
#include "GLM/glm/glm.hpp"
#include "GLM/glm/gtc/matrix_transform.hpp"
#include "GLM/glm/gtc/type_ptr.hpp"
#include "GLM/glm/gtx/rotate_vector.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <algorithm>

/**
 * @brief The ViewManager class
 * The ViewManager class provides viewing manipulation related functionalities.
 *
 * To use the ViewManager class, call mousePressEvent(),
 * mouseReleaseEvent(), mouseMoveEvent(), wheelEvent() in your event handlers
 * with the event objects.
 *
 * The viewing manipulation will be done for you in the class. When you are ready
 * to render something, call GetModelMatrix(), GetViewMatrix(), GetProjectionMatrix()
 * and their composite versions to get the MVP matrices which ecode current viewing
 * properties.
 */

class ViewManager
{
public:
    ViewManager();

	void mouseEvents(int button, int state, int x, int y);
	void mousePressEvent(int button, int x, int y);
	void mouseReleaseEvent(int button, int x, int y);
    void mouseMoveEvent(int x,int y);
    void wheelEvent(int direction);

	glm::mat4 GetModelMatrix();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix(float aspect);
	glm::mat4 GetViewProjectionMatrix(float aspect);
	glm::mat4 GetModelViewProjectionMatrix(float aspect);
	glm::vec3 GetEyePosition() {return eyePosition;}
	glm::vec3 GetViewVector() {return viewVector;}

	glm::vec3 GetWorldEyePosition();
	glm::vec3 GetWorldViewVector();

    bool IsOrthoProjection() { return ortho; }

    void SetRotation(float theta, float phi);
    void SetRotation(float x, float y, float z);
	void SetWindowSize(int width, int height);

    bool ToggleOrtho() { return ortho = !ortho; }
    void Zoom(float distance);
    void Reset();

private:
    bool ortho = false;
    float zoom = 1.0f;

	glm::mat4 translationMatrix;
	glm::mat4 rotationMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 viewVector;
	glm::vec3 rotateXAxis;
	glm::vec3 rotateYAxis;
	glm::vec3 eyePosition;
	glm::vec3 eyeLookPosition;

    bool lmbDown = false;
    bool midDown = false;
	glm::vec2 lmbDownCoord;
	glm::vec2 midDownCoord;
	
	int w_width;
	int w_height;
	float wheel_val;
};


#endif // VIEWMANAGER_H