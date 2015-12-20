#!/usr/bin/env python
"""
Denoise image just by taking the average color of each pixel.
For this problem 100 images were enough to detect de Qr code
"""

from PIL import Image

IMAGES_FOLDER = 'images'
IMAGE_SIZE = 634
NUM_IMAGES = 100


def main():
    result = Image.new('RGB', (IMAGE_SIZE, IMAGE_SIZE))
    result_pixels = result.load()
    new_pixels = [[(0,0,0) for i in range(0, IMAGE_SIZE)] for j in range(0, IMAGE_SIZE)]

    # For each operator
    for n in range(1, NUM_IMAGES+1):
        filename = IMAGES_FOLDER + '/op{0:03d}'.format(n)
        print('processing image {0:03d}'.format(n))
        image = Image.open(filename)
        pixels = image.load()


        # Itearate over pixels
        for i in range(0, IMAGE_SIZE):
            for j in range(0, IMAGE_SIZE):
                new_pixels[i][j] = tuple(map(sum,zip(pixels[i,j],new_pixels[i][j])))

    # Divide result by number of images
    for i in range(0, IMAGE_SIZE):
        for j in range(0, IMAGE_SIZE):
            result_pixels[i,j] = tuple(map(lambda x: int(x/NUM_IMAGES),list(new_pixels[i][j])))

    # Save image
    result.save('output.jpeg')


if __name__ == "__main__":
    main()

