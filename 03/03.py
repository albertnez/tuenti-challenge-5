#!/usr/bin/env python 
"""
Similar as before, we will precompute each number, and save the result
in an accumulated sum list
"""
import fileinput

MAXN = 30000
primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]

def precalc():
    accumulated = [[0 for i in range(100)] for j in range(MAXN)]

    ind = 0
    with open('numbers.txt','r') as f:
        for line in f:
            # accumulated sum
            if ind > 0:
                for p in primes:
                    accumulated[ind][p] += accumulated[ind-1][p]

            # calc actual line
            num = int(line)
            for p in primes:
                while ((num % p) == 0):
                    num //= p
                    accumulated[ind][p] += 1
            ind += 1
                
    f.closed
    return accumulated


def main():
    accumulated = precalc()

    isFirst = False
    for line in fileinput.input():
        if isFirst is False:
            isFirst = True
            continue
        indFrom = int(line.split()[0])
        indTo = int(line.split()[1])-1

        occurs = list(accumulated[indTo])
        if indFrom > 0:
            for p in primes:
                occurs[p] -= accumulated[indFrom-1][p]

        ans = max(occurs)
        nums = [x for x in filter(lambda y: occurs[y] == ans, primes)]
        print('{0} {1}'.format(ans, ' '.join(map(str,nums))))


if __name__ == "__main__":
    main()
