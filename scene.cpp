#include "scene.h"
#include "transform.h"

typedef unsigned int uint;

Camera::Camera(glm::vec3 lookfrom, glm::vec3 lookat, glm::vec3 up_dir, float fov_y)
	: eye(lookfrom), center(lookat), up(up_dir), fovy(fov_y) {}

Camera::Camera() : eye(0, 0, 1), center(0, 0, 0), up(0, 1, 0) {}

// This function is for stacking the transformations
void Scene::rightmultiply(const glm::mat4 &M, stack<glm::mat4> &transfstack)
{
	glm::mat4 &T = transfstack.top();
	T = T * M;
}

// Function to read the input data values
bool Scene::readvals(stringstream &s, const int numvals, float *values)
{
	for (int i = 0; i < numvals; i++)
	{
		s >> values[i];
		if (s.fail())
		{
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}

void Scene::readfile(const char *filename)
{

	string str, cmd;
	ifstream in;
	in.open(filename);
	if (in.is_open())
	{

		// matrix stack to store transforms.
		stack<glm::mat4> transfstack;
		transfstack.push(glm::mat4(1.0)); // identity

		getline(in, str);
		while (in)
		{
			if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#'))
			{
				// Ruled out comment and blank lines

				stringstream s(str);
				s >> cmd;
				int i;
				float values[10]; // Position and color for light, colors for others
				// Up to 10 params for cameras.
				bool validinput; // Validity of input
				//------------------------------------------------------------------------
				//
				// ----------------THE FOLLOWING PARSES GENERAL COMMANDS-----------------
				if (cmd == "size")
				{
					validinput = readvals(s, 2, values);
					if (validinput)
					{
						width = (int)values[0];
						height = (int)values[1];
					}
				}
				else if (cmd == "maxdepth")
				{
					validinput = readvals(s, 1, values);
					if (validinput)
					{
						depth = values[0];
					}
				}
				else if (cmd == "output")
				{
					string file;
					s >> file;
					if (!s.fail())
					{
						outfilename = file;
					}
					else
					{
						cout << "Failed reading value, will skip\n";
					}
				}
				//---------------------------------------------------------------------
				//
				//--------------THE FOLLOWING PARSES CAMERA----------------------------
				//
				else if (cmd == "camera")
				{
					validinput = readvals(s, 10, values); // 10 values eye cen up fov
					if (validinput)
					{
						for (int i = 0; i < 3; i++)
						{
							cam.eye[i] = values[i];
							cam.center[i] = values[i + 3];
							cam.up[i] = values[i + 6];
						}
						cam.fovy = values[9];
					}
				}
				//----------------------------------------------------------------------

				// --------THE FOLLOWING PARSES GEOMETRY----------
				else if (cmd == "sphere")
				{
					validinput = readvals(s, 4, values);
					if (validinput)
					{
						Sphere* sphere = new Sphere();
						sphere->type = Object::sphere;

						// save center coords and radius
						for (int i = 0; i < 3; i++)
						{
							sphere->center[i] = values[i];
						}
						sphere->radius = values[3];

						// save material properties
						sphere->diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
						sphere->specular = glm::vec3(specular[0], specular[1], specular[2]);
						sphere->emission = glm::vec3(emission[0], emission[1], emission[2]);
						sphere->ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
						sphere->shininess = shininess;

						// save transormation matrix
						sphere->trans = transfstack.top();
						sphere->inv_trans = glm::inverse(sphere->trans);

						objects.push_back(sphere);
					}
				}
				else if (cmd == "maxverts")
				{
					validinput = readvals(s, 1, values);
					if (validinput)
					{
						maxverts = values[0];
					}
				}
				else if (cmd == "maxvertsnorms")
				{
					validinput = readvals(s, 1, values);
					if (validinput)
					{
						maxvertnorms = values[0];
					}
				}
				else if (cmd == "vertex")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						glm::vec3 vert;
						for (int i = 0; i < 3; i++)
						{
							vert[i] = values[i];
						}
						vertices.push_back(vert);
					}
				}
				else if (cmd == "vertexnormal")
				{
					validinput = readvals(s, 6, values);
					if (validinput)
					{
						glm::vec3 vert;
						glm::vec3 norm;
						for (int i = 0; i < 3; i++)
						{
							vert[i] = values[i];
						}
						for (int i = 0; i < 3; i++)
						{
							norm[i] = values[i + 3];
						}

						vertnorms.push_back(vert);
						norms.push_back(norm);
					}
				}
				else if (cmd == "tri")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						Triangle* triangle = new Triangle();
						triangle->type = Object::triangle;
						triangle->areNorms = false;
						// save its coords
						for (int i = 0; i < 3; i++)
							triangle->vertices[i] = vertices[values[i]];

						// save material properties
						triangle->diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
						triangle->specular = glm::vec3(specular[0], specular[1], specular[2]);
						triangle->emission = glm::vec3(emission[0], emission[1], emission[2]);
						triangle->ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
						triangle->shininess = shininess;

						// save transormation matrix
						triangle->trans = transfstack.top();
						triangle->inv_trans = glm::inverse(transfstack.top());

						objects.push_back(triangle);
					}
				}
				else if (cmd == "trinormal")
				{
					validinput = readvals(s, 6, values);
					if (validinput)
					{
						Triangle* triangle = new Triangle();
						triangle->type = Object::triangle;
						triangle->areNorms = true;

						// save its coords
						for (int i = 0; i < 3; i++)
							triangle->vertices[i] = vertnorms[values[i]];
						for (int i = 0; i < 3; i++)
							triangle->vertexnorms[i] = norms[i + 3];

						// save material properties
						triangle->diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
						triangle->specular = glm::vec3(specular[0], specular[1], specular[2]);
						triangle->emission = glm::vec3(emission[0], emission[1], emission[2]);
						triangle->ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
						triangle->shininess = shininess;

						// save transormation matrix
						triangle->trans = transfstack.top();
						triangle->inv_trans = glm::inverse(triangle->trans);

						objects.push_back(triangle);
					}
				}
				//----------------------------------------------------------
				//
				//--------THE FOLLOWING PARSES TRANSORMATIONS---------------

				else if (cmd == "translate")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						rightmultiply(Transform::translate(values[0], values[1], values[2]), transfstack);
					}
				}
				else if (cmd == "scale")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						rightmultiply(Transform::scale(values[0], values[1], values[2]), transfstack);
					}
				}
				else if (cmd == "rotate")
				{
					validinput = readvals(s, 4, values);
					if (validinput)
					{
						glm::vec3 axis = glm::vec3(values[0], values[1], values[2]);
						glm::mat3 rot = Transform::rotate(values[3], axis);
						glm::mat4 rot4 = glm::mat4(rot);
						rightmultiply(rot4, transfstack);
					}
				}
				else if (cmd == "pushTransform")
				{
					transfstack.push(transfstack.top());
				}
				else if (cmd == "popTransform")
				{
					if (transfstack.size() <= 1)
					{
						cerr << "Stack has no elements.  Cannot Pop\n";
					}
					else
					{
						transfstack.pop();
					}
				}

				//-----------------------------------------------------------
				//
				//--------THE FOLLOWING PARSES LIGHTS------------------------
				else if (cmd == "directional")
				{
					validinput = readvals(s, 6, values); // directional (x,y,z,r,g,b)
					if (validinput)
					{
						Light light;
						for (int i = 0; i < 3; i++)
							light.coord[i] = values[i];
						light.type = Light::directional; // directional light
						light.color.R = values[3];
						light.color.G = values[4];
						light.color.B = values[5];

						lights.push_back(light);
					}
				}
				else if (cmd == "point")
				{
					validinput = readvals(s, 6, values); // point (x,y,z,r,g,b)
					if (validinput)
					{
						Light light;
						for (int i = 0; i < 3; i++)
							light.coord[i] = values[i];
						light.type = Light::point;
						light.color.R = values[3];
						light.color.G = values[4];
						light.color.B = values[5];

						lights.push_back(light);
					}
				}
				else if (cmd == "attenuation")
				{
					validinput = readvals(s, 3, values); // attenuation (const, lin, quadr)
					if (validinput)
					{
						for (i = 0; i < 3; i++)
						{
							attenuation[i] = values[i];
						}
					}
				}
				//------------------------------------------------------------
				//
				//-------THE FOLLOWING PARSES MATERIAL PROPERTIES-------------
				else if (cmd == "ambient")
				{
					validinput = readvals(s, 3, values); // ambient (r,g,b)
					if (validinput)
					{
						for (i = 0; i < 3; i++)
						{
							ambient[i] = values[i];
						}
					}
				}
				else if (cmd == "diffuse")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						for (i = 0; i < 3; i++)
						{
							diffuse[i] = values[i];
						}
					}
				}
				else if (cmd == "specular")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						for (i = 0; i < 3; i++)
						{
							specular[i] = values[i];
						}
					}
				}
				else if (cmd == "emission")
				{
					validinput = readvals(s, 3, values);
					if (validinput)
					{
						for (i = 0; i < 3; i++)
						{
							emission[i] = values[i];
						}
					}
				}
				else if (cmd == "shininess")
				{
					validinput = readvals(s, 1, values);
					if (validinput)
					{
						shininess = values[0];
					}
				}
				//-----------------------------------------------------------------
				//
				//
				else
				{
					cerr << "Unknown Command: " << cmd << " Skipping \n";
				}
			}
			getline(in, str);
		}
	}
	else
	{
		cerr << "Unable to Open Input Data File " << filename << "\n";
		throw 2;
	}
}
