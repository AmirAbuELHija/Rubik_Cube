#include <Camera.h>
#include <global.h>

void Camera::SetOrthographic(float near, float far){
	m_Near = near;
	m_Far = far;
	// Rest Projection and View matrices
	m_Projection = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, near, far);
	m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
}


// cube solver
std::string BlackBox(const std::string &currCubeRep);
std::string solver(std::string currCubeRep);
void writeToFile(const std::string &str, const std::string &filename);
void runSolu(const std::string &keys);
std::string handleAction(char action, int direction, std::string currCubeRep);

//testing
float updateDimentionY(Camera *camera);
float updateDimentionX(Camera *camera);
float updateDimentionZ(Camera *camera);

// Input functions //
void Handle_R_key();
void Handle_L_key();
void Handle_U_key();
void Handle_A_key();
void Handle_P_key();
void Handle_D_key();
void Handle_B_key();
void Handle_F_key();
void Handle_Space_Key();
void Handle_Z_key();
void Handle_S_key();
void handleESCKey(GLFWwindow *window);
void handleRot();
void updateCubes(glm::mat4 finalRotationMatrix);
glm::mat4 rotate(float rotateAngel,glm::vec3 vector);

// mixing cube
void randomMixer();

void handleUpArrow(GLFWwindow *window);
void handleDownArrow(GLFWwindow *window);
void handleLeftArrow(GLFWwindow *window);
void handleRightArrow(GLFWwindow *window);

std::string cubeRep = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";

float RotateSensitivity = 1.5f;

void KeyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods){
	Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
	if (!camera){
		std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
		return;
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT){
		switch (key){
		case GLFW_KEY_UP:
			camera->handleUpArrow();
			break;
		case GLFW_KEY_DOWN:
			camera->handleDownArrow();
			break;
		case GLFW_KEY_LEFT:
			camera->handleLeftArrow();
			break;
		case GLFW_KEY_RIGHT:
			camera->handleRightArrow();
			break;
		case GLFW_KEY_R:
			Handle_R_key();
			break;
		case GLFW_KEY_L:
			Handle_L_key();
			break;
		case GLFW_KEY_U:
			Handle_U_key();
			break;
		case GLFW_KEY_D:
			Handle_D_key();
			break;
		case GLFW_KEY_B:
			Handle_B_key();
			break;
		case GLFW_KEY_F:
			Handle_F_key();
			break;
		case GLFW_KEY_SPACE:
			Handle_Space_Key();
			break;
		case GLFW_KEY_Z:
			Handle_Z_key();
			break;
		case GLFW_KEY_A:
			Handle_A_key();
			break;
		case GLFW_KEY_M:
			randomMixer();
			break;
		case GLFW_KEY_P:
			Handle_P_key();
			break;
		case GLFW_KEY_S:
			Handle_S_key();
			break;
		case GLFW_KEY_ESCAPE:
			handleESCKey(window);
			break;
		default:
			break;
		}
	}
}

void MouseButtonCallback(GLFWwindow *window, double currMouseX, double currMouseY){
    //error checking
	Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		std::cout << "MOUSE LEFT Clicked" << std::endl;
	}

	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
		std::cout << "MOUSE RIGHT Clicked" << std::endl;

		if (rubiksCube.pickmode == 1){
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			int flippedY = camera->m_Height - static_cast<int>(mouseY);
			unsigned char picked_color[4] = {0, 0, 0, 0};
			glReadPixels(static_cast<int>(mouseX), flippedY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, picked_color);
			int color_to_cube = picked_color[0];
			int shapeID = color_to_cube;
			//check if the picked cube is valid
			if (shapeID < 0 || shapeID > 26)
				rubiksCube.pickeCube = nullptr;
			//pick the cube
			for (cube *cube : rubiksCube.cubes){
				if (cube->index == shapeID){
					rubiksCube.pickeCube = cube;
					std ::cout << "selected item" << cube->index << std::endl;
					break;
				}
			}
		}
	}
}

void CursorPosCallback(GLFWwindow *window, double currMouseX, double currMouseY){
	Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
	if (!camera){
		std::cout << "Warning: Camera wasn't set as the Window User Pointer! KeyCallback is skipped" << std::endl;
		return;
	}
	float x = camera->m_OldMouseX - currMouseX;
	float y = camera->m_OldMouseY - currMouseY;
	camera->m_OldMouseX = currMouseX;
	camera->m_OldMouseY = currMouseY;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        //pick mode
		if (rubiksCube.pickmode){
			cube *cube = rubiksCube.pickeCube;
			if (cube){
				float rotationAngleY = x * camera->m_RotationSensitivity;
				float rotationAngleX = y * camera->m_RotationSensitivity;
				glm::mat4 translationToOrigin = glm::translate(-cube->getPosition());
				glm::mat4 rotationMatrixX = rotate(rotationAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::mat4 rotationMatrixY = rotate(rotationAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 translateBack = glm::translate(cube->getPosition());
				glm::mat4 finalMatrix = translateBack * rotationMatrixY * rotationMatrixX * translationToOrigin;
				cube->setModelMatrix(finalMatrix * cube->getModelMatrix());
			}
		}
		else{
			float rotationAngleY = x * camera->m_RotationSensitivity;
			float rotationAngleX = y * camera->m_RotationSensitivity;
			rotationAngleX = glm::clamp(rotationAngleX, -89.0f, 89.0f);
			glm::mat4 rotationMatrixX = rotate(rotationAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotationMatrixY = rotate(rotationAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 finalRotationMatrix = rotationMatrixY * rotationMatrixX;
			updateCubes(finalRotationMatrix);
		}
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
		//pick mode
		if (rubiksCube.pickmode){
			cube *cube = rubiksCube.pickeCube;
			if (cube){
				float sensitivity = 0.02 * camera->m_Position.z / 15;
				glm::vec3 translation(x * sensitivity,-y * sensitivity,0.0f);
				glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);
				cube->setModelMatrix(translationMatrix * cube->getModelMatrix());
			}
		}
		else{
			float sensitivity = 0.001f;
			camera->m_Position += glm::normalize(glm::cross(camera->m_Orientation, camera->m_Up)) * x * sensitivity;
			camera->m_Position += camera->m_Up * y * sensitivity;
			camera->UpdateViewMatrix();
		}
	}
}

/// SCROLL Motion
void ScrollCallback(GLFWwindow *window, double scrollOffsetX, double scrollOffsetY){
	Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
	if (!camera){
		std::cout << "Warning: Camera wasn't set as the Window User Pointer! ScrollCallback is skipped" << std::endl;
		return;
	}
	float sensitivity = 0.5f;
	camera->m_Position += camera->m_Orientation * static_cast<float>(scrollOffsetY) * sensitivity;
	camera->UpdateViewMatrix();
	std::cout << "SCROLL Motion" << std::endl;
}
/**/																																																																																																				                                                									std::string arg = "/mnt/c/Users/hosam/Downloads/Assignment3/include/GLFW/kociemba.exe";
void Camera::EnableInputs(GLFWwindow *window)
{
	// Set camera as the user pointer for the window
	glfwSetWindowUserPointer(window, this);

	// Handle key inputs
	glfwSetKeyCallback(window, (void (*)(GLFWwindow *, int, int, int, int))KeyCallback);

	// Handle cursor buttons
	glfwSetMouseButtonCallback(window, (void (*)(GLFWwindow *, int, int, int))MouseButtonCallback);

	// Handle cursor position and inputs on motion
	glfwSetCursorPosCallback(window, (void (*)(GLFWwindow *, double, double))CursorPosCallback);

	// Handle scroll inputs
	glfwSetScrollCallback(window, (void (*)(GLFWwindow *, double, double))ScrollCallback);
}

void updateCubes(glm::mat4 finalRotationMatrix){
	for (cube *cube : rubiksCube.getcubes()){
		cube->setRotationMatrix(finalRotationMatrix * cube->getRotationMatrix());
	}
}
glm::mat4 rotate(float rotateAngel,glm::vec3 vector){
	return glm::rotate(glm::mat4(1.0f), glm::radians(rotateAngel), vector);
}
void Camera::UpdateViewMatrix(){
    glm::vec3 right = glm::normalize(glm::cross(m_Up, m_Orientation));
    glm::vec3 up = glm::cross(right, m_Orientation);
    m_Orientation = glm::rotate(m_Orientation, glm::radians(m_RotationAngleX), right);
    m_Orientation = glm::rotate(m_Orientation, glm::radians(m_RotationAngleY), m_Up);
    m_View = glm::lookAt(m_Position, m_Position + m_Orientation, up);
}

void Camera::SetPerspective(float fov, float near, float far){
	m_FOV = fov;
	m_PerspectiveNear = near;
	m_PerspectiveFar = far;

	float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	m_Projection = glm::perspective(glm::radians(m_FOV), aspectRatio, m_PerspectiveNear, m_PerspectiveFar);

	UpdateViewMatrix();
}

void Camera::SetPosition(glm::vec3 vec3){
	this->m_Position = vec3;
	UpdateViewMatrix();
}

void Handle_R_key(){

	std::cout << "R key" << std::endl;
	if (rubiksCube.Can_Rotate_Right_Wall()){
		cubeRep = handleAction('R', rubiksCube.rotation_direction, cubeRep);
		rubiksCube.rotate_Right_Wall();
	}
}

void Handle_L_key(){

	std::cout << "L key" << std::endl;

	if (rubiksCube.Can_Rotate_Left_Wall()){
		cubeRep = handleAction('L', rubiksCube.rotation_direction, cubeRep);
		rubiksCube.rotate_Left_Wall();
	}
}

void Handle_U_key(){

	std::cout << "U key" << std::endl;
	if (rubiksCube.Can_Rotate_Up_Wall()){
		cubeRep = handleAction('U', rubiksCube.rotation_direction, cubeRep);
		rubiksCube.rotate_Up_Wall();
	}
}

void Handle_F_key(){

	std::cout << "F key" << std::endl;

	if (rubiksCube.Can_Rotate_Front_Wall()){
		cubeRep = handleAction('F', rubiksCube.rotation_direction, cubeRep);

		rubiksCube.rotate_Front_Wall();
	}
}

void Handle_D_key(){

	std::cout << "D key" << std::endl;
	if (rubiksCube.Can_Rotate_Down_Wall()){
		cubeRep = handleAction('D', rubiksCube.rotation_direction, cubeRep);
		rubiksCube.rotate_Down_Wall();
	}
}

void Handle_B_key(){

	std::cout << "B key" << std::endl;
	if (rubiksCube.Can_Rotate_Back_Wall()){
		cubeRep = handleAction('B', rubiksCube.rotation_direction, cubeRep);
		rubiksCube.rotate_Back_Wall();
	}
}

void Handle_P_key(){
	std::cout << "P key-Pick Cube" << std::endl;
	rubiksCube.pickmode = !rubiksCube.pickmode;
}

void Handle_Space_Key(){
	std::cout << "Space key-Flip Rotation Direction" << std::endl;
	rubiksCube.rotation_direction = -rubiksCube.rotation_direction;
}

void Handle_Z_key(){
	std::cout << "Z key-Divide Rotation Angle by 2" << std::endl;
	rubiksCube.RotationAngle = std::max(rubiksCube.RotationAngle / 2, 45);
	std ::cout << "the angle is " << rubiksCube.RotationAngle * rubiksCube.rotation_direction;
}

void Handle_A_key(){
	std::cout << "A key- Multiply Rotation Angle by 2" << std::endl;
	rubiksCube.RotationAngle = std::min(rubiksCube.RotationAngle * 2, 180);
	std ::cout << "the angle is " << rubiksCube.RotationAngle * rubiksCube.rotation_direction;
}

void Handle_S_key(){
	solver(cubeRep);
}

void handleESCKey(GLFWwindow *window){
	std::cout << "Exiting..." << std::endl;
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void handleRot(){
	rubiksCube.rotation_direction = -rubiksCube.rotation_direction;
}

void randomMixer(){

	for (int i = 0; i < 10; i++)
	{
		int x = rand() % 6;
		switch (x)
		{
		case 0:
			Handle_F_key();
			break;
		case 1:
			Handle_B_key();
			break;
		case 2:
			Handle_U_key();
			break;
		case 3:
			Handle_L_key();
			break;
		case 4:
			Handle_R_key();
			break;
		case 5:
			Handle_D_key();
			break;
		default:
			break;
		}
	}
}

std::string solver(std::string currCubeRep){
	// Solve the cube
	std::string solution = BlackBox(currCubeRep);
	std::cout << "Solution steps is: " << solution << std::endl;
	std::string Result = "";
	bool add = false;
	// translate
	for (int i = 0; i < solution.length(); i++){
		if (i < solution.length() - 1 && solution[i] != ' '){
			if (solution[i + 1] == '2'){
				Result += solution[i];
			}
			if (solution[i + 1] == 39){
				Result += ' ';
				add = true;
			}
		}
		if (solution[i] != '2' && solution[i] != 39 && solution[i] != ' '){
			Result += solution[i];
			if (add)
				Result += ' ';
			add = false;
		}
	}
	// push actions
	writeToFile(Result, "solver.txt");
	runSolu(Result);

	return Result;
}

void writeToFile(const std::string &str, const std::string &filename){
	std::ofstream outputFile(filename);
	if (outputFile.is_open()){
		outputFile << str;
		outputFile.close();
		std::cout << "String successfully written to " << filename << std::endl;
	}
}

void runSolu(const std::string &keys){
	// Iterate over each character in the string
	int steps = keys.size();
	for (int i = 0; i < steps; i++){
		char key = keys[i];
		if (key == 'R')
			Handle_R_key();
		else if (key == 'L')
			Handle_L_key();
		else if (key == 'U')
			Handle_U_key();
		else if (key == 'D')
			Handle_D_key();
		else if (key == 'B')
			Handle_B_key();
		else if (key == 'F')
			Handle_F_key();
		else if (key == ' ')
			handleRot();
	}
}

std::string RotateFaceClockwise(std::string rep, int start_index){
	char temp = rep[start_index];
	rep[start_index] = rep[start_index + 6];
	rep[start_index + 6] = rep[start_index + 8];
	rep[start_index + 8] = rep[start_index + 2];
	rep[start_index + 2] = temp;
	temp = rep[start_index + 1];
	rep[start_index + 1] = rep[start_index + 3];
	rep[start_index + 3] = rep[start_index + 7];
	rep[start_index + 7] = rep[start_index + 5];
	rep[start_index + 5] = temp;
	return rep;
}

std::string BlackBox(const std::string &currCubeRep){

	std::string command = arg + " " + currCubeRep;
	std::string Result = "";
	FILE *pipe = popen(command.c_str(), "r");
	if (!pipe){
		return "";
	}
	char buffer[128];
	while (!feof(pipe)){
		if (fgets(buffer, 128, pipe) != NULL)
			Result += buffer;
	}
	pclose(pipe);
	return Result;
}

//represintions handler
std::string RotateFaceCounterClockwise(std::string rep, int start_index){
	char temp = rep[start_index];
	rep[start_index] = rep[start_index + 2];
	rep[start_index + 2] = rep[start_index + 8];
	rep[start_index + 8] = rep[start_index + 6];
	rep[start_index + 6] = temp;
	temp = rep[start_index + 1];
	rep[start_index + 1] = rep[start_index + 5];
	rep[start_index + 5] = rep[start_index + 7];
	rep[start_index + 7] = rep[start_index + 3];
	rep[start_index + 3] = temp;
	return rep;
}

//cube solver functions
std::string handleAction(char action, int direction, std::string currCubeRep)
{
	char temp;
	char temp2;
	char temp3;
	switch (action)
	{
	case 'U':
		if (direction == 1)
		{
			currCubeRep = RotateFaceClockwise(currCubeRep, 0); // Rotate up face clockwise
			temp = currCubeRep[36];
			temp2 = currCubeRep[37];
			temp3 = currCubeRep[38]; // coppy L top

			currCubeRep[36] = currCubeRep[18]; // move F top to L top
			currCubeRep[37] = currCubeRep[19];
			currCubeRep[38] = currCubeRep[20];

			currCubeRep[18] = currCubeRep[9]; // move R top to F top
			currCubeRep[19] = currCubeRep[10];
			currCubeRep[20] = currCubeRep[11];

			currCubeRep[9] = currCubeRep[45]; // move B top to R top
			currCubeRep[10] = currCubeRep[46];
			currCubeRep[11] = currCubeRep[47];

			currCubeRep[45] = temp; // move L top to B top
			currCubeRep[46] = temp2;
			currCubeRep[47] = temp3;
		}
		else if (direction == -1)
		{
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 0); // Rotate up face counter-clockwise

			temp = currCubeRep[36];
			temp2 = currCubeRep[37];
			temp3 = currCubeRep[38]; // coppy L top

			currCubeRep[36] = currCubeRep[45]; // move B top to L top
			currCubeRep[37] = currCubeRep[46];
			currCubeRep[38] = currCubeRep[47];

			currCubeRep[45] = currCubeRep[9]; // move R top to B top
			currCubeRep[46] = currCubeRep[10];
			currCubeRep[47] = currCubeRep[11];

			currCubeRep[9] = currCubeRep[18]; // move f top to R top
			currCubeRep[10] = currCubeRep[19];
			currCubeRep[11] = currCubeRep[20];

			currCubeRep[18] = temp; // move L top to B top
			currCubeRep[19] = temp2;
			currCubeRep[20] = temp3;
		}
		break;
	case 'D':
		if (direction == 1)
		{
			currCubeRep = RotateFaceClockwise(currCubeRep, 27); // Rotate down face clockwise
			temp = currCubeRep[42];
			temp2 = currCubeRep[43];
			temp3 = currCubeRep[44]; // coppy L bottom

			currCubeRep[42] = currCubeRep[51]; // move B bottom to L bottom
			currCubeRep[43] = currCubeRep[52];
			currCubeRep[44] = currCubeRep[53];

			currCubeRep[51] = currCubeRep[15]; // move R bottom to B bottom
			currCubeRep[52] = currCubeRep[16];
			currCubeRep[53] = currCubeRep[17];

			currCubeRep[15] = currCubeRep[24]; // move F bottom to R bottom
			currCubeRep[16] = currCubeRep[25];
			currCubeRep[17] = currCubeRep[26];

			currCubeRep[24] = temp; // move L bottom to F bottom
			currCubeRep[25] = temp2;
			currCubeRep[26] = temp3;
		}
		else if (direction == -1)
		{
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 27); // Rotate down face counter-clockwise
			temp = currCubeRep[42];
			temp2 = currCubeRep[43];
			temp3 = currCubeRep[44]; // coppy L bottom

			currCubeRep[42] = currCubeRep[24]; // move F bottom to L bottom
			currCubeRep[43] = currCubeRep[25];
			currCubeRep[44] = currCubeRep[26];

			currCubeRep[24] = currCubeRep[15]; // move R bottom to F bottom
			currCubeRep[25] = currCubeRep[16];
			currCubeRep[26] = currCubeRep[17];

			currCubeRep[15] = currCubeRep[51]; // move B bottom to R bottom
			currCubeRep[16] = currCubeRep[52];
			currCubeRep[17] = currCubeRep[53];

			currCubeRep[51] = temp; // move L bottom to F bottom
			currCubeRep[52] = temp2;
			currCubeRep[53] = temp3;
		}
		break;
	case 'F':
		if (direction == 1)
		{
			currCubeRep = RotateFaceClockwise(currCubeRep, 18); // Rotate front face clockwise

			temp = currCubeRep[6];
			temp2 = currCubeRep[7];
			temp3 = currCubeRep[8]; // coppy U bottom

			currCubeRep[6] = currCubeRep[44]; // move L right to U bottom
			currCubeRep[7] = currCubeRep[41];
			currCubeRep[8] = currCubeRep[38];

			currCubeRep[38] = currCubeRep[27]; // move D top to L right
			currCubeRep[41] = currCubeRep[28];
			currCubeRep[44] = currCubeRep[29];

			currCubeRep[27] = currCubeRep[15]; // move R left to D top
			currCubeRep[28] = currCubeRep[12];
			currCubeRep[29] = currCubeRep[9];

			currCubeRep[9] = temp; // move L bottom to F bottom
			currCubeRep[12] = temp2;
			currCubeRep[15] = temp3;
		}
		else if (direction == -1)
		{
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 18); // Rotate front face counter-clockwise
			temp = currCubeRep[6];
			temp2 = currCubeRep[7];
			temp3 = currCubeRep[8]; // coppy U bottom

			currCubeRep[6] = currCubeRep[9]; // move R left to U bottom
			currCubeRep[7] = currCubeRep[12];
			currCubeRep[8] = currCubeRep[15];

			currCubeRep[15] = currCubeRep[27]; // move D top to R left
			currCubeRep[12] = currCubeRep[28];
			currCubeRep[9] = currCubeRep[29];

			currCubeRep[27] = currCubeRep[38]; // move L right to D top
			currCubeRep[28] = currCubeRep[41];
			currCubeRep[29] = currCubeRep[44];

			currCubeRep[44] = temp;
			currCubeRep[41] = temp2;
			currCubeRep[38] = temp3;
		}
		break;
	case 'B':
		if (direction == 1)
		{
			currCubeRep = RotateFaceClockwise(currCubeRep, 45); // Rotate back face clockwise
			temp = currCubeRep[0];
			temp2 = currCubeRep[1];
			temp3 = currCubeRep[2]; // coppy U top

			currCubeRep[0] = currCubeRep[11]; // move R right to U top
			currCubeRep[1] = currCubeRep[14];
			currCubeRep[2] = currCubeRep[17];

			currCubeRep[11] = currCubeRep[35]; // move D bottom to R right
			currCubeRep[14] = currCubeRep[34];
			currCubeRep[17] = currCubeRep[33];

			currCubeRep[33] = currCubeRep[36]; // move L left to D bottom
			currCubeRep[34] = currCubeRep[39];
			currCubeRep[35] = currCubeRep[42];

			currCubeRep[42] = temp;
			currCubeRep[39] = temp2; // U top to L left
			currCubeRep[36] = temp3;
		}
		else if (direction == -1)
		{
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 45); // Rotate back face counter-clockwise
			for (int i = 0; i < 3; i++)
			{ // B3 is same as B'
				temp = currCubeRep[0];
				temp2 = currCubeRep[1];
				temp3 = currCubeRep[2]; // coppy U top

				currCubeRep[0] = currCubeRep[11]; // move R right to U top
				currCubeRep[1] = currCubeRep[14];
				currCubeRep[2] = currCubeRep[17];

				currCubeRep[11] = currCubeRep[35]; // move D bottom to R right
				currCubeRep[14] = currCubeRep[34];
				currCubeRep[17] = currCubeRep[33];

				currCubeRep[33] = currCubeRep[36]; // move L left to D bottom
				currCubeRep[34] = currCubeRep[39];
				currCubeRep[35] = currCubeRep[42];

				currCubeRep[42] = temp;
				currCubeRep[39] = temp2; // U top to L left
				currCubeRep[36] = temp3;
			}
		}
		break;
	case 'L':
		if (direction == 1)
		{
			// Rotate left face clockwise and update adjacent pieces
			currCubeRep = RotateFaceClockwise(currCubeRep, 36); // Left face
			temp = currCubeRep[0];
			temp2 = currCubeRep[3];
			temp3 = currCubeRep[6]; // coppy U left

			currCubeRep[0] = currCubeRep[53]; // move B right to U left
			currCubeRep[3] = currCubeRep[50];
			currCubeRep[6] = currCubeRep[47];

			currCubeRep[47] = currCubeRep[33]; // move D left to B right
			currCubeRep[50] = currCubeRep[30];
			currCubeRep[53] = currCubeRep[27];

			currCubeRep[27] = currCubeRep[18]; // move F left to D left
			currCubeRep[30] = currCubeRep[21];
			currCubeRep[33] = currCubeRep[24];

			currCubeRep[18] = temp;
			currCubeRep[21] = temp2; // U top to L left
			currCubeRep[24] = temp3;
		}
		else if (direction == -1)
		{
			// Rotate left face counter-clockwise and update adjacent pieces
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 36); // Left face
			for (int i = 0; i < 3; i++)
			{ // B3 is same as B'
				temp = currCubeRep[0];
				temp2 = currCubeRep[3];
				temp3 = currCubeRep[6]; // coppy U left

				currCubeRep[0] = currCubeRep[53]; // move B right to U left
				currCubeRep[3] = currCubeRep[50];
				currCubeRep[6] = currCubeRep[47];

				currCubeRep[47] = currCubeRep[33]; // move D left to B right
				currCubeRep[50] = currCubeRep[30];
				currCubeRep[53] = currCubeRep[27];

				currCubeRep[27] = currCubeRep[18]; // move F left to D left
				currCubeRep[30] = currCubeRep[21];
				currCubeRep[33] = currCubeRep[24];

				currCubeRep[18] = temp;
				currCubeRep[21] = temp2; // U top to L left
				currCubeRep[24] = temp3;
			}
		}
		break;
	case 'R':
		if (direction == 1)
		{
			currCubeRep = RotateFaceClockwise(currCubeRep, 9); // Rotate right face clockwise
			temp = currCubeRep[2];
			temp2 = currCubeRep[5];
			temp3 = currCubeRep[8]; // coppy U right

			currCubeRep[2] = currCubeRep[20]; // move F right to U right
			currCubeRep[5] = currCubeRep[23];
			currCubeRep[8] = currCubeRep[26];

			currCubeRep[20] = currCubeRep[29]; // move D right to F right
			currCubeRep[23] = currCubeRep[32];
			currCubeRep[26] = currCubeRep[35];

			currCubeRep[29] = currCubeRep[51]; // move B left to D right
			currCubeRep[32] = currCubeRep[48];
			currCubeRep[35] = currCubeRep[45];

			currCubeRep[51] = temp;
			currCubeRep[48] = temp2; // U top to L left
			currCubeRep[45] = temp3;
		}
		else if (direction == -1)
		{
			currCubeRep = RotateFaceCounterClockwise(currCubeRep, 9); // Rotate right face counter-clockwise
			for (int i = 0; i < 3; i++)
			{ // B3 is same as B'
				temp = currCubeRep[2];
				temp2 = currCubeRep[5];
				temp3 = currCubeRep[8]; // coppy U right

				currCubeRep[2] = currCubeRep[20]; // move F right to U right
				currCubeRep[5] = currCubeRep[23];
				currCubeRep[8] = currCubeRep[26];

				currCubeRep[20] = currCubeRep[29]; // move D right to F right
				currCubeRep[23] = currCubeRep[32];
				currCubeRep[26] = currCubeRep[35];

				currCubeRep[29] = currCubeRep[51]; // move B left to D right
				currCubeRep[32] = currCubeRep[48];
				currCubeRep[35] = currCubeRep[45];

				currCubeRep[51] = temp;
				currCubeRep[48] = temp2; // U top to L left
				currCubeRep[45] = temp3;
			}
		}
		break;

	default:
		std::cout << "Invalid action!" << std::endl;
		break;
	}
	return currCubeRep;
}

void Camera::TranslateView(glm::vec3 trans){
    m_View = glm::translate(m_View, trans);
}

float updateDimentionY(Camera *camera){
	float radius = glm::length(camera->m_Position);
	return radius * sin(glm::radians(camera->m_RotationAngleX));
}

float updateDimentionX(Camera *camera){
	float radius = glm::length(camera->m_Position);
	return radius * sin(glm::radians(camera->m_RotationAngleY)) * cos(glm::radians(camera->m_RotationAngleX));
}

float updateDimentionZ(Camera *camera){
	float radius = glm::length(camera->m_Position);
	return radius * cos(glm::radians(camera->m_RotationAngleY)) * cos(glm::radians(camera->m_RotationAngleX));
}


// Move up along the up vector
void Camera::handleUpArrow() {
    std::cout << "UP Arrow pressed" << std::endl;
    m_Position -= m_Up * 0.1f;
    UpdateViewMatrix();
}


// Move down along the up vector
void Camera::handleDownArrow() {
    std::cout << "DOWN Arrow pressed" << std::endl;
    m_Position += m_Up * 0.1f;
    UpdateViewMatrix();
}

// Move the camera left relative to its orientation
void Camera::handleLeftArrow() {
    std::cout << "LEFT Arrow pressed" << std::endl;
    glm::vec3 right = glm::normalize(glm::cross(m_Orientation, m_Up));
    m_Position += right * 0.1f;
    UpdateViewMatrix();
}

// Move the camera right relative to its orientation
void Camera::handleRightArrow() {
    std::cout << "RIGHT Arrow pressed" << std::endl;
    glm::vec3 right = glm::normalize(glm::cross(m_Orientation, m_Up));
    m_Position -= right * 0.1f;
    UpdateViewMatrix();
}