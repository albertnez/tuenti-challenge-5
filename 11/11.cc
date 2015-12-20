/*
 * The idea is that if we model a graph from the rooms we visit, we obtain a 
 * DAG, and therefore we can use dynamic programming, where the state is:
 * (in which room we are, how much stamina do we have). When we take different
 * paths, those paths will never generate the same set, as we must kill at least
 * one minion in each room, therefore, we calculate for each door, the number
 * of ways to kill those minion, multiplied by the number of ways in the next
 * room and so on. When we exit, then return 1. We could have a problem of 
 * counting one set many times if we had to kill all minions in a room in 
 * many doors, and more of one of them lead to an exit, but this cannot happen by
 * definition of problem.
 */
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

const std::string FILENAME = "scenarios.txt";
const unsigned MOD = 1e9+7;
const unsigned MAX_DOORS = 1000;

struct Edge {
  unsigned v, keys;
  int stamina;
  Edge(unsigned v, unsigned keys, int stamina)
      : v(v), keys(keys), stamina(stamina) {}
  Edge() {}
};

typedef std::vector<std::vector<Edge>> Graph;

// Get the id of the string, if it does not exist, assign the next id to it
unsigned stringToId(const std::string& string, 
    std::map<std::string,unsigned>& map,
    unsigned& id) {
  auto it = map.find(string);
  if (it != map.end()) return it->second;
  return map[string] = id++;
}

// Class to solve the problem
class Solver {
public:
  Solver();
  int solveScenario(unsigned scenario);
private:
  void getScenerarioPositions();
  void calculateBinomial();
  int calculateDp(unsigned room, int stamina);
  // Global
  std::vector<int> scenarioPositions;
  std::vector<std::vector<unsigned>> binomial;

  // For each scenario
  unsigned numRooms;
  int maxStamina;
  Graph graph; // Graph for each scenario
  std::vector<std::vector<int>> memo; // Our memo table
};

// Initialize with precalcs
Solver::Solver() {
  getScenerarioPositions();
  calculateBinomial();
}


// Calculate the position in file of each case
void Solver::getScenerarioPositions() {
  std::ifstream file(FILENAME);
  if (!file.good()) {
    std::cerr << "Error reading 'scenarios.txt'" << std::endl;
    exit(1);
  }
  int numCases;
  file >> numCases;
  scenarioPositions.resize(numCases);
  for (int ncase = 0; ncase < numCases; ++ncase) {
    scenarioPositions[ncase] = file.tellg();

    unsigned rooms;
    file >> rooms >> rooms;
    while (rooms--) {
      std::string name;
      unsigned doors;
      file >> name >> doors;
      while (doors--) {
        unsigned dummy;
        file >> name >> dummy >> dummy;
      }
    }
  }
}

// Calculate binomial coefficients "n choose k"
void Solver::calculateBinomial() {
  binomial = std::vector<std::vector<unsigned>>(MAX_DOORS, std::vector<unsigned>(MAX_DOORS));
  for (unsigned i = 0; i < MAX_DOORS; ++i) {
    for (unsigned j = 0; j <= i; ++j) {
      if (i == 0 || j == 0) binomial[i][j] = 1;
      else {
        binomial[i][j] = binomial[i-1][j];
        if (j) {
          binomial[i][j] += binomial[i-1][j-1];
          if (binomial[i][j] > MOD) binomial[i][j] %= MOD;
        }
      }
    }
  }
}

int Solver::solveScenario(unsigned scenario) {
  // Read the graph
  std::ifstream file(FILENAME);
  file.seekg(scenarioPositions[scenario]);
  if (!file.good()) {
    std::cerr << "Error reading 'scenarios.txt'" << std::endl;
    exit(1);
  }
  file >> maxStamina >> numRooms;
  // Exit will always have id numRooms
  std::map<std::string,unsigned> roomnameToId {{"exit", numRooms}};
  graph = Graph(numRooms);
  unsigned currentId = 0;
  for (unsigned nroom = 0; nroom < numRooms; ++nroom) {
    std::string name;
    unsigned doors;
    file >> name >> doors;
    unsigned roomId = stringToId(name, roomnameToId, currentId);
    for (unsigned i = 0; i < doors; ++i) {
      Edge edge;
      file >> name;
      unsigned destinationId = stringToId(name, roomnameToId, currentId);
      edge.v = destinationId;
      file >> edge.keys >> edge.stamina;
      graph[roomId].push_back(edge);
    }
  }

  // Now calculate the answer
  // Initialize memo to -1
  memo = std::vector<std::vector<int>> (numRooms, std::vector<int>(maxStamina+1, -1));
  return calculateDp(0, maxStamina);
}

int Solver::calculateDp(unsigned room, int stamina) {
  if (room == numRooms) return 1;
  int& ans = memo[room][stamina];
  if (ans != -1) return ans; // Already calculated 
  ans = 0;
  for (Edge edge : graph[room]) { // For each door
    // If enough stamina and keys
    if (std::min(maxStamina, int(stamina + graph[room].size())) >= edge.stamina && 
        edge.keys <= graph[room].size()) {
      // There are n-1 choose k-1 possible sets, as we are enforced to kill
      // the one protecting the door
      int minionsToKill = std::max(1, std::max(std::min(maxStamina,edge.stamina - stamina), int(edge.keys)));
      int newStamina = std::min(stamina + minionsToKill, maxStamina);
      newStamina = std::min(maxStamina, newStamina - edge.stamina);
      int otherKilledMinion = std::max(0, minionsToKill-1);
      ans = ((unsigned long long)(ans) + (unsigned long long)(binomial[graph[room].size()-1][otherKilledMinion]) * 
          (unsigned long long)(calculateDp(edge.v, newStamina)))%MOD;
    }
  }
  return ans;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  Solver solver;
  unsigned scenario;
  while (std::cin >> scenario) {
    std::cout << "Scenario " << scenario << ": " 
              << solver.solveScenario(scenario) << std::endl;
  }
}
