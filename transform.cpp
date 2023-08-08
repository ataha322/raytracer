#include "transform.h"

/* general rotation matrix around an axis */
glm::mat3 Transform::rotate(const float degrees, const glm::vec3& axis) 
{
	float angle = glm::radians(degrees);
	float cos_theta = glm::cos(angle);
	float sin_theta = glm::sin(angle);
	glm::vec3 norm_axis = glm::normalize(axis);

	glm::mat3 along = glm::mat3(
			norm_axis.x * norm_axis.x, norm_axis.x * norm_axis.y, norm_axis.x * norm_axis.z,
			norm_axis.x * norm_axis.y, norm_axis.y * norm_axis.y, norm_axis.y * norm_axis.z,
			norm_axis.x * norm_axis.z, norm_axis.y * norm_axis.z, norm_axis.z * norm_axis.z
			);

	glm::mat3 perpendicular = glm::mat3(
			0.0, norm_axis.z, -norm_axis.y,
			-norm_axis.z, 0.0, norm_axis.x,
			norm_axis.y, -norm_axis.x, 0.0
			);
	
	glm::mat3 rot = 
		(cos_theta * glm::mat3(1.0)) +
		((1.0f - cos_theta) * along) +
		(sin_theta * perpendicular);
	
	return rot;
}

glm::mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
	return glm::mat4(
			sx, 0, 0, 0,
			0, sy, 0, 0,
			0, 0, sz, 0,
			0, 0, 0, 1
			);
}

glm::mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
	return glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			tx, ty, tz, 1
			);
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
