/*
 * CECS 545
 * Fall 2017
 * Samual Chesak
 * 
*/
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <thread>

#include "City.hpp"
#include "SalesmanRoute.hpp"
#include "TsmHelper.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
vector<City> ReadCities(string filePath);
void PrintRoute(SalesmanRoute route);
void PrintTravsversedRoute(SalesmanRoute route);
void PrintGraphRoute(SalesmanRoute route);
SalesmanRoute GeneticAlgorithm(vector<City> cities, int populationSize, int generations, bool reverseCrossover, bool adaptiveMutator);
void drawRoute(SalesmanRoute route, Shader shader, float offSetX, float offSety);
void drawGraph(vector<GraphPoint> graph, Shader shader, int generations, float offSetX, float offSety, float scaleFactor);


int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TSP: Genetic Algorithm", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	
	Shader shader("../vshader.glsl", "../fshader.glsl");
	shader.set4f("myColor", 0.498f, 1.0f, 0.0f, 1.0f);
	
	//parse file and populate cities
	vector<City> cities;
	cities = ReadCities(argv[1]);
	
	vector<thread> algorithmThreads;
	vector<TsmHelper*> tsmHelpers;
	int generations = 1000;
	
	for(int i = 0; i < 4; i++)
	{
		TsmHelper* tsmHelper = new TsmHelper(cities);
		tsmHelpers.push_back(tsmHelper);
	}
	
	//start timer
	clock_t start;
	double runTime;
	start = clock();
	
	//create and start threads
	algorithmThreads.push_back(thread(&TsmHelper::GeneticStatistics, tsmHelpers[0], 10, 100, generations, false, false));
	algorithmThreads.push_back(thread(&TsmHelper::GeneticStatistics, tsmHelpers[1], 10, 100, generations, true, false));
	algorithmThreads.push_back(thread(&TsmHelper::GeneticStatistics, tsmHelpers[2], 10, 100, generations, false, true));
	algorithmThreads.push_back(thread(&TsmHelper::GeneticStatistics, tsmHelpers[3], 10, 100, generations, true, true));
	
	//render loop
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//draw routes
		drawRoute((*tsmHelpers[0]).GetBestRoute(), shader, -0.5, 0.5);
		drawRoute((*tsmHelpers[1]).GetBestRoute(), shader, 0.5, 0.5);
		drawRoute((*tsmHelpers[2]).GetBestRoute(), shader, -0.5, -0.5);
		drawRoute((*tsmHelpers[3]).GetBestRoute(), shader, 0.5, -0.5);
		
		//draw graphs for routes
		drawGraph((*tsmHelpers[0]).GetGenerationGraph(), shader, generations, -0.5f, 0.0f, 0.25f);
		drawGraph((*tsmHelpers[1]).GetGenerationGraph(), shader, generations, 0.5f, 0.0f, 0.25f);
		drawGraph((*tsmHelpers[2]).GetGenerationGraph(), shader, generations, -0.5f, -0.25f, 0.25f);
		drawGraph((*tsmHelpers[3]).GetGenerationGraph(), shader, generations, 0.5f, -0.25f, 0.25f);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		for(int i = 0; i < 4; i++)
		{
			cout << "Generation " << (*tsmHelpers[i]).GetCurrentGeneration() << ": " << (*tsmHelpers[i]).GetBestRoute().GetTotalDistance() << endl;
		}
		cout << endl;
		
		
		chrono::milliseconds dura(500);
		this_thread::sleep_for(dura);
	}
	
	glfwTerminate();
	
	for(unsigned int i = 0; i < algorithmThreads.size(); i++)
	{
		if(algorithmThreads[i].joinable())
		{
			algorithmThreads[i].join();
		}
	}
	
	for(unsigned int i = 0; i < tsmHelpers.size(); i++)
	{
		cout << "Algorithm Variation: " << i << endl
			<< "Average: " << (*tsmHelpers[i]).routeStats.average << endl
			<< "Standard Deviation: " << (*tsmHelpers[i]).routeStats.stdDeviation << endl
			<< "Max: " << (*tsmHelpers[i]).routeStats.max << endl
			<< "Min: " << (*tsmHelpers[i]).routeStats.min << endl << endl;
	}
	
	//end timer and print output
	runTime = (clock() - start)/(double)CLOCKS_PER_SEC;
	cout << "Run time: " << runTime << endl;
	
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void drawRoute(SalesmanRoute route, Shader shader, float offSetX, float offSety)
{
	vector<float> vertices;
	
	for(unsigned int i = 0; i < route.GetRoute().size(); i++)
	{
		vertices.push_back((float)route.GetRoute()[i]._latitude/200 + offSetX - 0.25f);
		vertices.push_back((float)route.GetRoute()[i]._longitude/200 + offSety - 0.25f);
		vertices.push_back(0.0f);
	}
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	
	glBindVertexArray(vao);
	
	shader.set4f("myColor", 0.498f, 1.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	shader.setFloat("myPointSize", 5.0f);
	
	glUseProgram(shader.Id);
	glDrawArrays(GL_LINE_LOOP, 0, vertices.size()/3);
	
	shader.set4f("myColor", 0.275f, 0.519f, 0.706f, 1.0f);
	glDrawArrays(GL_POINTS, 0, vertices.size()/3);
	glDisableVertexAttribArray(0);
}

void drawGraph(vector<GraphPoint> graph, Shader shader, int generations, float offSetX, float offSety, float scaleFactor)
{
	vector<float> vertices;
	double normalSize = graph[0].distance;
	for(unsigned int i = 0; i < graph.size(); i++)
	{
		vertices.push_back((float)graph[i].generation/generations * scaleFactor + offSetX);
		vertices.push_back((float)graph[i].distance/normalSize * scaleFactor + offSety);
		vertices.push_back(0.0f);
	}
	
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	
	glBindVertexArray(vao);
	
	shader.set4f("myColor", 0.498f, 1.0f, 0.0f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	shader.setFloat("myPointSize", 1.0f);
	
	glUseProgram(shader.Id);
	glDrawArrays(GL_POINTS, 0, vertices.size()/3);
}

vector<City> ReadCities(string filePath)
{
	vector<City> cities;
	ifstream inFile(filePath);
	string currentLine;
	if(!inFile)
	{
		cerr << "Unable to open system Cities file path.";
	}
	size_t currentPosition;
	bool sectionFound = false;
	while(getline(inFile, currentLine))
	{
		if(sectionFound)
		{
			vector<string> splitLine;
			
			istringstream iss(currentLine);
			string token;
			while(getline(iss, token, ' '))
			{
				splitLine.push_back(token);
			}
			
			City newCity(stod(splitLine[1]),stod(splitLine[2]),stoi(splitLine[0])); 
			cities.push_back(newCity);
		} else {
			currentPosition = currentLine.find("NODE_COORD_SECTION");
			if(currentPosition != string::npos)
			{
				sectionFound = true;
			}
		}
	}
	
	return cities;
}

void PrintRoute(SalesmanRoute route)
{
	vector<City> cities = route.GetRoute();
	
	cout << "Route: ";
	
	for(unsigned int i = 0; i < cities.size(); i++)
	{
		cout << cities[i].CityId << "->";
	}
	
	cout << cities[0].CityId << endl;
}