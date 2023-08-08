#include "Intersection.cpp"

void saveScreenshot(Scene& scene, BYTE* pixels);
BYTE* raytrace(Scene& scene); // the core function
Ray rayThruPixel(Scene& cam, int i, int j);
Color FindColor(const Intersection& hit); //test function
Color findColor(const Intersection& hit, const Ray &ray, const Scene &scene, int depth);

int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Usage: scene txt file as an argument\n"; 
		exit(-1); 
	}

	FreeImage_Initialise();

	Scene scene;
	scene.readfile(argv[1]);

	BYTE* pixels = raytrace(scene);

	saveScreenshot(scene, pixels);

	FreeImage_DeInitialise();

	delete [] pixels;

	for (Object* obj : scene.objects)
		delete obj;

	return 0;
}

//the main raytracing algorithm
BYTE* raytrace(Scene& scene) {

	BYTE* image = new BYTE[3 * scene.width * scene.height];

	for (int i = 0; i < scene.height; i++) {
		for (int j = 0; j < scene.width; j++) {
			Ray ray = rayThruPixel(scene, i, j);
			Intersection hit;
			hit.intersect(ray,scene);
			Color color = findColor(hit, ray, scene, 0);
			int byte_index = 3 * ((scene.height-i-1) * scene.width + j);
			image[byte_index] = color.blueByte();
			image[byte_index+1] = color.greenByte();
			image[byte_index+2] = color.redByte();
		}
	}
	return image;
}

Ray rayThruPixel(Scene& scene, int i, int j) {
	//construct orthonormal basis
	glm::vec3 w = glm::normalize(scene.cam.eye - scene.cam.center);
	glm::vec3 u = glm::normalize(glm::cross(scene.cam.up, w));
	glm::vec3 v = glm::cross(w,u);

	//find coeffs alpha and beta for the equation:
	//ray = eye + norm(alpha*u + beta*v - w) * t

	float tany = glm::tan(glm::radians(scene.cam.fovy) / 2.0f);
	float tanx = tany * ((float)scene.width / (float)scene.height);

	float alpha = tanx * (((j+0.5f)-(scene.width/2.0f)) / (scene.width/2.0f));
	float beta = tany * (((scene.height/2.0f)-(i+0.5f)) / (scene.height/2.0f));

	glm::vec3 direction;

	direction = glm::normalize(alpha*u + beta*v - w);

	return Ray(scene.cam.eye, direction);
}

void saveScreenshot(Scene& scene, BYTE* pixels) {
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, scene.width, scene.height, scene.width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
	std::cout << "Saving screenshot: " << scene.outfilename << std::endl;
	FreeImage_Save(FIF_PNG, img, scene.outfilename.c_str(), 0);
}

Color helpFindColor(const Light &light, const Intersection &hit, const Ray &ray, const glm::vec3& attenuation);

Color findColor(const Intersection &hit, const Ray &ray, const Scene &scene, int depth)
{
    if (depth > scene.depth)
        return BLACK; // base of recursion

    if (!hit.isIntersected)
        return BLACK;

    Color color(hit.object->ambient + hit.object->emission);
    for (Light light : scene.lights)
    {
        if (light.type == Light::point)
        {
            Ray light_ray(light.coord, glm::normalize(hit.coord - light.coord));

            Intersection new_hit;
            new_hit.intersect(light_ray, scene);

            if (new_hit.isIntersected)
            {
                if (glm::dot(hit.coord - new_hit.coord, hit.coord - new_hit.coord) < epsilon)
                {
                    Color tmp_col = helpFindColor(light, hit, ray, scene.attenuation);
                    color.R += tmp_col.R;
                    color.G += tmp_col.G;
                    color.B += tmp_col.B;
                }
            }
        }
        else if (light.type == Light::directional)
        {
            Color tmp_col = helpFindColor(light, hit, ray, scene.attenuation);
            color.R += tmp_col.R;
            color.G += tmp_col.G;
            color.B += tmp_col.B;
        }
    }

    float spec_r = hit.object->specular.x;
    float spec_g = hit.object->specular.y;
    float spec_b = hit.object->specular.z;

    bool isZero = spec_r < epsilon && spec_g < epsilon && spec_b < epsilon;

    if (isZero)
    {
        glm::vec3 norm = hit.object->interpolate(hit.coord);
        glm::vec3 reflect_dir = ray.direction - (norm * (2 * glm::dot(ray.direction, norm)));
        Ray reflect_ray(hit.coord, reflect_dir);

        Intersection recur_hit;
        recur_hit.intersect(reflect_ray, scene);
        Color recur_color = findColor(recur_hit, reflect_ray, scene, depth + 1);

        color.R += hit.object->specular.x * recur_color.R;
        color.G += hit.object->specular.y * recur_color.G;
        color.B += hit.object->specular.z * recur_color.B;
    }
    return color;
}

Color helpFindColor(const Light &light, const Intersection &hit, const Ray &ray, const glm::vec3& attenuation)
{
    glm::vec3 dir = (light.type == Light::point) ? glm::normalize(light.coord - hit.coord) : glm::normalize(light.coord);

    glm::vec3 norm = glm::normalize(hit.object->interpolate(hit.coord));

	float nDotL = std::max(glm::dot(norm, dir), 0.0f);
   	Color diffuse(hit.object->diffuse);
    diffuse.R *= light.color.R * nDotL;
    diffuse.G *= light.color.G * nDotL;
    diffuse.B *= light.color.B * nDotL;
    
    glm::vec3 halfvec = glm::normalize(dir + glm::normalize(-ray.direction));

	float nDotH = std::max(glm::dot(norm, halfvec), 0.0f);
    
    Color specular(hit.object->specular);
	float exp = std::pow(nDotH, hit.object->shininess);
    specular.R *= light.color.R * exp;
    specular.G *= light.color.G * exp;
    specular.B *= light.color.B * exp;
	
	Color ret = diffuse;
    ret.R += specular.R;
    ret.G += specular.G;
    ret.B += specular.B;

	if (light.type == Light::point) {
        float consta = glm::length(light.coord - hit.coord);
        float tmp = 1.0f / (attenuation.z * consta * consta + attenuation.y * consta + attenuation.x);

        ret.R *= tmp;
        ret.G *= tmp;
        ret.B *= tmp;
	}

	return ret;
}