"""
For this problem, we had to comunicate via TCP with a server.
From a 16 byte string, we had to give a 24 byte string whose
sha1 had the last byte to FF. This can be done with bruteforce.

Then, to decript the message, as it is done with AES CFB, each byte
results in exactly another byte in same position, and each byte does not 
depend on previous bytes. So we can start by guessing the first byte 
and comparing with the encrypted one. Then, with the next one and so on.

This script reads the encrypted message in base 64 and writes the original
message in 'out.txt'
"""

import hashlib
import itertools
import string
import sys
import telnetlib
import base64


HOST = '54.83.207.93'
PORT = '12345'
# Assuming these are the only chars in the message, else we could change this
SEARCH_CHARS = ' ' + string.ascii_letters + '._-+=/'


def get_sha1_string(prefix):
    """ Returns a string with given prefix whose sha1's last byte is FF """
    for s in itertools.product(string.ascii_letters + string.digits, repeat = 8):
        suffix = ''.join(s)
        message = prefix + suffix
        h = hashlib.new('sha1')
        h.update(message)
        if h.digest()[-1:] == '\xff':
            return message
    return base


def encrypt_message(message):
    """ Given a message, returns the encrypted message by server at HOST """
    telnet = telnetlib.Telnet(HOST, PORT)
    # Read the first message which ends with '1:' 
    recv = telnet.read_until('1: ')[28:44]
    sha1_string = get_sha1_string(recv)
    telnet.write(sha1_string)
    recv = telnet.read_until('encrypt:')
    telnet.write(message)
    recv = telnet.read_until('==')  # read until == or end if no padding required
    return base64.b64decode(recv)


def main(b64_encrypted_key):
    encrypted_key = base64.b64decode(b64_encrypted_key)
    # Original message can already have some text, and this will continue
    # from current character
    original_message = ''
    for i in range(len(original_message), len(encrypted_key)):
        print('Finding letter {0}...'.format(i))
        found = False
        for c in SEARCH_CHARS:
            my_encrypted = encrypt_message(original_message + c)
            if (my_encrypted[i] == encrypted_key[i]):
                original_message += c
                print ("{0}-th character found: '{1}'".format(i, c))
                print ("Current message: {0}".format(original_message))
                found = True
                break
        if found is False:
            print('ERROR, letter {0} not found!'.format(i))
    print('Done. Original message found: {0}'.format(original_message))
    # Write result to file
    with open('out.txt', 'w') as f:
        f.write(original_message)
        f.close()


if __name__ == "__main__":
    """ Give the encrypted message either by stdin, or as an argument """
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        b64_encrypted_key = sys.stdin.readline().strip()
        main(b64_encrypted_key)
