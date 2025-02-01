#ifndef cube_H
#define cube_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

class cube {
    
    public:
    glm::vec3 position; //position in 3D space
    glm::mat4 modelMatrix; //matrix for transformations
    glm::mat4 RotationMatrix;
    int index ; 

    // Constructor
    cube(const glm::vec3& pos , int index);
    cube() ; 

    // Getters and setters
    glm::vec3 getPosition() const;
    glm::mat4 getModelMatrix() const;   // Getter for the model matrix
    glm::mat4 getRotationMatrix() const; 
    void setPosition(const glm::vec3& pos);
    void setModelMatrix(const glm::mat4& matrix); // Setter for the model matrix
    void setRotationMatrix ( const glm::mat4 &matrix);


};
#endif // cube_H

