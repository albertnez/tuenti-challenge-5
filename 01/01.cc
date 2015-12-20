/*
 * Easy problem. Solution is (n+1)/2. Just be aware of overflows
 */
#include <iostream>

int main() {
  int numCases;
  std::cin >> numCases;
  while (numCases--) {
    unsigned int n;
    std::cin >> n;
    std::cout << n/2 + (n&1) << std::endl;
  }
}

