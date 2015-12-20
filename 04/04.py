#!/usr/bin/env python
"""
Just decode the given string, and we will be taking the number of bits
requested, with the format requested.
"""

import base64
import sys


def toLittleEndian(rawBytes):
    n = ''
    while (len(rawBytes) > 8):
        n = rawBytes[:8] + n
        rawBytes = rawBytes[8:]
    n = rawBytes + n
    return n


# Bytes in the interval [fromByte, toByte)
def getBytes(fromByte, toByte, decodedBytes):
    startInd = fromByte>>3
    endInd = toByte>>3
    # starting byte
    n = '{0:08b}'.format(decodedBytes[startInd])
    n = n[fromByte%8:]

    # now add the other bytes
    for i in range(startInd+1, endInd):
        n += '{0:08b}'.format(decodedBytes[i])

    # process the last bytes
    endrem = 8-(toByte%8)

    if startInd != endInd:
        n += '{0:08b}'.format(decodedBytes[endInd])
    # erase extra bits taken
    n = n[:-endrem]

    return n


def main():
    base64String = sys.stdin.readline().strip()
    decodedBytes = base64.b64decode(base64String)
    numPieces = int(sys.stdin.readline())

    accumBits = 0  # Bits already read
    for line in sys.stdin:
        line = line.strip()
        isReverse = (line[-1] == 'R')
        if isReverse:
            line = line[:-1]
        isLittleEndian = (line[-1] == 'L')
        line = line[:-1]
        numBits = int(line)
        
        answer = getBytes(accumBits, accumBits + numBits, decodedBytes)
        if isLittleEndian:
            answer = toLittleEndian(answer)
        if isReverse:
            answer = answer[::-1]
        print('{0}'.format(int(answer, 2)))
        accumBits += numBits


if __name__ == "__main__":
    main()
