#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <chrono>

/* NOTES
* - should have used heap for neighbours
* - should have used maps for storing nodes
*/

class Node {

	public :
		int index, x, y;										  // Node index and x,y coordinates
		std::vector <std::pair <float, int>> neighbours;          // Vector of neighbours of the node. Sorted in incresing order of length.
		std::vector <std::pair <float, int>> MSTneighbours;		  // Vector of neighbours of the node in MST.

		Node (int index, int x, int y) {
			this->index = index;
			this->x = x;
			this->y = y;
		}

		float GetEdgeLength (Node node2) {
			return sqrt(pow((this->x - node2.x),2) + pow((this->y - node2.y),2));
		}

		void InsertNeighbour (Node node2) {
			neighbours.push_back (std::make_pair (GetEdgeLength(node2),node2.index) );
		}

		void SortNeighbours () {
			std::sort (neighbours.begin(), neighbours.end());
		}

		void TestPrint (){
			// std::cout<<"*********************************\n";
			// for (auto e : neighbours) {
			// 	std::cout<<e.first<<" "<<e.second<<std::endl;
			// }
			// std::cout<<"*********************************\n";
			std::cout<<"*********************************\n";
			for (auto e : MSTneighbours) {
				std::cout<<e.first<<" "<<e.second<<std::endl;		
			}
			std::cout<<"*********************************\n";
		}
};


class Graph {

	public : 
		typedef std::pair<float, std::pair<int,int>> np;

		std::string name, comment, type, edgeWeightType;
		std::string inputName;
		int dimension;
		std::vector<Node> nodes;											// Vector of all nodes
		int size;
		std::vector <std::pair <int,int> > MST;								// Not used - just storing MST in some form I didn't use
		float MSTlen;
		float pathLen;
		std::vector <int> tour;												// Vectour of final tour indices in order
		std::priority_queue<np, std::vector<np>, std::greater<np>> nn;      // Vector of potential edges for MST

	Graph (std::string str) {												// Read File and store in vector of nodes
		inputName = str;
		std::ifstream inputFile;
		std::string line;
		inputFile.open ("data/" + inputName + ".tsp", std::ios::in | std::ios::binary);
		getline (inputFile, line);
		name = &line[7];
		getline (inputFile, line);
		comment = &line[10];
		getline (inputFile, line);
		type = &line[7];
		getline (inputFile, line);
		dimension = std::stoi(&line[12]);
		getline (inputFile, line);
		edgeWeightType = &line[19];
		getline (inputFile, line);
		while ( getline(inputFile, line) ) {

			if ( line.compare("EOF") == 0 ) {
				break;
			}
			else {

				std::string word;
				int x, y, index;
				std::stringstream ss(line);
				ss >> word;
				index = std::stoi(word);
				ss >> word;
				x = std::stoi(word);
				ss >> word;
				y = std::stoi(word);
				nodes.push_back(Node(index,x,y));
			}
		}

		size = nodes.size();

		for (int i=0; i<size; i++) {
			for (int j=0; j<size; j++) {
				if (i != j) {
					nodes[i].InsertNeighbour(nodes[j]);
				}
			}
			nodes[i].SortNeighbours();
			// nodes[i].TestPrint();
		}
	}

	void PrintMST (){
		std::cout<<"*********************************\n";
		for (auto e : MST) {
			std::cout<<e.first<<" "<<e.second<<std::endl;
		}
		std::cout<<"*********************************\n";
		std::cout<<"MST total no. of nodes : "<<MST.size()<<std::endl;
		std::cout<<"MST length : "<<MSTlen<<std::endl;
	}

	void GenMST () {
		// Start with first node
		std::unordered_set<int> visited;
		np cp;
		float sum = 0;
		visited.insert(nodes[0].index);
		nn.push(std::make_pair(nodes[0].neighbours[0].first, std::make_pair(nodes[0].index, nodes[0].neighbours[0].second)));
		nodes[0].neighbours.erase(nodes[0].neighbours.begin());
		while (visited.size() < size) {
			cp = nn.top();
			nn.pop();
			int cppos = cp.second.first - 1;
			int cpnin = cp.second.second - 1;
			float cpl = cp.first;
			if (visited.count(cp.second.second) == 1) {	   		// If cycle is being formed
				nn.push(std::make_pair(nodes[cpnin].neighbours[0].first, 
							std::make_pair(nodes[cpnin].index, nodes[cpnin].neighbours[0].second)));
				nodes[cpnin].neighbours.erase(nodes[cpnin].neighbours.begin());
			} else {
				nodes[cppos].MSTneighbours.push_back(std::make_pair(cpl,cp.second.second));
				MST.push_back(cp.second);
				sum += cpl;
				nn.push(std::make_pair(nodes[cpnin].neighbours[0].first, std::make_pair(nodes[cpnin].index, nodes[cpnin].neighbours[0].second)));
				nodes[cpnin].neighbours.erase(nodes[cpnin].neighbours.begin());
				visited.insert(cp.second.second);
			}
			nn.push(std::make_pair(nodes[cppos].neighbours[0].first, std::make_pair(nodes[cppos].index, nodes[cppos].neighbours[0].second)));
			nodes[cppos].neighbours.erase(nodes[cppos].neighbours.begin());
		}

		for (int i=0; i<size; i++) {
			std::sort(nodes[i].MSTneighbours.begin(), nodes[i].MSTneighbours.end(), std::greater<std::pair<float, int>>());
			// nodes[i].TestPrint();
		}
		MSTlen = sum;
		PrintMST();
	}

	void GenPathHuer0 () {

		std::priority_queue<np, std::vector<np>, std::greater<np>> pp;
		// std::unordered_set<int> visited;
		std::vector<int> path;
		std::vector<int> rootStack;
		int startNodeIndex = 1;
		path.push_back(startNodeIndex);
		rootStack.push_back(startNodeIndex);
		// visited.insert(startNodeIndex);
		while (path.size() < size) {
			int cn = rootStack.back();
			if (nodes[cn-1].MSTneighbours.size() == 0) {	// If leaf node or repeated node is reached
				rootStack.pop_back();
			} else {
				rootStack.push_back(nodes[cn-1].MSTneighbours.back().second);
				nodes[cn-1].MSTneighbours.pop_back();
				path.push_back(rootStack.back());
				// visited.insert(cn);
			}
		}
		path.push_back(startNodeIndex);
		float sum = 0;
		std::cout<<"*********************************\n";
		std::cout << "Path : ";
		for (int i = 0; i < path.size() - 1; i++) {
			sum += nodes[path[i]-1].GetEdgeLength(nodes[path[i+1]-1]);
			std::cout<< path[i] << "->";
		}
		std::cout << startNodeIndex;
		pathLen = sum;
		std::cout << "\nPath length is : " << pathLen << std::endl;
		std::cout<<"*********************************\n";
		tour = path;
	}

	void WriteTour () {
		std::ofstream outputFile;
		outputFile.open("output/" + inputName + ".out.tour", std::ios::out | std::ios::binary);
		outputFile << "NAME : " << inputName + ".out.tour" << std::endl;
		outputFile << "COMMENT : Tour for " << inputName + ".tsp (Length " << pathLen << ")" << std::endl;
		outputFile << "TYPE : TOUR" << std::endl;
		outputFile << "DIMENSION : " << dimension << std::endl;
		outputFile << "TOUR_SECTION " << std::endl;
		for (int i = 0; i < tour.size()-1; i++) {
			outputFile << tour[i] << std::endl;
		}
		outputFile << "-1" << std::endl << "EOF" << std::endl;
		outputFile.close();
	}
};
		
int main() {
	
	// std::string inputName;
	// int n = 300;
	// for (int i = 1; i <=10; i++) {
	// 	inputName = "rnd/rnd" + std::to_string(n) + std::to_string(i);
	// 	std::cout<< "\n\n\n\n" << inputName;
	// 	Graph graph(inputName);
	// 	auto startTime = std::chrono::high_resolution_clock::now();
	// 	graph.GenMST();
	// 	graph.GenPathHuer0();
	// 	auto endTime = std::chrono::high_resolution_clock::now();
	// 	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	// 	std::cout << "Duration : " << duration.count() << " microseconds" << std::endl;
	// 	graph.WriteTour();
	// }

	std::string inputName;
	std::cout << "Enter file name : ";
	std::cin >> inputName;
	Graph graph(inputName);
	auto startTime = std::chrono::high_resolution_clock::now();
	graph.GenMST();
	graph.GenPathHuer0();
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
	std::cout << "Duration : " << duration.count() << " microseconds" << std::endl;
	graph.WriteTour();

	return 0;
}