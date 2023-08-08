//header file for camera

#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "geometry.h"

using namespace std;

const float epsilon = 1e-4;

class Camera {
	public:
		glm::vec3 eye, center, up;
		float fovy;

		Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 up_dir, float fov_y);

		Camera();
};

// To be used when camera configuration is passed



class Scene {
	public:
		Camera cam;
		std::vector<Light> lights;
		std::vector<glm::vec3> vertices;
		std::vector<Object*> objects; //holds pointers to all objects 

		//for vertices with norms. one-to-one correspondence between indices.
		std::vector<glm::vec3> vertnorms;
		std::vector<glm::vec3> norms;

		glm::vec3 attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

		void rightmultiply(const glm::mat4 & M, stack<glm::mat4> &transfstack);
		bool readvals(stringstream &s, const int numvals, float* values); 
		void readfile(const char* filename);

		// The following are temporary storage variables for parsing
		int width = 256;
		int height = 256;
		int depth = 5;
		std::string outfilename = "raytrace.png";

		float sx, sy ; // the scale in x and y 
		float tx, ty ; // the translation in x and y

		// Materials (read from file) 
		// With multiple objects, these are colors for each.
		float ambient[3] ; 
		float diffuse[3] ; 
		float specular[3] ; 
		float emission[3] ; 
		float shininess = 0; 

		int maxverts; //max number of vertices for a triangle
		int maxvertnorms; //max number of vertices with normals
		float vertex[3]; //vertex with coords x,y,z
		float vertexnorm[6]; //vertex with coords x,y,z and its surface normals nx,ny,nz
};


