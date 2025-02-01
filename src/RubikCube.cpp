#include "RubikCube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <global.h>

RubikCube::RubikCube():lockers{false, false, false, false, false, false}{Create_all_cubes();}

RubikCube::~RubikCube(){
    for (cube *cube : cubes){
        delete cube;
    }
}

void RubikCube::Create_all_cubes(){

    cubesPosition.clear();
    int cubesize = 3;
    cubesPosition.resize(cubesize, std::vector<std::vector<cube *>>(cubesize, std::vector<cube *>(cubesize)));
    cubes.clear();
    int index = 0;
    const int offset = 1;
    //3x3x3 cube
    for (int x = -1; x <= 1; ++x){
        for (int y = -1; y <= 1; ++y){
            for (int z = -1; z <= 1; ++z){
                glm::vec3 position(x, y, z);
                cube *box = new cube(position, index);
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
                box->setModelMatrix(modelMatrix);
                cubes.push_back(box);
                cubesPosition[x + offset][y + offset][z + offset] = box;
                if (index == 13){//center cube
                    this->centerCube = box;
                }
                index++;
            }
        }
    }
}

std::vector<cube *> RubikCube::getcubes(){
    return this->cubes;
}
glm::mat4 calculate_mvp(glm::mat4 a, glm::mat4 b, glm::mat4 c){
    return a * b * c;
}
void RubikCube::update(){

    for (cube *cube : cubes){
        glm::mat4 model = cube->getRotationMatrix() * cube->getModelMatrix();
        glm::mat4 mvp =calculate_mvp(proj, view, model);

        test->Bind();
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        test->SetPickMode(0);
        test->SetUniform4f("u_Color", color);
        test->SetUniformMat4f("u_MVP", mvp);

        va->Bind();
        ib->Bind();
        GLCall(glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr));

        test->Unbind();
    }
        glfwSwapBuffers(window);
    if (pickmode){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //assigning a unique color to each cube
        //this color will be used to identify the cube when clicked
        //this color is the index of the cube
        for (cube *cube : cubes){
            glm::mat4 model = cube->getRotationMatrix() * cube->getModelMatrix();
            glm::mat4 mvp =calculate_mvp(proj, view, model);
            test->Bind();
            glm::vec3 uniqueColor = glm::vec3(cube->index, cube->index, cube->index);
            glm::vec4 pickingColor = glm::vec4(uniqueColor / 255.0f, 1.0f);
            test->SetPickMode(1);
            test->SetUniform4f("u_Color", pickingColor);
            test->SetUniformMat4f("u_MVP", mvp);
            va->Bind();
            ib->Bind();
            GLCall(glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr));
            test->Unbind();
        }
        
        glFlush();
        glFinish();
    }

}

void RubikCube::render_animation(){
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    update();
    glfwSwapBuffers(window);
}

void RubikCube::render(GLFWwindow *window){
    if (!animated){
        GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f)); // Set background to white
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        update();
    }
}

void RubikCube::rotate_Left_Wall(){
    animated = 1;
    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++){
        for (int i = 0; i < cubes.size(); ++i){
            cube *cube = cubes[i];
            if (cube->getPosition().x == centerPosition.x + 1){
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * -rotation_direction), glm::vec3(1.0f, 0.0f, 0.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }

    // Locking condition check
    if (RotationAngle == std::abs(45))
    {
        this->lockers[0] = !this->lockers[0];
    }
    animated = 0;
}

void RubikCube::rotate_Right_Wall(){
    animated = 1;
    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++){
        for (int i = 0; i < cubes.size(); ++i){
            cube *cube = cubes[i];
            if (cube->getPosition().x == centerPosition.x - 1){
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * rotation_direction), glm::vec3(1.0f, 0.0f, 0.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }

    if (RotationAngle == std::abs(45)){
        this->lockers[1] = !this->lockers[1];
    }
    animated = 0;
}

void RubikCube::rotate_Front_Wall(){
    animated = 1;
    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++)
    {
        for (int i = 0; i < cubes.size(); ++i)
        {
            cube *cube = cubes[i];

            if (cube->getPosition().z == centerPosition.z + 1)
            {

                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * -rotation_direction), glm::vec3(0.0f, 0.0f, 1.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }
    if (RotationAngle == std::abs(45))
    {
        this->lockers[2] = !this->lockers[2];
    }

    animated = 0;
}

void RubikCube::rotate_Back_Wall()
{
    animated = 1;

    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++)
    {
        for (int i = 0; i < cubes.size(); ++i)
        {
            cube *cube = cubes[i];

            if (cube->getPosition().z == centerPosition.z - 1)
            {

                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * rotation_direction), glm::vec3(0.0f, 0.0f, 1.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }
    if (RotationAngle == std::abs(45))
    {
        this->lockers[3] = !this->lockers[3];
    }
    animated = 0;
}

void RubikCube::rotate_Down_Wall()
{
    animated = 1;
    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++)
    {
        for (int i = 0; i < cubes.size(); ++i)
        {
            cube *cube = cubes[i];

            if (cube->getPosition().y == centerPosition.y + 1)
            {

                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * -rotation_direction), glm::vec3(0.0f, 1.0f, 0.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }
    if (RotationAngle == std::abs(45))
    {
        this->lockers[4] = !this->lockers[4];
    }
    animated = 0;
}

void RubikCube::rotate_Up_Wall()
{
    animated = 1;
    glm::vec3 centerPosition = this->centerCube->getPosition();
    for (int i = 1; i <= RotationAngle; i++)
    {
        for (int i = 0; i < cubes.size(); ++i)
        {
            cube *cube = cubes[i];

            if (cube->getPosition().y == centerPosition.y - 1)
            {

                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians((float)1 * rotation_direction), glm::vec3(0.0f, 1.0f, 0.0f));
                cube->setModelMatrix(rotationMatrix * cube->getModelMatrix());
            }
        }
        render_animation();
    }

    if (RotationAngle == std::abs(45))
    {
        this->lockers[5] = !this->lockers[5];
    }
    animated = 0;
}

bool RubikCube::IsRotationAllowedForWall( std::vector<int>& wallLockers) {
    return !(lockers[wallLockers[0]] || lockers[wallLockers[1]] || lockers[wallLockers[2]] || lockers[wallLockers[3]]);
}

bool RubikCube::Can_Rotate_Right_Wall() {
    std::vector<int> rightWallLockers = {2, 3, 4, 5};
    return IsRotationAllowedForWall(rightWallLockers);
}

bool RubikCube::Can_Rotate_Left_Wall() {
    std::vector<int> leftWallLockers = {2, 3, 4, 5};
    return IsRotationAllowedForWall(leftWallLockers);
}

bool RubikCube::Can_Rotate_Front_Wall() {
    std::vector<int> frontWallLockers = {0, 1, 4, 5};
    return IsRotationAllowedForWall(frontWallLockers);
}

bool RubikCube::Can_Rotate_Back_Wall() {
    std::vector<int> backWallLockers = {0, 1, 4, 5};
    return IsRotationAllowedForWall(backWallLockers);
}

bool RubikCube::Can_Rotate_Down_Wall() {
    std::vector<int> downWallLockers = {0, 1, 2, 3};
    return IsRotationAllowedForWall(downWallLockers);
}

bool RubikCube::Can_Rotate_Up_Wall() {
    std::vector<int> upWallLockers = {0, 1, 2, 3};
    return IsRotationAllowedForWall(upWallLockers);
}

glm::vec3 RubikCube::getPosition(){
    return this->cubesPosition[1][1][1]->position;
}

