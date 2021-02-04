//=======================================================================
// Original Authors: Raymond Lo and William Lo
// Modified by: Zheng Zu
//=======================================================================

//force the application to compile with radians measure only.
#define GLM_FORCE_RADIANS
#define VIDEO_SOBEL

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#ifdef _WIN32
#define GLEW_STATIC
#endif 

//define version
#define VERSION "1.0.0"

//Define les code du couleur

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */





//GLFW library
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLM library, short for opengl math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Common libraries
#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "common.h"


//Global variables
GLFWwindow* g_window;
int interval=41667; //in microseconds between each frame

 int WINDOWS_WIDTH = 1280;
 int WINDOWS_HEIGHT = 720;

float aspect_ratio = 16.0/9.0f;
float z_offset = 1.0f;
float rotateY = 0.0f;
float rotateX = 0.0f;


//our vertices
static const GLfloat g_vertex_buffer_data[] = {
	-aspect_ratio,-1.0f,z_offset,
	aspect_ratio,-1.0f,z_offset,
	aspect_ratio,1.0f,z_offset,
	-aspect_ratio,-1.0f,z_offset,
	aspect_ratio,1.0f,z_offset,
	-aspect_ratio,1.0f,z_offset,
};

//UV map for the vertices
static const GLfloat g_uv_buffer_data[] = {
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
};


//========================================================================
// Handle key strokes
//========================================================================
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    
  
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;
    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            rotateX=0;
            rotateY=0;
            break;
        case GLFW_KEY_W:
            rotateX+=0.01;
            break;
        case GLFW_KEY_S:
            rotateX-=0.01;
            break;
        case GLFW_KEY_A:
            rotateY+=0.01;
            break;
        case GLFW_KEY_D:
            rotateY-=0.01;
            break;
        default:
            break;
    }
}



int main(int argc, char **argv)
{
	//Initialize GLFW
	if(!glfwInit()){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		exit(EXIT_FAILURE);
	}

	//enable anti-alising 4x with GLFW
	glfwWindowHint(GLFW_SAMPLES, 4);
	//specify the client API version that the created context must be compatible with.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//make GLFW forward compatible
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//use the OpenGL Core (http://www.opengl.org/wiki/Core_And_Compatibility_in_Contexts)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create a GLFW windows object
	g_window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Edge Detecteur 1.0.0", NULL, NULL);
	if(!g_window){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
    


	//make the context of the specified window current for the calling thread
	glfwMakeContextCurrent(g_window);
	glfwSwapInterval(1);
    

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Final to Initialize GLEW\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//keyboard input callback
	glfwSetInputMode(g_window,GLFW_STICKY_KEYS,GL_TRUE);
	glfwSetKeyCallback(g_window, key_callback);

	//Keyboard shortcuts
   // std::system("Color 0A");  windows only
    std::cout<<std::endl;
    std::cout<<GREEN<<"EDGE DETECTEUR: Version "<<RED<<VERSION<<std::endl; 

    std::cout<<GREEN<<"This program uses a filter to perform edge detection "<<std::endl; 
    std::cout<<"Use Arrows keys and W,A,S,D for manipulation"<<std::endl;  
    std::cout<<endl;
	//black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	//create and compile our GLSL program from the shaders

#ifdef	VIDEO_SOBEL
	GLuint program_id = LoadShaders( "transform.vert", "texture_sobel.frag" );
#else
	GLuint program_id = LoadShaders( "transform.vert", "texture.frag" );
    
#endif



	char *filepath;

	//load the texture from image with SOIL
	if(argc<2){
		filepath = (char*)malloc(sizeof(char)*512);
		sprintf(filepath, "video.mov");
	}
	else{
		filepath = argv[1];
	}
      

    
    //Handle video input with OpenCV
	VideoCapture cap(filepath); // open the default camera
	Mat frame;


	if (!cap.isOpened()){  // check if we succeeded
		std::cout<<GREEN<<"Can't open files"<<std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}else{

        cap >> frame; // get a new frame from the camera
                
            
		std::cout<<GREEN<<"Acquired vidéo stream "<<RED<<frame.size().width<<"X"<<RED<<frame.size().height<<GREEN<<std::endl;
	}
    
		std::cout<<GREEN<<"Playing back at approximately "<<RED<<(double)1000000/interval<<GREEN<<" frames per second"<<std::endl;

    
    cap >> frame; // get a new frame from the camera
    

	GLuint texture_id = initializeTexture(frame.data, frame.size().width, frame.size().height, GL_BGR);
	aspect_ratio = (float)frame.size().width/(float)frame.size().height;

	//get the location of our "MVP" uniform variable
	GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

	//get the location of our "textureSampler" uniform variable
	GLuint texture_sampler_id  = glGetUniformLocation(program_id, "textureSampler");

	//get the location of the attribute variables
	GLint attribute_vertex, attribute_uv;

	attribute_vertex = glGetAttribLocation(program_id, "vertexPosition_modelspace");
	attribute_uv = glGetAttribLocation(program_id, "vertexUV");

	//generate the Vertex Array Object (Depedency: GLEW)
	GLuint vertex_array_id;
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	//initialize the vertex buffer memory (similar to malloc)
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//initialize the UV buffer memory
	GLuint uv_buffer;
	//generate the array (like malloc)
	glGenBuffers(1, &uv_buffer);
	//bind the array buffer to that UV
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	//use our shader
	glUseProgram(program_id);

	//bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(texture_sampler_id, 0);

	//1st attribute buffer : vertices for position
	glEnableVertexAttribArray(attribute_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glVertexAttribPointer(attribute_vertex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//2nd attribute buffer : UV mapping
	glEnableVertexAttribArray(attribute_uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	do{
		//clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // make background blanche and transparent
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

		//compute the MVP matrix from keyboard and mouse input
		computeViewProjectionMatrices(g_window);

		//get the View and Model Matrix and apply to the rendering
		glm::mat4 projection_matrix = getProjectionMatrix();
		glm::mat4 view_matrix = getViewMatrix();
		glm::mat4 model_matrix = glm::mat4(1.0);
		model_matrix = glm::rotate(model_matrix, glm::pi<float>() * rotateY, glm::vec3(0.0f, 1.0f, 0.0f));
		model_matrix = glm::rotate(model_matrix, glm::pi<float>() * rotateX, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;

		//send our transformation to the currently bound shader,
		//in the "MVP" uniform variable
		glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

        
        
        //get the video feed, reset to beginning if it reaches the end of the video
		if(!cap.grab()){
			//printf("End of Video, Resetting\n");
			cap.release();
			cap.open(filepath); // open the default camera
		}

        std::this_thread::sleep_for (std::chrono::microseconds(interval)); //approximate 
    
		cap >> frame; // get a new frame from camera
   
        
		//update the texture with the new frame
		updateTexture(frame.data, frame.size().width, frame.size().height, GL_BGR);

		glDrawArrays(GL_TRIANGLES, 0, 6); //draw the square

		//swap buffers
		glfwSwapBuffers(g_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while(!glfwWindowShouldClose(g_window) && glfwGetKey(g_window, GLFW_KEY_ESCAPE )!=GLFW_PRESS);

	glDisableVertexAttribArray(attribute_vertex);
	glDisableVertexAttribArray(attribute_uv);

	// Clean up VBO and shader
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &uv_buffer);
	glDeleteProgram(program_id);
	glDeleteTextures(1, &texture_id);
	glDeleteVertexArrays(1, &vertex_array_id);

	// Close OpenGL window and terminate GLFW
	glfwDestroyWindow(g_window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}

