#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>

// using namespace std;

class Node {

	public :
		int index, x, y;

		Node (int index, int x, int y) {
			this->index = index;
			this->x = x;
			this->y = y;
		}

		float getEdgeLength (Node node2) {
			return sqrt(pow((this->x - node2.x),2) + pow((this->y - node2.y),2));
		}
};


class Graph {

	public : 
		std::string name, comment, type, edgeWeightType;
		int dimension;
		std::vector<Node> nodes;

	Graph (std::string filePath) {

		std::ifstream inputFile;
		std::string line;
		inputFile.open (filePath, std::ios::in | std::ios::binary);
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
		// std::cout << name << comment << line;
		while ( getline(inputFile, line) ) {

			if ( line.compare("EOF") == 0 ) {
				break;
			}
			else {

				std::string word;
				int x, y, index;
				std::stringstream ss(line);
				ss >> word;
				// std::cout<<line;
				// std::cout << x << y << "\n";
				index = std::stoi(word);
				ss >> word;
				// std::cout<<word;
				x = std::stoi(word);
				ss >> word;
				y = std::stoi(word);
				nodes.push_back(Node(index,x,y));
				std::cout << index <<' '<< x <<' '<< y <<'\n';
			}
		}
	}
};

int main() {

	Graph graph("/home/rohan/818B_HW1_TSP/data/eil51.tsp");

	return 0;
}