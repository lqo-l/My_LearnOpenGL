#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

const float YAW = -90.f;
const float PITCH = 0.f;
const float SPEED = 4.f;
const float SENSITIVITY_X = 0.02f;
const float SENSITIVITY_Y = 0.03f;
const float ZOOM = 45.f;

class Camera {
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front; // -z
	glm::vec3 Up; // 相机坐标系的y方向
	glm::vec3 Right;
	glm::vec3 WorldUp; // 用于计算右向量的上向量，非y轴 如（0,1,0）
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivityX;
	float MouseSensitivityY;
	float Zoom;

	//向量构造(位置、上、代表前朝向的角）
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// 标量构造
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	// lookAt构造
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch);
	void ProcessMouseScroll(float yOffset);
	glm::mat4 LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up);
private:
	// 从欧拉角计算Front等坐标轴向量
	void updateCameraVectors();
};

inline Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) {
	// 相机方向参数
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	// 默认参数
	MovementSpeed = SPEED;
	MouseSensitivityX = SENSITIVITY_X;
	MouseSensitivityY = SENSITIVITY_Y;
	Zoom = ZOOM;

	updateCameraVectors();
}

inline Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
	// 相机方向参数
	Position = glm::vec3{ posX, posY, posZ };
	WorldUp = glm::vec3{ upX, upY, upZ };
	Yaw = yaw;
	Pitch = pitch;
	// 默认参数
	MovementSpeed = SPEED;
	MouseSensitivityX = SENSITIVITY_X;
	MouseSensitivityY = SENSITIVITY_Y;
	Zoom = ZOOM;

	updateCameraVectors();
}

inline Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	// 相机方向参数
	Position = position;
	WorldUp = up;
	Front = glm::normalize(target - position);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

	Pitch = glm::degrees(asin(glm::clamp<float>(Front.y,glm::sin(glm::radians(-89.f)), glm::sin(glm::radians(89.f))))); // 需要考虑pitch是否超出正负89度范围
	Yaw = glm::degrees(atan2(Front.z, Front.x)); //注：如果用asin的话,斜边不是1，是1*cos(pitch)

	// 默认参数
	MovementSpeed = SPEED;
	MouseSensitivityX = SENSITIVITY_X;
	MouseSensitivityY = SENSITIVITY_Y;
	Zoom = ZOOM;
}

inline glm::mat4 Camera::GetViewMatrix() {
	glm::mat4 R_w2c(
		Right.x, Up.x, -Front.x, 0.0f, // 相机Front方向是z轴反方向
		Right.y, Up.y, -Front.y, 0.0f,
		Right.z, Up.z, -Front.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	glm::mat4 T_w2c(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-Position.x, -Position.y, -Position.z, 1.f
	);
	return R_w2c * T_w2c; // 已有Right位置，不调api重新计算right，手动构建

	//return glm::lookAt(Position, Position + Front, Up); // 原：调api返回view
}

// WASDQE
inline void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	float velocity = MovementSpeed * deltaTime;
	switch (direction) {
	case FORWARD:
		Position += Front * velocity;
		break;
	case BACKWARD:
		Position -= Front * velocity;
		break;
	case LEFT:
		Position -= Right * velocity;
		break;
	case RIGHT:
		Position += Right * velocity;
		break;
	case UP:
		Position += Up * velocity;
		break;
	case DOWN:
		Position -= Up * velocity;
		break;
	}
}

inline void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
	xOffset = xOffset * MouseSensitivityX;
	yOffset = yOffset * MouseSensitivityY;
	Yaw += xOffset; // 角度制
	Pitch += yOffset;
	if (constrainPitch) {
		Pitch = glm::clamp<float>(Pitch, -89.0, 89.0); // 限制不超过89度，fps模式
	}
	updateCameraVectors();
}

inline void Camera::ProcessMouseScroll(float yOffset) {
	Zoom -= yOffset;
	Zoom = glm::clamp<float>(Zoom, 1.0f, 70.0f);
}

inline glm::mat4 Camera::LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 z_axis = glm::normalize(position - target); // -front
	glm::vec3 x_axis = glm::normalize(glm::cross(up, z_axis));
	glm::vec3 y_axis = glm::cross(z_axis, x_axis);

	glm::mat4 R_w2c( // 列优先，把这里输入的每一行当成手写的每一列就行
		x_axis.x, y_axis.x, z_axis.x, 0.0f,
		x_axis.y, y_axis.y, z_axis.y, 0.0f,
		x_axis.z, y_axis.z, z_axis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	glm::mat4 T_w2c(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		-position.x, -position.y, -position.z, 1.f
	);
	return R_w2c * T_w2c; // 先平移后旋转，因为是w2c的逆（w2c就是相机物体的模型变换矩阵，遵循先旋转后平移，取逆后是这里的先平移后旋转）
}

inline void Camera::updateCameraVectors() {
	Front = glm::normalize(glm::vec3{
		cos(glm::radians(Pitch)) * cos(glm::radians(Yaw)),
		sin(glm::radians(Pitch)),
		cos(glm::radians(Pitch)) * sin(glm::radians(Yaw))
		});
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::cross(Right, Front); // Right和Front垂直，所以这里不用归一化
}