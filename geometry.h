// Force glm to use radians since usage of degrees is deprecated
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <FreeImage.h>

//ray class
class Ray { //Ray equation is P0 + P1*t
	public:
		glm::vec3 origin; //corresponds to P0
		glm::vec3 direction; //corresponds to P1

		//basic constructor
		Ray(const glm::vec3& orig, const glm::vec3& direct)
			: origin(orig), direction(direct) {}

		Ray() {}
};

//general object class for derivation of triangles and sphere classes
class Object{
	public:
		glm::mat4 trans; //transformation matrix
		glm::mat4 inv_trans; //inverse transformation matrix

		//material properties
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 emission;
		glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		float shininess;

		enum {triangle, sphere} type;

		Object() {}
		virtual bool hit(const Ray& ray, float& t_val);
		virtual glm::vec3 interpolate(const glm::vec3& P);
};

class Sphere : public Object {
	public:
		glm::vec3 center;
		float radius;

		Sphere() {}
		virtual bool hit(const Ray& ray, float& t_val) ;
		virtual glm::vec3 interpolate(const glm::vec3& P);

};

class Triangle : public Object {
	public: 
		glm::vec3 vertices[3];
		bool areNorms; //indicates whether vertices have norms
						//i.e. whether the next array is worth using
		glm::vec3 vertexnorms[3];

		Triangle() {}
		virtual bool hit(const Ray& ray, float& t_val);
		virtual glm::vec3 interpolate(const glm::vec3& P);

};

//color class
class Color {
	public:

		float R,G,B;


		Color() : R(0.0f), G(0.0f), B(0.0f) {}

		Color(float r, float g, float b) {
			R = r;
			G = g;
			B = b;
		}

		Color(glm::vec3 color) {
			R = color.x;
			G = color.y;
			B = color.z;
		}

		BYTE redByte() { 
			return std::min(R, 1.0f) * 255;
		}
		BYTE greenByte(){ 
			return std::min(G, 1.0f) * 255;
		}
		BYTE blueByte() {
			return std::min(B, 1.0f) * 255;
		}
};


//light class
class Light {
	public:
		glm::vec3 coord;
		Color color;
		enum {point, directional} type;
};

