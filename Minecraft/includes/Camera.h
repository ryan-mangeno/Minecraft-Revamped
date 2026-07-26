#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util.h"

enum MovementDir {
	NONE = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FORWARD,
	BACK
};




class Camera
{
public:


	glm::mat4 CalcViewMatrix() const;

	inline glm::vec3& GetPos() { return m_Position; }
	inline glm::mat4& GetProjMat() { return m_ProjectionMat; }
	inline float GetZoom() { return m_Zoom; }
	inline glm::vec3& GetOrientation() { return m_Orientation; }

	void DispatchKeyboardEvent(MovementDir direction, float deltaTime);
	void DispatchMouseMoveEvent(float xrot, float yrot);
	void DispatchMouseScrollEvent(float scroll);

	void OnUpdate(float deltaTime);

	// singleton for camera, only one camera for minecraft
	static Camera& GetCamera() {
		static Camera c(glm::vec3(0.0f, 25.0f, 0.0f), WIDTH, HEIGHT, 45.f, 0.1f, 100.f);
		return c;
	}

private:

	Camera(glm::vec3 position, int width, int height, float fov, float nearPlane, float farPlane);

	// projection stuff
	glm::mat4 m_MVP;
	glm::mat4 m_ProjectionMat;

	//not adding model mat, will define in maybe super class with multiplayer
	// the model mats will define the offset of the player models for each player in game


	// camera attribs
	glm::vec3 m_Position;
	glm::vec3 m_Orientation;
	glm::vec3 m_Right;
	glm::vec3 m_Up;

	// rotation angles
	float m_Yaw;
	float m_Pitch;

	unsigned int m_Width;
	unsigned int m_Height;

	float m_Speed;
	float m_Velocity;
	float m_Sensitivity;
	float m_Zoom;

	void updateCameraVectors();
};
