/*
 * A classic 2D matrix dynamic programming. We can precompute the accumulated
 * sum of each cell, that is, matrix[y][x] has de sum of all cells Cij such
 * that i <= y and j <= x. Then for each position, we can compute the desired
 * sequares in O(1). As the problem is not to big, given each test case, we can
 * simply search for the best position one by one, trying all possible points
 * and calculating the sum in O(1).
 */
#include <iostream>
#include <vector>
#include <fstream>

const std::string FILENAME = "sheet.data";

void precalc(std::vector<std::vector<unsigned long long>>& matrix) {
  std::ifstream file(FILENAME);
  if (file.is_open()) {
    unsigned n, m;
    file >> n >> m;
    matrix = std::vector<std::vector<unsigned long long>>(n, 
        std::vector<unsigned long long>(m, 0ULL));
    for (unsigned i = 0; i < n; ++i) {
      for (unsigned j = 0; j < m; ++j) {
        file >> matrix[i][j];
        if (i) matrix[i][j] += matrix[i-1][j];
        if (j) matrix[i][j] += matrix[i][j-1];
        if (i && j) matrix[i][j] -= matrix[i-1][j-1];
      }
    }
    file.close();
  }
  else {
    exit(1);
  }
}

int main() {
  std::vector<std::vector<unsigned long long>> matrix;
  precalc(matrix);

  int numCases;
  std::cin >> numCases;
  for (int ncase = 1; ncase <= numCases; ++ncase) {
    unsigned y0, x0, y1, x1, k;
    std::cin >> y0 >> x0 >> y1 >> x1 >> k;
    unsigned long long answer = 0;
    for (unsigned i = y0+k; i <= y1-k; ++i) {
      for (unsigned j = x0+k; j <= x1-k; ++j) {
        // top left
        unsigned long long current = matrix[i-1][j-1];
        if (i-k && j-k) current += matrix[i-k-1][j-k-1];
        if (i-k) current -= matrix[i-k-1][j-1];
        if (j-k) current -= matrix[i-1][j-k-1];

        // bottom right
        current += matrix[i+k][j+k];
        current += matrix[i][j];
        current -= matrix[i][j+k] + matrix[i+k][j];

        answer = std::max(answer, current);
      }
    }
    std::cout << "Case " << ncase << ": " << answer << std::endl;
  }
}
