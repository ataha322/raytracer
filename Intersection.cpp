#include "scene.h"

const Color WHITE = Color(1.0f, 1.0f, 1.0f);
const Color BLACK = Color(0.0f, 0.0f, 0.0f);
const Color RED = Color(1.0f, 0.0f, 0.0f);

// intersection class
class Intersection
{
public:
	bool isIntersected;
	Object *object;
	glm::vec3 coord;

	Intersection() : isIntersected(false), coord(glm::vec3(0, 0, 0)) {}

	void intersect(const Ray &ray, const Scene &scene);
};

Ray transform(const Ray& ray, const Object* obj); //helper to transform ray

void Intersection::intersect(const Ray &ray, const Scene &scene)
{
	float closest_dist = std::numeric_limits<float>::max();
	object = nullptr;
	isIntersected = false;

	for (Object *obj : scene.objects) {
		Ray trans_ray = transform(ray, obj);
		float t_val = 0;

		if (obj->hit(trans_ray, t_val)) {
			glm::vec3 P_prime  = trans_ray.origin + trans_ray.direction * t_val;
			glm::vec4 P_world4 = obj->trans * glm::vec4(P_prime, 1.0f);

			glm::vec3 P_world = glm::vec3(P_world4) / P_world4.w;

			t_val = glm::length(P_world - ray.origin);

			if (t_val < closest_dist) {
				closest_dist = t_val; // update closest distance to an object
				coord = P_world;
				object = obj;
				isIntersected = true;
			}
		}
	}
}

Ray transform(const Ray& ray, const Object* obj) {
    glm::vec4 trans_origin(ray.origin, 1.0f);
    glm::vec4 trans_direction(ray.direction, 0.0f);

    trans_origin = obj->inv_trans * trans_origin;
    trans_direction = obj->inv_trans * trans_direction;

	glm::vec3 ret_origin = glm::vec3(trans_origin) / trans_origin.w;
	glm::vec3 ret_direction = glm::vec3(trans_direction);
	
    return Ray(ret_origin, ret_direction);
}