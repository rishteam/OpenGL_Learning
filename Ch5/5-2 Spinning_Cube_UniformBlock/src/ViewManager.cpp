#include "../Include/ViewManager.h"

using namespace glm;

ViewManager::ViewManager()
{
    eyePosition = vec3(0.0f, 0.0f, 10.0f);
    eyeLookPosition = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0, 1, 0);
    viewMatrix = lookAt(eyePosition, eyeLookPosition, up);
    viewVector = eyePosition - eyeLookPosition;
	viewVector = normalize(viewVector);
}

mat4 ViewManager::GetModelMatrix() {
	return translationMatrix * rotationMatrix;
}

mat4 ViewManager::GetViewMatrix()
{
    return viewMatrix;
}

mat4 ViewManager::GetProjectionMatrix(float aspect)
{
    mat4 projectionMatrix;
    float nearVal;
    float farVal;
    nearVal = 0.1f;
    farVal = 15.0f;
    if(ortho) {
        float size = 1.5f * zoom;
        projectionMatrix = glm::ortho(-aspect * size, aspect * size, -size, size, nearVal, farVal);
    } 
	else {
        projectionMatrix = perspective(radians(30.0f * zoom), aspect, nearVal, farVal);
    }
    return projectionMatrix;
}

mat4 ViewManager::GetViewProjectionMatrix(float aspect)
{
    return GetProjectionMatrix(aspect) * viewMatrix;
}

mat4 ViewManager::GetModelViewProjectionMatrix(float aspect)
{
    return GetViewProjectionMatrix(aspect) * GetModelMatrix();
}

vec3 ViewManager::GetWorldEyePosition() {
	vec4 wordEyePosition = vec4(eyePosition, 0) * GetModelMatrix();
	return wordEyePosition.xyz;
}

vec3 ViewManager::GetWorldViewVector() {
	vec4 wordLookVector = vec4(-viewVector, 0) * GetModelMatrix();
	return wordLookVector.xyz;
}

void ViewManager::keyEvents(unsigned char key) {
	switch (key)
	{
	case 'w': 
	case 'W':
		Translate(vec2(0, moveSpeed));
		break;
	case 'a':
	case 'A':
		Translate(vec2(moveSpeed, 0));
		break;
	case 's':
	case 'S':
		Translate(vec2(0, -moveSpeed));
		break;
	case 'd':
	case 'D':
		Translate(vec2(-moveSpeed, 0));
		break;
	case '+':
		wheelEvent(-moveSpeed);
		break;
	case '-':
		wheelEvent(moveSpeed);
		break;
	default:
		break;
	}
}

void ViewManager::mouseEvents(int button, int state, int x, int y) {

	if (state == GLUT_UP)
	{
		mouseReleaseEvent(button, x, y);
	}
	else if (state == GLUT_DOWN) 
	{
		mousePressEvent(button, x, y);
	}
	if (button == 4) 
	{
		wheelEvent(1);
	}
	else if (button == 3)
	{
		wheelEvent(-1);
	}
}

void ViewManager::mousePressEvent(int button, int x, int y)
{
    if( button == GLUT_LEFT_BUTTON)
	{
        lmbDown = true;
        lmbDownCoord = vec2(x,y);
        mat4 invrtRot = inverse(rotationMatrix);
        rotateYAxis = (invrtRot * vec4(0, 1, 0, 0)).xyz;
        rotateXAxis = (invrtRot * vec4(1, 0, 0, 0)).xyz;
    } 
	else if(button == GLUT_MIDDLE_BUTTON)
	{
        midDown = true;
        midDownCoord = vec2(x, y);
    }
}

void ViewManager::mouseReleaseEvent(int button, int x, int y)
{
    if( button == GLUT_LEFT_BUTTON)
	{
        lmbDown = false;
    }
	else if(button == GLUT_MIDDLE_BUTTON || button == 3 || button == 4) {
        midDown = false;
    }
}

void ViewManager::mouseMoveEvent(int x,int y)
{
    if(lmbDown)
	{
        vec2 coord = vec2(x, y);
		vec2 diff = coord - lmbDownCoord;
        float factor = 0.002f;
        rotationMatrix = rotate(rotationMatrix,diff.x * factor, rotateYAxis);
        rotationMatrix = rotate(rotationMatrix,diff.y * factor, rotateXAxis);
        lmbDownCoord = coord;
    }
	else if(midDown)
	{
		vec2 coord = vec2(x,y);
		vec2 diff = coord - midDownCoord;

		vec4 up = vec4(0, 1, 0, 0);
		vec4 right = vec4(1, 0, 0, 0);

		vec3 diffUp = up.xyz * diff.y / (float)w_height;
		vec3 diffRight = right.xyz * diff.x / (float)w_width;

        translationMatrix = translate(translationMatrix, (-diffUp + diffRight) * zoom * 3.0f);
        midDownCoord = coord;
    }
}

void ViewManager::wheelEvent(int direction)
{
	wheel_val = direction * 15.0f;
    Zoom(wheel_val / 120.0f);
}

void ViewManager::Zoom(float distance)
{
    zoom *= (1.0f + 0.05f * distance);
    zoom = clamp(0.1f, zoom, 3.0f);
}

void ViewManager::SetWindowSize(int width,int height) {
	w_width = width;
	w_height = height;
}

void ViewManager::SetRotation(float theta, float phi)
{
    rotationMatrix = mat4(1.0);
    rotationMatrix = rotate(rotationMatrix, theta, vec3(0, 1, 0));
    rotationMatrix = rotate(rotationMatrix, phi, vec3(1, 0, 0));
}

void ViewManager::SetRotation(float x, float y, float z)
{
	vec3 v(x, y, z);
    v = normalize(v);
	vec3 o(0, 0, 1);
    double angle = acos(dot(v, o));
    rotationMatrix = mat4(1.0);
    rotationMatrix = rotate(rotationMatrix, (float)angle, cross(o, v));
}

void ViewManager::Reset()
{
	wheel_val = 0.0f;
    zoom = 1.0f;
    translationMatrix = mat4(1.0);
    rotationMatrix = mat4(1.0);
}

void ViewManager::Translate(vec2 vec) {
	vec2 diff = vec;

	vec4 up = vec4(0, 1, 0, 0);
	vec4 right = vec4(1, 0, 0, 0);

	vec3 diffUp = up.xyz * diff.y / (float)w_height;
	vec3 diffRight = right.xyz * diff.x / (float)w_width;

	translationMatrix = translate(translationMatrix, (-diffUp + diffRight) * zoom * 3.0f);
}