#include "geometry.h"

const float epsilon = 1e-4;

// helpers to solve quadratic equations
float computeDiscr(const Ray &ray, const glm::vec3 &center, const float &radius);
std::pair<float, float> solveRoots(const Ray &ray, const glm::vec3 &center, float discr);

// the following empty definitions are for compilation
bool Object::hit(const Ray &ray, float &t_val) { return false; }
glm::vec3 Object::interpolate(const glm::vec3& P) { return glm::vec3(); }

// return true if sphere was hit, false otherwise
// record hit parameter t_val (for equation ray = origin + direction * t_val)
bool Sphere::hit(const Ray &ray, float &t_val)
{
    float discr = computeDiscr(ray, center, radius);
    if (discr < 0.0f) // no intersection
        return false;

    // solve for roots
    std::pair<float, float> roots;
    roots = solveRoots(ray, center, discr);

    if (roots.first >= 0 && roots.second >= 0)       // if there're 2 intersections
        t_val = std::min(roots.first, roots.second); // or a tangent
    else
        t_val = std::max(roots.first, roots.second); // 1 root is negative

    return true;
}

//return interpolated normal
glm::vec3 Sphere::interpolate(const glm::vec3& P) {
    glm::vec4 P4 = glm::vec4(P, 1.0f);
    P4 = inv_trans * P4;
    // P4 = glm::transpose(inv_trans) * P4;
    glm::vec3 ret = glm::vec3(P4) / P4.w;

    P4 = glm::vec4(ret - center, 0.0f);
    P4 = glm::transpose(inv_trans) * P4;
    // P4 = inv_trans * P4;

    ret = glm::vec3(P4);
    return glm::normalize(ret);

    // glm::vec4 norm(P - center, 0.0f);
    // norm = glm::inverse(glm::transpose(trans)) * norm;
    // return glm::normalize(glm::vec3(norm));
}


// return true if triangle was hit, false otherwise
// record hit parameter t_val (for equation ray = origin + direction * t_val)
bool Triangle::hit(const Ray &ray, float &t_val)
{
    glm::vec3 A, B, C; // vertices of the triangle
    A = vertices[0];
    B = vertices[1];
    C = vertices[2];

    glm::vec3 norm = glm::cross(B - A, C - A);
    float dir_dot_norm = glm::dot(ray.direction, norm);

    if (glm::abs(dir_dot_norm) < epsilon) // close to zero, no intersection
        return false;

    float tmp_t_val;
    tmp_t_val = glm::dot(A, norm) - glm::dot(ray.origin, norm);
    tmp_t_val /= glm::dot(ray.direction, norm);

    glm::vec3 P = ray.origin + ray.direction * tmp_t_val;

    glm::vec3 area(0.0f, 0.0f, 0.0f);
    float area_val = 0.0f;

    glm::vec3 tmp_cross = glm::cross(P - A, P - B);
    area_val += glm::length(tmp_cross);
    area += tmp_cross;

    tmp_cross = glm::cross(P - B, P - C);
    area_val += glm::length(tmp_cross);
    area += tmp_cross;

    tmp_cross = glm::cross(P - C, P - A);
    area_val += glm::length(tmp_cross);
    area += tmp_cross;

    glm::vec3 cross1 = glm::cross(C - P, A - P);
    glm::vec3 cross2 = glm::cross(A - P, B - P);

    float norm_dot_norm = glm::dot(norm, norm);

    float alpha, beta, gamma;

    beta = glm::dot(cross1, norm) / norm_dot_norm;
    gamma = glm::dot(cross2, norm) / norm_dot_norm;
    alpha = 1.0 - beta - gamma;

    float one_eps = 1.0f + epsilon;
    if (beta > -epsilon && beta < one_eps &&
        gamma > -epsilon && gamma < one_eps &&
        alpha > -epsilon && alpha < one_eps) 
    {
        t_val = tmp_t_val;
        return true;
    } else
        return false;
}

glm::vec3 Triangle::interpolate(const glm::vec3& P) {
    glm::vec3 A = vertices[0];
    glm::vec3 B = vertices[1];
    glm::vec3 C = vertices[2];  

    glm::vec4 P_trans4 = inv_trans * glm::vec4(P, 1.0f);
    glm::vec3 P_trans = glm::vec3(P_trans4) / P_trans4.w;
    // glm::vec3 P_trans = P;

    glm::vec3 norm = glm::cross(B-A, C-A);
    glm::vec3 cross1 = glm::cross(C-P_trans, A-P_trans);
    glm::vec3 cross2 = glm::cross(A-P_trans, B-P_trans);
    
    float norm_dot_norm = glm::dot(norm, norm);
    float beta = glm::dot(norm, cross1) / norm_dot_norm;
    float gamma = glm::dot(norm, cross2) / norm_dot_norm;
    float alpha = 1.0f - beta - gamma;

    glm::vec3 tmp = (norm * alpha) + (norm * beta) + (norm * gamma);

    return glm::normalize(glm::vec3((glm::transpose(inv_trans) * glm::vec4(tmp, 0.0f))));
    // return glm::normalize(glm::vec3(inv_trans * glm::vec4(tmp,0.0f)));

}

float computeDiscr(const Ray &ray, const glm::vec3 &center, const float &radius)
{
    float discr;
    discr = 2 * glm::dot(ray.direction, (ray.origin - center));
    discr = discr * discr;

    float dir_dot = glm::dot(ray.direction, ray.direction);
    glm::vec3 orig_cen = ray.origin - center;
    float orig_cen_dot = glm::dot(orig_cen, orig_cen);
    orig_cen_dot -= radius * radius;

    discr -= 4 * dir_dot * orig_cen_dot;

    return discr;
}

std::pair<float, float> solveRoots(const Ray &ray, const glm::vec3 &center, float discr)
{
    float root1, root2;

    discr = glm::sqrt(discr);

    root1 = -2 * glm::dot(ray.direction, ray.origin - center);
    root2 = root1;

    root1 += discr;
    root2 -= discr;

    float dir_dot = glm::dot(ray.direction, ray.direction);

    root1 /= 2.0f * dir_dot;
    root2 /= 2.0f * dir_dot;

    return std::pair(root1, root2);
}