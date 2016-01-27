#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <sstream>
#include <string>
#include <vector>
#include "time.h"


using namespace std;

int *Dijkstra(int **GR, int V, int st) {  
	int *distance, count, index, i, u;
	bool *visited;
	distance = new int[V];
	visited = new bool[V];
	for (i = 0; i<V; i++) 
	{
		distance[i] = INT_MAX; 
		visited[i] = false; 
	}
	distance[st] = 0; 
	int num_of_threads = omp_get_num_threads();
	int *mins = new int[num_of_threads];
	int *index1 = new int[num_of_threads];
	for (count = 0; count < V - 1; count++) {
		int minimum = INT_MAX;
		index = 0;
#pragma omp parallel
		{
#pragma omp for private(i)
			int  id = omp_get_num_threads();
			for (i = 0; i < V; i++) {
				if (!visited[i] && distance[i] < minimum)
				{
					mins[id] = distance[i];
					index1[id] = i;
				}
			}
#pragma omp critical
			for (int j = 0; j < num_of_threads; j++) {
				if (mins[j] > minimum) {
					minimum = mins[j];
					index = index1[j];
				}
			}
		}
		u = index;
		visited[u] = true;
		for (i = 0; i < V; i++)
			if (!visited[i] && GR[u][i] && distance[u] != INT_MAX && distance[u] + GR[u][i] < distance[i])
				distance[i] = distance[u] + GR[u][i];
		}
		return distance;
}

int getSize(char *str, int n)
{
	ifstream Prog(str);
	cout << str;
	int count = 0;
	int cc;
	while (!Prog.eof()) {
		Prog >> cc;
		++count;
	}
	n = (int)sqrt((double)count);
	return n;
	
}
void readLoadFile1(char *str, int **a, int n )
{
	ifstream in(str);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j)
		{
			in >> a[i][j];
		}
	}
}

int main(int argc, char * argv[]) {
	//setlocale(LC_ALL, "Rus");
	setlocale(LC_CTYPE, "Russian");
	if (argc != 3)
	{
		cout << "Enter command arguments" << endl;
		return -1;
	}
	else
	{
	int start;
	int V = 0; 
	V =	getSize(argv[1],V);
	int **GR = new int *[V];
	for (int i = 0; i<V; i++)
		GR[i] = new int[V];
		readLoadFile1(argv[1], GR, V);
	cout << "matrix load";
	start = 0; 
	clock_t start1, end;
	start1 = clock();
	int *distance = Dijkstra(GR, V, start); 
	end = clock();
	printf(" %1.4f second(s)\n", ((double)end - start1) / ((double)CLOCKS_PER_SEC));
	 
	std::ofstream infile(argv[2]);
	for (int i = 0; i < V; i++) {	
		string str = ""; 
		if (distance[i] != INT_MAX) {
			//cout << start + 1 << " > " << i + 1 << " = " << distance[i] << endl;
			infile << start + 1 << " > " << i + 1 << " = " << distance[i] << "\n ";
		}
		else { 
			//cout << start + 1 << " > " << i + 1 << " = " << "INF" << endl; 
			infile << start + 1 << " > " << i + 1 << " = " << "INF" << "\n ";
		}
	}
	}
	return 0;
}