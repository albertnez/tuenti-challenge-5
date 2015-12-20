/*
 * In this problem one could precompute some calculations and then obtain a
 * O(N^3) algorithm instead of O(N^4) given. I focused the answer solution the
 * wrong way, thinking in some FFT or things should be done completely different,
 * I could not get with a proper solution, and ended up running O(N^4) algorithm.
 * Then I realized that the expected solution was to precompute parts so that it
 * could run in O(N^3), but this is the answer exactly as I sent it, could not
 * change it.
 */
#include <iostream>
#include <vector>
#include <cmath>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#define THRESCORR 1e-30

std::vector<double> crosscorr(const double* x, int xSize, const double * y, int ySize)
{
  std::vector<double> xcorr;

  //! Calculate the mean of the two series x[], y[]
  double xMean = 0.0;
  for (int i = 0; i < xSize; i++) {
      if (x[i] >= 0.0 && x[i] <= 15.0)
    xMean += x[i] / xSize;
  }

  double yMean = 0.0;
  for (int i = 0; i < ySize; i++) {
      if (y[i] >= 0.0 && y[i] <= 15.0)
    yMean += y[i] / ySize;
  }

  //! Calculate the denominator (product of standard deviations)
  double xSumCuadraticDiff = 0.0;
  for (int i = 0; i < xSize; i++) {
    xSumCuadraticDiff += pow(x[i] - xMean, 2);
  }
  
  double ySumCuadraticDiff = 0.0;
  for (int i = 0; i < ySize; i++) {
    ySumCuadraticDiff += pow(y[i] - yMean, 2);
  }

  double denom = sqrt(xSumCuadraticDiff * ySumCuadraticDiff);
  if (denom < THRESCORR){
    xcorr.resize(0);
    return xcorr;
  }

  //! Calculate the correlation series
  xcorr.resize(ySize - xSize + 1);

  for (int delay = 0; delay < xcorr.size(); delay++) {
    double xySum = 0.0;
    for (int i = 0; i < xSize; i++) {
      xySum += (x[i] - xMean) * (y[i + delay ] - yMean);
    }

    xcorr[delay] = xySum / denom;
  }	
  return xcorr;
}


double findScore(const double* wave, int waveSize, const double* pattern, int patternSize){
  double score = 0.0;
  int minSubvectorLength = 2;

  for (int subvectorStart = 0; subvectorStart <= waveSize - minSubvectorLength; subvectorStart++) {
    for (int subvectorLength = minSubvectorLength; subvectorLength <= MIN(waveSize - subvectorStart, patternSize); subvectorLength++) { 
      std::vector<double> xcorrelation = crosscorr(&(wave[subvectorStart]), subvectorLength, pattern, patternSize);
      for (int xcorrelationIndex = 0; xcorrelationIndex < xcorrelation.size(); xcorrelationIndex++) {
        score = MAX(score, xcorrelation[xcorrelationIndex] * subvectorLength);
      }
    }
  }

  return score;
}


double xcorr(const std::vector<double>& x, unsigned xFrom, unsigned xTo, double xMean, const std::vector<double>& y, unsigned yFrom, unsigned yTo, double yMean, double denom) {
  unsigned ySize = yTo - yFrom;
  unsigned xSize = xTo - xFrom;

  double best = 0.0;
  for (unsigned delay = 0; delay <  (ySize - xSize + 1); ++delay) {
    double xySum = 0.0;
    for (unsigned i = 0; i < xSize; ++i) {
      xySum += (x[xFrom + i] - xMean) * (y[yFrom + i + delay] - yMean);
    }
    best = std::max(best, xySum / denom);
  }
  return best;
}

double calcScore(const std::vector<double>& wave, const std::vector<double>& pattern) {
  const unsigned minSubvectorLength = 2;

  // Calc patternMean
  double patternMean = 0.0;
  for (double d : pattern) {
    patternMean += d;
  }
  patternMean /= double(pattern.size());

  double ySumCuadraticDiff = 0.0;
  for (unsigned d : pattern) {
    ySumCuadraticDiff += (d-patternMean)*(d-patternMean);
  }

  double bestScore = 0.0;

  for (unsigned subvectorStart = 0; subvectorStart <= wave.size() - minSubvectorLength; ++subvectorStart) {
    double waveSum = wave[subvectorStart];
    double xCSum = wave[subvectorStart]*wave[subvectorStart];
    for (unsigned subvectorLength = minSubvectorLength; subvectorLength <= std::min(wave.size() - subvectorStart, pattern.size()); ++subvectorLength) {
      double act = wave[subvectorStart + subvectorLength -1];
      waveSum += act;
      xCSum += act*act;
      double xMean = waveSum / double(subvectorLength);
      double xSumCuadraticDiff = xCSum - 2.0*xMean*(waveSum) + subvectorLength*xMean*xMean;
      double denom = sqrt(xSumCuadraticDiff * ySumCuadraticDiff);

      if (denom >= THRESCORR) {
        bestScore = std::max(bestScore, subvectorLength * xcorr(wave, subvectorStart, subvectorStart + subvectorLength, waveSum / double(subvectorLength), pattern, 0, pattern.size(), patternMean, denom));
      }
    }
  }
  return bestScore;
}


int main() {
  std::cout.precision(4);
  unsigned P, W;
  std::cin >> P >> W;
  std::vector<double> pattern(P);
  for (unsigned i = 0; i < P; ++i) {
    std::cin >> pattern[i];
  }
  std::vector<double> wave(W);
  for (unsigned i = 0; i < W; ++i) {
    std::cin >> wave[i];
  }
  

  
  //std::cout << std::fixed << calcScore(wave, pattern) << std::endl;
  std::cout << std::fixed << findScore(&wave[0], W, &pattern[0], P) << std::endl; 
}
