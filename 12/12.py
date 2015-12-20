#!/usr/bin/env python
"""
In this problem, you log into a terminal as an operator. In the javascript 
console, you can get the username and the password.
Looking at the files in the index.html and http://54.83.207.93:8080/js/forth.js
we can see that it runs a small interpreter of Forth.
We can see some details as how parameters are passed, the stdlib given etc.

The access code is the sum of the letters of the operator in ascii value
(this is given by the interpreter reading args) and the password. Then XORed 
with the magic number.

When taking the photo, the "PROCESS" function just multiplies two numbers given,
and the process of taking a photo, takes the arg, the date and value 3000, and
what it does is: pop the last element. multiply it by 3. Add it to the now
last element, until we only have one number.

Given the operator, the password, the access code, the date and the processed 
number K in "PROCESS" function when taking the photo, we get the imatge at:
http://54.83.207.93:14333/operatorXXX_PASSWD_ACCESSCODE_YEAR_MONTH_DAY_HOUR_MIN_SECONDS_K

If we change any value, we get different images. The original image is:
http://bit.ly/1FCjhxq

The, we can see that there is a squared shadow in the noisy image. Suspecting 
that it could be a Qr code, we can donwload many images (100 were enough),
and then, denoise it just by taking the average color of each pixel.

After this, the Qr code has the text containing the password.

(denoise.py script in other file)

"""
import urllib


MAGIC = 574381
BASE_URL = 'http://54.83.207.93:14333/'
PROCESS_CONSTANT = 30000
IMAGES_FOLDER = 'images'
NUM_IMAGES = 100


def get_access_code(operator, passwd):
    """ Returns the access code of given operator and password """
    return MAGIC ^ sum([ord(c) for c in operator]) + passwd


def process_number(access_code, date_array):
    """ Returns the number by the process of taking a photo given the elements
    in the stack
    """
    array = [access_code]
    array.extend(date_array)
    array.append(PROCESS_CONSTANT)
    while len(array) > 1:
        x = array.pop();
        array[-1] = array[-1]+x*3;
    return array[0]


def get_image_url(operator, passwd, access_code, processed_num, date_array):
    """ Returns the url of the image """
    elems = [operator, str(passwd), str(access_code)]
    elems.extend([str(x) for x in date_array[::-1]])
    elems.append(str(processed_num))
    return BASE_URL + '_'.join(elems)


def download_image_url(url, filename):
    """ Download an image with the given url to filename """
    urllib.urlretrieve(url, filename)


def main():
    # For each operator
    for i in range(1, NUM_IMAGES+1):
        operator = 'operator{0:03d}'.format(i)
        print('downloading {0}...'.format(operator))
        passwd = 1000000  # Any password will fit
        ac = get_access_code(operator, passwd)
        date_array = [0,0,0,0,0,0]  # Any data
        processed_num = process_number(ac, date_array)
        url = get_image_url(operator, passwd, ac, processed_num, date_array)
        download_image_url(url, IMAGES_FOLDER + '/op{0:03}'.format(i))


if __name__ == "__main__":
    main()

