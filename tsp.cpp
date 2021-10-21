#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <utility>
#include <vector>

using namespace std;



// --- Auxiliary Functions --- //


// - Print Functions

void printPairVector(vector<pair<int,int>> v){
	cout << "[ ";
	// Print each point in v between parentheses and seperated by a comma
	for(long unsigned i = 0; i<v.size(); i++){
		cout << "(" << v[i].first << "," << v[i].second << "), ";
	}
	cout << "]\n";
}

void printMenu(char* title, string options[], int count, int ident){

	// Function for printing menus in main

	for(int i = 0; i<(int)strlen(title)+ident; i++){ printf("-");}printf("\n");
	for(int i = 0; i<ident/2;i++){printf(" ");} cout << title << endl;
	for(int i = 0; i<(int)strlen(title)+ident; i++){ printf("-");}printf("\n");
	for(int i = 0; i<count; i++){
		printf("%d) ", i+1);
		cout << options[i] << endl;
	}
}


// - Read Functions

bool readInt(int* op){

	// Function to read user input and use it as an integer
	// Non digit characters are ignored

	long toInt;
	char buffer[1024];

	if(fgets(buffer, 1024, stdin) != NULL){
		size_t len = strlen(buffer);

		// Clear stdin if more than 1024 characters are read
		if(len > 0 && buffer[len-1] != '\n'){
			int clear;
			while((clear = getchar()) != '\n' && clear != EOF);
		}
	}
	else{ printf("\n"); exit(1);}

	// Transform string input into long and ignore not digit characters
	char* end; errno = 0;
	toInt = strtol(buffer, &end, 10);

	// Check errors

	if(errno == ERANGE){ return false; }

	if(end == buffer){ return false; }

	if(*end && *end != '\n'){ return false; }

	if(toInt > INT_MAX || toInt < INT_MIN){ return false; }

	*op = (int)toInt;
	return true;
}

// - Save result of core functions to textfile so it can be plotted

void saveTour(vector<pair<int,int>> v){

	// Ask user if they want to save a given result
	cout << "Save current result?[Y or N] > ";
 	int opt = getchar();
 	if(opt == 'Y' || opt == 'y'){

		// Save result to a file called tour.txt
		ofstream file;
		file.open("tour.txt");
		for(unsigned long i = 0; i<v.size(); i++){
			file << v[i].first << " " << v[i].second << endl;
		}
		cout << "Result as been saved to tour.txt" << endl;
	}
	else if(opt == 'N' || opt == 'n'){
		cout << "Result was not saved" << endl;
	}
	else{
		cout << "Invalid Option" << endl;
	}
}





// --- Core Functions --- //


// - Generating a random candidate

vector<pair<int,int>> genRandomPoints(int n, int bound){

	// Generates a vector of random points

	if(n >= (bound+bound+1)*(bound+bound+1) || n < 3){
		// Check if n points exceed the number of different points in specified bound
		// Make sure negative numbers can't be selecter, also exclude trivial cases such as n = 1 and n = 2
		puts("Error: invalid combination of values");
		exit(1);
	}

	vector<pair<int,int>> ret;

	// Mersenne Twister for generating random points
	random_device rd; mt19937 gen(rd());
	uniform_int_distribution<> randInt(-bound,bound);

	// Set to hold all points generated thus far
	set<pair<int,int>> points;
	set<pair<int,int>>::iterator it;

	while((int)ret.size() != n){
		int x = randInt(gen); int y = randInt(gen);
		pair<int,int> cur (x,y);

		// Guarantee the same point isn't added twice using set points
		it = points.find(cur);
		if ( it == points.end()){
			ret.push_back(cur);
			points.insert(cur);
		}
	}
	return ret;
}


// - Generating a random permutarion of given candidate

void pairShuffle(vector<pair<int,int>>* v){
	random_shuffle(v -> begin(), v -> end());
}


// - Nearest-neighbour heuristic

vector<pair<int,int>> nearestNeighbourFirst(vector<pair<int,int>> v, int startIndex){

	// Add content of starting index to the return vector and mark it as the current point
	vector<pair<int,int>> ret;
	pair<int,int> cur = v.at(startIndex);
	ret.push_back(cur);

	// Create a set that contains all points in vector and erase points already visited
	set<pair<int,int>> unvisited(v.begin(), v.end());
	unvisited.erase(cur);

	// While any unvisited points remain in the set look for the nearest point still in the set:
	//	- remove it from the set, add it to the return vector, and set it as the current point
	while(!unvisited.empty()){
		pair<int,int> toAdd;
		int min = INT_MAX;
		for(auto pair: unvisited){
			int curDist = abs(pair.first - cur.first)*abs(pair.first - cur.first)+abs(pair.second - cur.second)*abs(pair.second - cur.second);
			if(curDist < min){min = curDist; toAdd = pair;}
		}
		ret.push_back(toAdd);
		unvisited.erase(toAdd);
		cur = toAdd;

	}
	return ret;
}


// - Determining the 2-exchange neighbours of given candidate v

bool inBox(pair<int,int> p1, pair<int,int> p2, pair<int,int> p3){

	// Auxiliary function for determining if two given segments, p1p2 and p3p4, intersect
	return ( min(p1.first, p2.first) <= p3.first && p3.first <= max(p1.first, p2.first) ) && ( min(p1.second, p2.second) <= p3.second && p3.second <= max(p1.second, p2.second) );
}

bool checkIntersection(pair<int,int> p1, pair<int,int> p2, pair<int,int> p3, pair<int,int> p4){

	// Returs true if segment p1p2 and sengment p3p4 intersect

	long long d123 = (p3.first-p1.first)*(p2.second-p1.second)-(p3.second-p1.second)*(p2.first-p1.first);
	long long d124 = (p4.first-p1.first)*(p2.second-p1.second)-(p4.second-p1.second)*(p2.first-p1.first);
	long long d341 = (p1.first-p3.first)*(p4.second-p3.second)-(p1.second-p3.second)*(p4.first-p3.first);
	long long d342 = (p2.first-p3.first)*(p4.second-p3.second)-(p2.second-p3.second)*(p4.first-p3.first);
	if( ((d123 < 0 && d124 > 0) || (d123 > 0 && d124 < 0)) && ((d341 < 0 && d342 > 0) || (d341 > 0 && d342 < 0)) ){
		return true;
	}
	else if(d123 == 0 && inBox(p1,p2,p3)){
		return true;
	}
	else if(d124 == 0 && inBox(p1,p2,p4)){
		return true;
	}
	else if(d341 == 0 && inBox(p3,p4,p1)){
		return true;
	}
	else if(d342 == 0 && inBox(p3,p4,p2)){
		return true;
	}
	else{ return false; }
}


vector<pair<int,int>> getIntersections(vector<pair<int,int>> v){

	// Finds and returns all intersections in v

	v.push_back(v[0]);
	vector<pair<int,int>> intersections;
	for(unsigned long i = 0; i<v.size()-3; i++){

		for(unsigned long j = i+2; j<v.size()-1; j++){

			if(i == 0 && j == v.size() - 2){ continue; }

			int flag = 1;

			// Check collinearity, expression equals 0 if segment (v[i],v[i+1]) and segment (v[j],v[j+1]) are collinear
			int areCollinear = (v[i+1].first - v[i].first)*(v[j+1].second - v[j].second) - (v[i+1].second - v[i].second)*(v[j+1].first - v[j].first);
			int dotProduct = (v[i+1].first - v[i].first)*(v[j+1].first - v[j].first) + (v[i+1].second - v[i].second)*(v[j+1].second - v[j].second);

			// Skip check if current segments are not collinear and their dot product is less or equal than 0
			if((areCollinear == 0 && dotProduct <= 0)){flag = 0;}

			// Check if there is an intersection
			if(flag){
				if(checkIntersection(v[i],v[i+1],v[j],v[j+1])){
					pair<int,int> toAdd (i+1,j);
					intersections.push_back(toAdd);
				}
			}
		}
	}
	return intersections;
}


vector<vector<pair<int,int>>> twoOptNeighbours(vector<pair<int,int>> v){

	// Return the 2-exchange (or 2-opt) neighbours of v

	vector<vector<pair<int,int>>> neighbours;
	vector<pair<int,int>> intersections = getIntersections(v);
	for(auto intr: intersections){
		vector<pair<int,int>> toAdd = v;
		int start = intr.first;
		int end = intr.second;
		while(start < end){
			pair<int,int> tmp = toAdd[start];
			toAdd[start] = toAdd[end];
			toAdd[end] = tmp;
			start++;
			end--;
		}
		neighbours.push_back(toAdd);
	}
	return neighbours;
}


// - Hill Climbing

double perimeter(vector<pair<int,int>> v){
	// Returns the perimeter of candidate tour v
	double ret = 0;
	v.push_back(v[0]);
	for(unsigned long i = 0; i<v.size()-1; i++){
		ret += sqrt((abs(v[i+1].first-v[i].first))*(abs(v[i+1].first-v[i].first)) + (abs(v[i+1].second-v[i].second))*(abs(v[i+1].second-v[i].second)));
	}
	return ret;
}


vector<pair<int,int>> hillClimbing(vector<pair<int,int>> v ,int opt){

	// Run hill climbing algorithm with v as it's starting state according to specified condition
	vector<pair<int,int>> curCandidate = v;
	vector<vector<pair<int,int>>> neighbours = twoOptNeighbours(curCandidate);


	double bestPerimeter = perimeter(curCandidate);
	unsigned long leastConflicts = neighbours.size();
	switch(opt){
		case 1:

			// Best-improvement first

			while(neighbours.size() != 0){
				for(auto n: neighbours){
					double curPerimeter = perimeter(n);
					if(curPerimeter < bestPerimeter){
						bestPerimeter = curPerimeter;
						curCandidate = n;
					}
				}
				neighbours = twoOptNeighbours(curCandidate);
			}
			return curCandidate;

		case 2:

			// First improvement

			while(neighbours.size() != 0){
				curCandidate = neighbours[0];
				neighbours = twoOptNeighbours(curCandidate);
			}
			return curCandidate;


		case 3:

			// Least intersections

			while(leastConflicts != 0){
				int localMaximum = 1;
				for(auto n: neighbours){
					unsigned long curConflicts = getIntersections(n).size();
					if(curConflicts < leastConflicts){
						leastConflicts = curConflicts;
						curCandidate = n;
						localMaximum = 0;
					}
				}
				// Return current candidate if hill climbing starts converging on a local maximum
				if(localMaximum){ puts("Converging on local maxium"); return curCandidate; }
				neighbours = twoOptNeighbours(curCandidate);
			}
			return curCandidate;

		case 4:

			// Random choice

			while(neighbours.size() != 0){
				random_device rd; mt19937 gen(rd());
				uniform_int_distribution<> randIndex(0,(int)neighbours.size()-1);
				curCandidate = neighbours[randIndex(rd)];
				neighbours = twoOptNeighbours(curCandidate);
			}
			return curCandidate;

		default:
			puts("Invalid option");
	}
	return curCandidate;

}


// - Simulated Annealing

vector<pair<int,int>> simulatedAnnealing(vector<pair<int,int>> v, int n){

	random_device rd; mt19937 gen(rd());
	uniform_real_distribution<double> randPercentage(0,1);

	vector<pair<int,int>> curCandidate = v;
	double T = 500;
	while(T > 2.56){
		for(int i = 0; i<n*(n-1); i++){
			vector<vector<pair<int,int>>> curNeighbours = twoOptNeighbours(curCandidate);

			// If current candidate has no neighbours then intended solution as been reached and thus can be returned immediately
			if(curNeighbours.size() == 0){ return curCandidate; }

			// Uniform random choice for selecting neighbour
			uniform_int_distribution<int> randIndex(0,curNeighbours.size()-1);
			vector<pair<int,int>> nextCandidate = curNeighbours[randIndex(rd)];

			unsigned long curConflicts = getIntersections(curCandidate).size();
			unsigned long nextConflicts = getIntersections(nextCandidate).size();

			// If randomly chosen neighbour has more intersections accept it with probability exp((curConflicts - nextConflicts)/T)
			if( curConflicts < nextConflicts && exp((double)(curConflicts-nextConflicts)/T) >= randPercentage(rd) ){
				curCandidate = nextCandidate;
			}
			else{ curCandidate = nextCandidate; }
		}
		// Update temperature
		T = 0.95 * T;
	}
	return curCandidate;
}


int main(){

	// Ask user for number of nodes (n) and for the maximum absolute values coordinates can take (-m to m)
	int n = 0; int m = 0;
	cout << "Number of nodes > ";
	if(!readInt(&n)){ puts("Invalid option"); exit(1); }
	cout << "Axis limits > ";
	if(!readInt(&m)){ puts("Invalid option"); exit(1); }

	// Vector to store initial state
	vector<pair<int,int>> v;

	// Ask user if they want to input an initial state themselves or if one should be generated for them
	char titleStart[] = "Choosing Initial State";
	string optionsStart[] = {"Input Initial State Manually", "Generate Random Initial State", "Exit"};
	printMenu(titleStart,optionsStart,3,10);
	cout << "> ";
	int optStart; if(!readInt(&optStart)){ puts("Invalid option"); exit(1); }
	switch(optStart){

		case 1:
			for(int i = 0; i<n; i++){
				cout << "x value for point " << i << ": ";
				int x; if(!readInt(&x)){ puts("Invalid option"); exit(1); }
				if(-m > x || x > m){ puts("Invalid input"); exit(1); }
				cout << "y value for point " << i << ": ";
				int y; if(!readInt(&y)){ puts("Invalid option"); exit(1); }
				if(-m > y || y > m){ puts("Invalid input"); exit(1); }
				pair<int,int> toAdd (x,y);
				v.push_back(toAdd);
			}
			break;

		case 2:
			v = genRandomPoints(n,m);
			break;
		case 3:
			puts("Exiting..."); exit(0);

		default:
			puts("Invalid option"); exit(1);
	}


	// Mersenne Twister for selecting a random starting index for nearest neighbout heuristic
	random_device rd; mt19937 gen(rd());
	uniform_int_distribution<> randIndex(0,v.size()-1);

	// Vector to store the 2-exchange neighbours of v in case such option is selected
	vector<vector<pair<int,int>>> neighbours;

	cout << "Initial State: " << endl;
	printPairVector(v);

	// Title and options for main menu
	char titleMain[] = "Travelling Salesman Problem";
	string optionsMain[] = {"Random Permutation","Nearest Neighbour","2-Exchange Neighbours","Hill Climbing","Simulated Annealing","Exit"};

	// Title and options for hill climbing menu
	char titleHC[] = "Hill Climbing";
	string optionsHC[] = {"Best-Improvement","First-Improvement","Least Intersections","Random Neighbour","Exit"};

	// Pair vector to hold new tour created by the option selected by the user
	vector<pair<int,int>> curTour;

	// Main menu switch case
	printMenu(titleMain,optionsMain,5,10);
	cout << "> ";
	int optMain; if(!readInt(&optMain)){ puts("Invalid Option"); exit(1); }
	switch(optMain){

		case 1:
			// Random Permutation
			curTour = v;
			pairShuffle(&curTour);
			cout << "Result: ";
			printPairVector(curTour);
			saveTour(curTour);
			break;

		case 2:
			// Nearest Neighbour
			curTour = nearestNeighbourFirst(v,randIndex(rd));
			cout << "Result: ";
			printPairVector(curTour);
			saveTour(curTour);
			break;

		case 3:
			// 2-Exchange Neighbours
			neighbours = twoOptNeighbours(v);
			for(unsigned long i; i< neighbours.size(); i++){
				cout << "Neighbour " << i+1 << ": " << endl;
				printPairVector(neighbours[i]);
				cout << '\n';
			}
			break;

		case 4:
			// Hill Climbing switch case
			printMenu(titleHC,optionsHC,5,10);
			cout << "> ";
			int optHC; if(!readInt(&optHC)){ puts("Invalid Input"); exit(1); }
			switch(optHC){

				case 1:
					curTour = hillClimbing(v,1);
					cout << "Result: ";
					printPairVector(curTour);
					saveTour(curTour);
					break;

				case 2:
					curTour = hillClimbing(v,2);
					cout << "Result: ";
					printPairVector(curTour);
					saveTour(curTour);
					break;

				case 3:
					curTour = hillClimbing(v,3);
					cout << "Result: ";
					printPairVector(curTour);
					saveTour(curTour);
					break;

				case 4:
					curTour = hillClimbing(v,4);
					cout << "Result: ";
					printPairVector(curTour);
					saveTour(curTour);
					break;

				case 5:
					puts("Exiting");
					exit(0);

				default:
					puts("Invalid option");
					exit(1);
			}
			break;

		case 5:
			// Simulated Annealing
			curTour = simulatedAnnealing(v,n);
			cout << "Result: ";
			printPairVector(curTour);
			saveTour(curTour);
			break;

		case 6:
			// Exit
			puts("Exiting...");
			exit(0);
			break;

		default:
			puts("Invalid option");

	}
	return 0;
}
