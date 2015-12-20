/*
 * We must find how many numbers are in a interval such that they have two 
 * prime factors. We can make a precompute similar to sieve of eratosthenes,
 * saving one prime factor per each number. Then compute for each number from
 * 1 to 1e8 which ones are 'almost prime' and save that in an accumulated
 * vector. Then each query can be answered in O(1).
 */
#include <iostream>
#include <vector>

const int MAXN = 100000000;

void precalc(std::vector<int>& accumulated) {
  // calc prime vector
  std::vector<int> sieve(MAXN+1, 0);
  for (int i = 1; i <= MAXN; ++i) sieve[i] = i;
  for (int i = 2; i*i <= MAXN; ++i) {
    if (sieve[i] == i) {
      for (int j = i*i; j <= MAXN; j += i) {
        sieve[j] = i;
      }
    }
  }

  // calc accumulated vector
  accumulated = std::vector<int>(MAXN+1, 0);
  for (int i = 2; i <= MAXN; ++i) {
    accumulated[i] += accumulated[i-1];
    if (sieve[i] != i && sieve[i/sieve[i]] == i/sieve[i]) {
      ++accumulated[i];
    }
  }
}

int main() {
  std::vector<int> accumulated;
  precalc(accumulated);
  int T;
  std::cin >> T;
  while (T--) {
    int from, to;
    std::cin >> from >> to;
    int ans = accumulated[to];
    if (from > 0) ans -= accumulated[from-1];
    std::cout << ans << std::endl;
  }
}
