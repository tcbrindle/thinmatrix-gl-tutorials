#ifndef JAC_MATHS_HPP
#define JAC_MATHS_HPP

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace jac {

glm::mat4 create_transformation_matrix(const glm::vec3& translation,
                                       float rx, float ry, float rz,
                                       float scale);


}

#endif