/*
 * The idea is to run a dijkstra but taking care of nodes visited,
 * storing them in a bitmask to avoid repetitions. 
 * The state will be: (Money, bitmask with nodes visted, actual position, turn).
 * Most of data can be precomputed, such as gold you get substracted from other
 * pirates for each node and turn, etc.
 *
 *
 * Some parts of this code have been fixed: First the max number of turns, as
 * when I first solved this problem, the statement said N <= 100, but then it
 * turned out to be 125, thus it must be increased. The other one, is when
 * doing pillage, it was a bug that caused the wrong answer. 
 * Once fixed (two lines) the code gave correct answer. Corrected code is in
 * 05_fixed.cc
 */
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>

typedef std::pair<unsigned, int> Edge;
typedef std::vector<std::vector<Edge>> Graph;
// Our state is (money, bm with nodes visited, node, turn)
typedef std::tuple<int, __int128, unsigned, unsigned> State;

// Assuming at least one pirate gets to Raftel. 
// Otherwirse the answer would be infinite money;
const unsigned MAX_TURNS = 105;
const std::string FINAL_POSITION_NAME = "Raftel";

// READ FUNCTIONS
// Reads the nodes, and returns the id of the final position
unsigned readNodes(std::map<std::string,unsigned>& nodenameToId, 
    std::vector<int>& nodeCost) {
  unsigned n;
  std::cin >> n;
  unsigned currentId = 0;
  nodeCost.resize(n, 0);

  unsigned finalPosition = 0;
  for (unsigned i = 0; i < n; ++i) {
    std::string name;
    int cost;
    std::cin >> name >> cost;
    nodenameToId[name] = currentId;
    nodeCost[currentId] = cost;
    if (name == FINAL_POSITION_NAME) {
      finalPosition = i;
    }
    ++currentId;
  }
  return finalPosition;
}

void readRoutes(std::map<std::string,unsigned>& nodenameToId, Graph& graph) {
  unsigned numRoutes;
  std::cin >> numRoutes;
  for (unsigned i = 0; i < numRoutes; ++i) {
    std::string from, to;
    int cost;
    std::cin >> from >> to >> cost;
    unsigned fromId = nodenameToId[from], toId = nodenameToId[to];
    graph[fromId].push_back(std::make_pair(toId, cost));
  }
}

void readShips(std::vector<unsigned>& initialGold, 
    std::vector<unsigned>& initialPosition,
    std::map<std::string,unsigned> nodenameToId) {
  unsigned numShips;
  std::cin >> numShips;
  initialGold.resize(numShips);
  initialPosition.resize(numShips);
  for (unsigned i = 0; i < numShips; ++i) {
    int dummy;
    std::string nodeOrigin;
    // Assuming the ships are given in order and from 1 to N
    std::cin >> dummy >> nodeOrigin >> initialGold[i] >> nodeOrigin;
    initialPosition[i] = nodenameToId[nodeOrigin];
  }
}
// END READ FUNCTIONS

// Calculates the gold substracted each turn at each position, and the 
// minimum turn for a pirate to get to target position
std::vector<std::vector<unsigned>> calculateGoldSubstracted(
    const std::vector<unsigned>& initialPosition,
    const std::vector<unsigned>& initialGold,
    const Graph& graph,
    const unsigned finalPosition,
    unsigned& turnToGoal) {
  unsigned numShips = initialPosition.size();
  unsigned numPositions = graph.size();

  // Calculate for each node, the next node with minumimum and maximum cost
  std::vector<unsigned> nextMin(numPositions);
  std::vector<unsigned> nextMax(numPositions);
  for (unsigned node = 0; node < numPositions; ++node) {
    nextMin[node] = nextMax[node] = node; // by default, stuck at same node
    if (graph[node].size() > 0) {
      int maxCost, minCost;
      maxCost = minCost = graph[node][0].second;
      nextMin[node] = nextMax[node] = graph[node][0].first;
      for (const Edge& edge : graph[node]) {
        unsigned to = edge.first;
        int cost = edge.second;
        if (cost < minCost) nextMin[node] = to;
        if (cost > maxCost) nextMax[node] = to;
        minCost = std::min(minCost, cost);
        maxCost = std::max(maxCost, cost);
      }
    }
  }

  std::vector<std::vector<unsigned>> shipsPosition(
      numShips, 
      std::vector<unsigned>(MAX_TURNS));
  std::vector<std::vector<unsigned>> goldSubstracted(
    MAX_TURNS,
    std::vector<unsigned>(numPositions, 0));
  turnToGoal = MAX_TURNS;
  // for each ship excluding ourselves
  for (unsigned ship = 1; ship < numShips; ++ship) {
    unsigned currentPosition = initialPosition[ship];
    for (unsigned turn = 0; turn < MAX_TURNS; ++turn) {
      shipsPosition[ship][turn] = currentPosition;
      // only substract if the other pirate arrives
      if (currentPosition != finalPosition && turn > 0 && currentPosition != shipsPosition[ship][turn-1]) {
        goldSubstracted[turn][currentPosition] += initialGold[ship];
      }
      if (currentPosition == finalPosition) {
        turnToGoal = std::min(turnToGoal, turn);
      }
      else {
        // this is even, as we substracted 1, so go to max
        if (ship&1) currentPosition = nextMax[currentPosition];
        else currentPosition = nextMin[currentPosition];
      }
    }
  }
  return goldSubstracted;
}

int computeSolution(const std::vector<unsigned>& initialGold,
    const std::vector<unsigned>& initialPosition,
    const Graph& graph,
    const unsigned finalPosition,
    const unsigned turnToGoal,
    const std::vector<int>& nodeCost,
    const std::vector<std::vector<unsigned>>& goldSubstracted) {
  // Distance state: (bitmask, position, turn)
  typedef std::tuple<__int128, unsigned, unsigned> distanceState;

  __int128 u = 1;
  std::map<distanceState, int> distance;
  {
    distanceState initDistanceState(u<<initialPosition[0], initialPosition[0], 0);
  }
  std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
  {
    State initialState(initialGold[0], u<<initialPosition[0], initialPosition[0], 0);
    pq.push(initialState);
  }
  int bestGold = -1000000000;
  while (!pq.empty()) {
    State currentState = pq.top(); pq.pop();
    int currentGold;
    unsigned currentPosition, currentTurn;
    __int128 bitmask;
    std::tie(currentGold, bitmask, currentPosition, currentTurn) = currentState;

    if (currentTurn > turnToGoal) continue; // Another ship already arrived

    distanceState currentDistanceState(bitmask, currentPosition, currentTurn);
    auto currentIt = distance.find(currentDistanceState);
    if (currentGold < currentIt->second) continue; // There exists a better state
    if (currentPosition == finalPosition) {
      bestGold = std::max(bestGold, currentGold);
    } 

    // First, try to stay at same place
    {
      int newGold = currentGold + 10;
      // sustract all gold from other pirates;
      newGold = std::max(0, newGold - int(goldSubstracted[currentTurn+1][currentPosition]));
      distanceState newDistanceState(bitmask, currentPosition, currentTurn+1);
      // If improved distance
      if (currentIt->second < newGold) {
        currentIt->second = newGold;
        State newState(newGold, bitmask, currentPosition, currentTurn+1);
        pq.push(newState);
      }
    }
    // Now, try navigating to other places
    if (currentGold > 0) {
      for (const Edge& edge : graph[currentPosition]) {
        int cost = edge.second;
        unsigned to = edge.first;
        if ((bitmask & (u<<to)) == 0) { // if node not visited yet
          __int128 newBitmask = bitmask | (u<<to);
          //int newGold = std::max(0, std::max(0, currentGold - cost) - nodeCost[to]);
          int newGold = std::max(0, currentGold - cost - nodeCost[to]);
          // substract gold from other pirates
          if (to != finalPosition) newGold = std::max(0, newGold - int(goldSubstracted[currentTurn+1][to]));
          distanceState newDistanceState(newBitmask, to, currentTurn+1);
          auto it = distance.find(newDistanceState);
          // If improved distance
          if (it == distance.end() || it->second < newGold) {
            if (it == distance.end()) distance.insert(std::make_pair(newDistanceState, newGold));
            else it->second = newGold;
            State newState(newGold, newBitmask, to, currentTurn+1);
            pq.push(newState);
          }
        }
      }
    }
  }
  return bestGold;
}

int main() {
  std::map<std::string, unsigned> nodenameToId;
  std::vector<int> nodeCost;
  unsigned finalPosition = readNodes(nodenameToId, nodeCost);

  Graph graph(nodeCost.size());
  readRoutes(nodenameToId, graph);

  std::vector<unsigned> initialGold;
  std::vector<unsigned> initialPosition;
  readShips(initialGold, initialPosition, nodenameToId);

  unsigned turnToGoal;
  std::vector<std::vector<unsigned>> goldSubstracted = calculateGoldSubstracted(
      initialPosition,
      initialGold,
      graph,
      finalPosition,
      turnToGoal);

  int answer = computeSolution(initialGold, 
      initialPosition, 
      graph, 
      finalPosition, 
      turnToGoal,
      nodeCost, 
      goldSubstracted);

  std::cout << answer << std::endl;
}
