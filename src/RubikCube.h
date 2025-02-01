#ifndef RUBIKCUBE_H
#define RUBIKCUBE_H

#include <vector>
#include "cube.h"
#include <glm/glm.hpp>
#include <Shader.h>
#include <VertexArray.h>
#include <IndexBuffer.h>
#include <Camera.h>



class RubikCube {

public:
    int  rotation_direction = 1  ;    // global varibale for rotation direction
    int RotationAngle = 90 ;    // global variable for rotation 
    float sensitivity =0.1;   //  for arrow pressing
    int pickmode=0;
    cube* pickeCube;
    Shader *test;
    VertexArray *va;
    VertexBuffer *vb;
    IndexBuffer *ib;
    GLFWwindow *window;
    int animated=0;
    RubikCube() ;  // Constructor 
    ~RubikCube();  // Destructor

    void Create_all_cubes();  // Generate all the small cubes
    void render(GLFWwindow* window);
    void render_animation();
    void update();  // Update the Rubik's Cube
    bool lockers[6]; 
    std::vector<std::vector<std::vector<cube*>>> cubesPosition ;  
    // Rotaions  
    void rotate_Left_Wall() ; 
    void rotate_Right_Wall() ;
    void rotate_Front_Wall() ;
    void rotate_Back_Wall() ;
    void rotate_Down_Wall() ;
    void rotate_Up_Wall() ; 
    
    // check if can rotate
    bool IsRotationAllowedForWall(std::vector<int>& wallLockers);
    bool Can_Rotate_Right_Wall();
    bool Can_Rotate_Left_Wall();
    bool Can_Rotate_Front_Wall();
    bool Can_Rotate_Back_Wall();
    bool Can_Rotate_Down_Wall();
    bool Can_Rotate_Up_Wall();
    glm :: vec3 getPosition () ;  
    std::vector<cube*> getcubes() ; 
    std::vector<cube*> cubes;  // Store all the small cubes in the Rubik's Cube
    cube* centerCube;





};

#endif // RUBIKSCUBE_H
