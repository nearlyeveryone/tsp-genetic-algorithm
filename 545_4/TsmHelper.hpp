#ifndef TSMHELPER_HPP
#define TSMHELPER_HPP

#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

#include "City.hpp"
#include "SalesmanRoute.hpp"

struct GraphPoint
{
	int generation;
	double distance;
};

struct RouteStatistics
{
	double min = -1;
	double max = -1;
	double average = -1;
	double stdDeviation = -1;
};

class TsmHelper
{
private:
	SalesmanRoute _bestRoute;
	mutex muteRoute;
	int _currentGeneration = 0;
	mutex muteGeneration;
	vector<GraphPoint> generationGraph;
	mutex muteGraph;
	void setBestRoute(SalesmanRoute route);
	void setCurrentGeneration(int generation);
	void addPointToGraph(int generation, double distance);
	void clearGraphPoints();
	
public:
	TsmHelper();
	TsmHelper(vector<City>);
	SalesmanRoute GetBestRoute();  
	int GetCurrentGeneration();
	vector<GraphPoint> GetGenerationGraph();
	void GeneticAlgorithm(int populationSize, int generations, bool reverseCrossover, bool adaptiveMutator);
	void GeneticStatistics(int sampleSize, int populationSize, int generations, bool reverseCrossover, bool adaptiveMutator);
	struct RouteStatistics routeStats;
	~TsmHelper(); 

};

#endif // TSMHELPER_HPP
