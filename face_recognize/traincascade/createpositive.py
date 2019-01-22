#!/usr/bin/env python3
import os
import cv2 as cv


POSTIVE_DIR = 'pos/'
NEGATIVE_DIR = 'neg/'
INFO_FILENAME = 'info.dat'
BG_FILENAME = 'bg.txt'


this_dir = os.path.abspath(os.path.dirname(__file__))
postive_url = os.path.join(this_dir, POSTIVE_DIR)
negative_url = os.path.join(this_dir, NEGATIVE_DIR)


# create info.dat
img_list = os.listdir(postive_url)
with open(os.path.join(this_dir, INFO_FILENAME), 'wb') as file:

    for img_name in img_list:

        img_url = os.path.join(postive_url, img_name)
        img = cv.imread(img_url)
        cols, rows = img.shape[:2]

        file.write( bytes(POSTIVE_DIR + img_name + ' 1 0 0 %s %s\n' % (str(cols), str(rows)),'UTF-8'))


# create bg.txt
img_list = os.listdir(negative_url)
with open(os.path.join(this_dir, BG_FILENAME), 'wb') as file:

    for img_name in img_list:

        file.write( bytes(negative_url + img_name + '\n', 'UTF-8'))

