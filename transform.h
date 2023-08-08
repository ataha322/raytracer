// Force glm to use radians since usage of degrees is deprecated
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float pi = 3.14159265 ; // For portability across platforms

class Transform  
{
	public:
		Transform();
		virtual ~Transform();
		static glm::mat3 rotate(const float degrees, const glm::vec3& axis) ;
		static glm::mat4 scale(const float &sx, const float &sy, const float &sz) ; 
		static glm::mat4 translate(const float &tx, const float &ty, const float &tz);
};

