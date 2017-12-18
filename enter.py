from __future__ import print_function

import numpy as np
import cv2 as cv

# built-in modules
import os
import sys
import glob
import argparse
import App as AppInstance

from math import *

if __name__ == '__main__':
    print(__doc__)

    parser = argparse.ArgumentParser(description='Demonstrate mouse interaction with images')
    parser.add_argument("-i","--input", default='../data/', help="Input directory.")
    args = parser.parse_args()
    path = args.input

    main = AppInstance.Application()

    main.init()
    result = 0
    while result == 0:
        result = main.update()

    


