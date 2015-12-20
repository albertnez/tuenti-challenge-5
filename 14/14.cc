/*
 * The idea is that we can model a directed graph, where there is an edge
 * (u, v) if v is inside the range of u's explosion.
 * Then, we do the topological sort of this graph, and then explode them in
 * this order, as the first ones, or do not have any ship explodim them, thus
 * we choose him and it may explode others, or it is inside a cycle, in that
 * case doesn't matter which one you choose in the cycle.
 *
 * Overall cost: O(V^2 * K), where V is the vertices and K is the max
 * number of vertices of each explosion, to check if each ship is inside it.
 */
#include <iostream>
#include <vector>
#include <cmath>

typedef std::vector<std::vector<unsigned>> Graph;
typedef std::pair<int,int> Vec2;

Vec2 operator-(const Vec2 lhs, const Vec2 rhs) {
  return Vec2(lhs.first-rhs.first, lhs.second-rhs.second);
}

// Cross product. Beware of overflow!
long long crossProduct(const Vec2 lhs, const Vec2 rhs) {
  return (long long)(lhs.first)*(long long)(rhs.second) -
         (long long)(lhs.second)*(long long)(rhs.first);
}

// Aux I/O functions
std::ostream& operator<<(std::ostream& os, const std::vector<unsigned>& v) {
  for (const unsigned x : v) os << " " << x;
  return os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Graph& graph) {
  for (const std::vector<unsigned>& v : graph) {
    os << v;
  }
  return os << std::endl;
}

std::istream& operator>>(std::istream& is, Vec2& point) {
  return is >> point.first >> point.second;
}

std::ostream& operator<<(std::ostream& os, Vec2& point) {
  return os << "(" << point.first << "," << point.second << ")";
}

// Our solver class
class Solver {
public:
  Solver() {
    currentId = 0;
    readData();
    createGraph();
    topoSort();
  }

  unsigned getSolution();

private:
  void readData();
  void createGraph();
  void topoSort();
  void dfs(unsigned u, bool topoOrder = false);

  unsigned numEnemies;
  std::vector<Vec2> positions;
  std::vector<std::vector<Vec2>> explosions;
  Graph graph;
  std::vector<bool> visited;
  unsigned currentId;
  std::vector<unsigned> topologicalOrder;
};


void Solver::readData() {
  std::cin >> numEnemies;
  positions.resize(numEnemies);
  explosions.resize(numEnemies);
  graph.resize(numEnemies);
  visited.resize(numEnemies, false);
  topologicalOrder.resize(numEnemies);
  for (unsigned i = 0; i < numEnemies; ++i) {
    std::cin >> positions[i];
    unsigned numVertices;
    std::cin >> numVertices;
    explosions[i] = std::vector<Vec2>(numVertices);
    for (Vec2& vertex : explosions[i]) {
      std::cin >> vertex;
    }
  }
}

void Solver::createGraph() {
  for (unsigned i = 0; i < numEnemies; ++i) {
    unsigned numVertices = explosions[i].size();
    for (unsigned j = 0; j < numEnemies; ++j) if (i != j) {
      Vec2 point = positions[j];
      bool insideExplosion = true;
      bool firstCheck = true;
      long long sign;
      for (unsigned k = 0; k < numVertices; ++k) {
        Vec2 ab = explosions[i][(k+1)%numVertices] - explosions[i][k];
        Vec2 ap = point - explosions[i][k];
        long long xprod = crossProduct(ab, ap);
        if (firstCheck) {
          firstCheck = false;
          sign = xprod;
        }
        else {
          if (sign * xprod <= 0) {
            insideExplosion = false;
          }
        }
      }
      if (insideExplosion) {
        // Explosion from i destroys j
        graph[i].push_back(j);
      }
    }
  }
}

void Solver::dfs(unsigned u, bool topoOrder) {
  visited[u] = true;
  for (unsigned v : graph[u]) {
    if (!visited[v]) {
      dfs(v, topoOrder);
    }
  }
  if (topoOrder) {
    topologicalOrder[numEnemies - currentId - 1] = u;
    ++currentId;
  }
}

void Solver::topoSort() {
  for (unsigned i = 0; i < numEnemies; ++i) {
    if (!visited[i]) dfs(i, true);
  }
}

unsigned Solver::getSolution() {
  unsigned answer = 0;
  for (unsigned i = 0; i < numEnemies; ++i) visited[i] = false;
  for (unsigned u : topologicalOrder) {
    if (!visited[u]) {
      ++answer;
      dfs(u);
    }
  }
  return answer;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  Solver solver;
  std::cout << solver.getSolution() << std::endl;
}
