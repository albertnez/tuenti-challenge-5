#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>


/*
 * To avoid dealing with strings, we will have
 * basic_element: [1, 10000], having:
 * healing_potion = 9998
 * dragon_scale = 9999
 * steel_shield = 10000
 * And for compound_element: [10001, 110000]
 * having the ids with an offset of 10000, and
 * phoenix_potion = 109998
 * dragon_shield = 109999
 * phoenix_dragon_shield = 110000
 *
 * We will calculate the answer with a DFS, as the max depth will be 20,
 * as each time we combine there is one item less, and the search space 
 * will be small enough to run quite fast.
 */

std::map<std::string,unsigned> mapNameToId {
    {"healing_potion", 9998},
    {"dragon_scale", 9999},
    {"steel_shield", 10000},
    
    {"phoenix_potion", 99998},
    {"dragon_shield", 99999},
    {"phoenix_dragon_shield", 100000},
};

const unsigned OFFSET = 10000;
const unsigned NUM_IDS = 100001;
const unsigned MAX_CAP = 20;

unsigned nameToId(const std::string& name) {
  if (name[0] == 'b') return unsigned(stoi(name.substr(14)));
  if (name[0] == 'c') return unsigned(stoi(name.substr(17))) + OFFSET;
  return mapNameToId[name];
}

void readData(std::vector<std::vector<unsigned>>& craftData, 
    std::vector<unsigned>& valueData) {
  valueData.resize(NUM_IDS);
  craftData.resize(NUM_IDS - OFFSET);
  std::ifstream file("book.data");
  if (file.is_open()) {
    std::string line;
    while (getline(file, line)) {
      std::istringstream tokens(line);
      std::string token;
      unsigned id;
      // First, name and value
      tokens >> token;
      id = nameToId(token);
      tokens >> valueData[id];
      // Now required to craft
      id = id - OFFSET;
      while (tokens >> token) {
        unsigned newId = nameToId(token);
        craftData[id].push_back(newId);
      }
    }
  }
  else {
    std::cerr << "Error reading book.data file" << std::endl;
    exit(1);
  }
}
    
unsigned calculateValue(const std::vector<std::vector<unsigned>>& takenIds,
    unsigned numItems,
    const std::vector<unsigned>& valueData) {
  unsigned sum = 0;
  for (unsigned i = 0; i < numItems; ++i) {
    sum += valueData[takenIds[numItems][i]];
  }
  return sum;
}


// DFS to fins the answer
void calculateAnswer(std::vector<std::vector<unsigned>>& takenIds,
    unsigned numItems,
    unsigned& maxValue,
    const std::vector<std::vector<unsigned>>& craftData, 
    const std::vector<unsigned>& valueData) {

  unsigned currentValue = calculateValue(takenIds, numItems, valueData);
  maxValue = std::max(maxValue, currentValue);
  
  for (unsigned id = 1; id < NUM_IDS - OFFSET; ++id) {
    unsigned bmUsed = 0;
    bool possible = true;
    for (unsigned requiredItem : craftData[id]) {
      bool satisfied = false;
      for (unsigned j = 0; !satisfied && j < numItems; ++j) {
        if ((bmUsed & (1<<j)) == 0 && takenIds[numItems][j] == requiredItem) {
          satisfied = true;
          bmUsed |= (1<<j);
        }
      }
      possible &= satisfied;
    }

    // If we can take the object
    if (possible) {
      unsigned newNumItems = numItems + 1 - craftData[id].size();
      unsigned ind = 0;
      for (unsigned i = 0; i < numItems; ++i) {
        if (((1<<i) & bmUsed) == 0) {
          takenIds[newNumItems][ind++] = takenIds[numItems][i];
        }
      }
      // Add the new object
      takenIds[newNumItems][ind++] = id + OFFSET;
      calculateAnswer(takenIds, newNumItems, maxValue, craftData, valueData);
    }
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  std::vector<std::vector<unsigned>> craftData;
  std::vector<unsigned> valueData;

  readData(craftData, valueData);

  unsigned numCases;
  std::cin >> numCases;
  std::cin.ignore();
  while (numCases--) {
    std::string line;
    getline(std::cin, line);
    std::istringstream tokens(line);
    std::string token;
    std::vector<unsigned> initialIds;
    std::vector<std::vector<unsigned>> takenIds(MAX_CAP+1,
        std::vector<unsigned>(MAX_CAP, 0));
    std::vector<unsigned> tmpIds;
    while (tokens >> token) {
      tmpIds.push_back(nameToId(token));
    }
    unsigned ind = 0;
    for (unsigned id : tmpIds) {
      takenIds[tmpIds.size()][ind++] = id;
    }

    unsigned maxValue = 0;
    calculateAnswer(takenIds, ind, maxValue, craftData, valueData);
    std::cout << maxValue << std::endl;
  }
}
