#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <vector>

enum class CameraDirection
{
	NONE = 0,
	FORWARD,
	BACKWARDS,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	glm::vec3 cameraPos;

	glm::vec3 front, up, right;

	float yaw, pitch, speed, zoom;
	float n, f;

	bool rotateCamera = false;

	Camera(glm::vec3 pos = glm::vec3(0.0f));

	void UpdateCameraDir(double dx, double dy);
	void UpdateCameraPos(CameraDirection d, double dt);
	void UpdateCameraZoom(double dy);

	__inline glm::mat4 view() { return glm::lookAt(cameraPos, cameraPos + front, up); };
	__inline glm::mat4 perspective(int w, int h)
	{
		return glm::perspective(glm::radians(zoom),
			static_cast<float>(w) / static_cast<float>(h), n, f);
	}

	__inline float GetZoom() { return zoom; };

private:
	void UpdateCameraVectors();
};

#endif