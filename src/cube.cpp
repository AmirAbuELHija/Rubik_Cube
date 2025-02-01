#include "cube.h"

// Constructor
cube::cube(const glm::vec3 &pos, int index): index(index), position(pos), RotationMatrix(glm::mat4(1.0f)) {}

cube::cube(){}

glm::vec3 cube::getPosition() const{
    return position;
}

glm::mat4 cube::getModelMatrix() const{
    return modelMatrix;
}

void cube::setPosition(const glm::vec3 &pos){
    position = pos;
}

void cube::setModelMatrix(const glm::mat4 &matrix){

    this->modelMatrix = matrix;
    this->position = glm::vec3(round(matrix[3][0]), round(matrix[3][1]), round(matrix[3][2]));
}
glm::mat4 cube::getRotationMatrix() const{
    return RotationMatrix;
}

void cube ::setRotationMatrix(const glm::mat4 &matrix){
    this->RotationMatrix = matrix;
}
