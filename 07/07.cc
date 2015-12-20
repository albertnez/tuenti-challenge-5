/*
 * Graph problem involving BFS and UnionDisjoint.
 * Question A: O(n). O(1) for each person
 * Question B: O(n^2). O(n) for each person
 * Question C: O(n^2). Run O(n) BFS from each person to get distances. 
 * Then for each person, for each friend of this person, O(1) to check.
 * Question D: can be calculated in O(n^2).
 * Question E: Here we used a union disjoint set, although we could check if
 * there is no chain when we were doing the BFS from each person. So yes, this
 * is a bit overkill.
 *
 * Algorithm complexity: O(n^2).
 */
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <queue>

typedef std::vector<std::vector<unsigned>> Graph;

enum questionsName {
  None = 0,
  questionA = 1<<0,
  questionB = 1<<1,
  questionC = 1<<2,
  questionD = 1<<3,
  questionE = 1<<4,
};

// Union disjoint class
class UnionDisjoint {
public:
  UnionDisjoint(unsigned n) : n(n) {
    parent.resize(n);
    size.resize(n, 1);
    for (unsigned i = 0; i < n; ++i) {
      parent[i] = i;
    }
  }

  unsigned getParent(unsigned u) {
    if (parent[u] == u) return u;
    return parent[u] = getParent(parent[u]);
  }

  bool sameSet(unsigned u, unsigned v) {
    return getParent(u) == getParent(v);
  }

  void join(unsigned u, unsigned v) {
    if (sameSet(u, v)) return;
    unsigned pu = getParent(u);
    unsigned pv = getParent(v);
    if (size[pu] > pv) {
      parent[pv] = pu;
    }
    else {
      parent[pu] = pv;
    }
    if (size[pu] == size[pv]) {
      size[pu]++;
    }
  }

private:
  unsigned n;
  std::vector<unsigned> parent;
  std::vector<unsigned> size;
};

int main() {
  unsigned n, m;
  std::cin >> n >> m;
  std::vector<int> questions(n, None);
  std::map<std::string,unsigned> nameToId;
  for (unsigned i = 0; i < n; ++i) {
    std::string name;
    std::cin >> name;
    nameToId[name] = i;
    for (unsigned j = 0; j < 5; ++j) {
      char ans;
      std::cin >> ans;
      if (ans == 'Y') questions[i] |= 1<<j;
    }
  }
  // Build the graph
  UnionDisjoint uniondisjoint(n);
  Graph graph(n);
  std::vector<std::vector<bool>> adjMatrix(n, std::vector<bool>(n, false));
  std::cin.ignore(); // jump line
  for (unsigned i = 0; i < m; ++i) {
    std::string line;
    getline(std::cin, line);
    std::istringstream ss(line);
    std::vector<unsigned> ids;
    std::string name;
    while (ss >> name) ids.push_back(nameToId[name]);
    
    for (unsigned i = 0; i < ids.size(); ++i) {
      for (unsigned j = i+1; j < ids.size(); ++j) {
        unsigned u = ids[i], v = ids[j];
        if (adjMatrix[u][v] == 0) {
          graph[u].push_back(v);
          graph[v].push_back(u);
        }
        adjMatrix[u][v] = adjMatrix[v][u] = 1;
        uniondisjoint.join(u, v);
      }
    }
  }

  // Now run bfs all against all
  const unsigned limitedDistance = 2;
  const unsigned infDistance = 100;
  std::vector<std::vector<unsigned>> distance(n, std::vector<unsigned>(n, infDistance));
  for (unsigned i = 0; i < n; ++i) {
    std::queue<unsigned> q;
    distance[i][i] = 0;
    q.push(i);
    while (!q.empty()) {
      unsigned u = q.front();
      q.pop();
      unsigned d = distance[i][u];
      if (d == limitedDistance) continue;
      for (unsigned v : graph[u]) {
        if (d+1 < distance[i][v]) {
          distance[i][v] = d+1;
          q.push(v);
        }
      }
    }
  }

  // If only has one friend who likes cats. If it has two or more, this will
  // be set to 'n', so that further checking will return false, as G can have H
  // as a friend if H's friend who likes cats is G
  std::vector<unsigned> idFriendLikesCats(n);
  std::vector<bool> hasFriendLikesCats(n);
  for (unsigned i = 0; i < n; ++i) {
    for (unsigned j = 0; j < n; ++j) if (i != j) {
      if (adjMatrix[i][j]) {
        if (questions[j] & questionD) {
          if (!hasFriendLikesCats[i]) {
            hasFriendLikesCats[i] = true;
            idFriendLikesCats[i] = j;
          }
          else {
            idFriendLikesCats[i] = n;
          }
        }
      }
    }
  }

  // For each person, calculate
  unsigned long long answer = 0;
  for (unsigned i = 0; i < n; ++i) {
    unsigned long long current = 0;
    // Points for A
    bool questionDCounted = false;
    if (questions[i] & questionA) current += 7;

    for (unsigned j = 0; j < n; ++j) {
      // Points for B
      if (adjMatrix[i][j] && (questions[j] & questionB)) {
        current += 3;
      }
      // Points for C
      if (distance[i][j] == 2 && (questions[j] & questionC)) {
        current += 6;
      }
      // Points for D
      if (adjMatrix[i][j] && questions[j] & questionD && !questionDCounted && 
          idFriendLikesCats[j] == i) {
        questionDCounted = true;
        current += 4;
      }
      // Points for E
      if (!uniondisjoint.sameSet(i, j) && (questions[j] & questionE)) {
        current += 5;
      }

      if (i == 4) { 
      }
    }
    answer = std::max(answer, current);
  }

  std::cout << answer << std::endl;
}
