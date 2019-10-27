#!/usr/bin/env python3

import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")

import time
import select
import socket
import struct
import numpy as np
import cv2 as cv

from misc_utils import get_last_packet, make_map



vid_out = None
WRITE_VIDEO = False
if WRITE_VIDEO:
    vid_out = cv.VideoWriter('map_video.avi', cv.VideoWriter_fourcc('M', 'J', 'P', 'G'), 30, (800, 800))


DATA_PORT = 25252

data_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data_sock.bind(("0.0.0.0", DATA_PORT))


depth_scale = 0.001


# FoV is +/- 105 degrees, so:
#half_rads = np.radians(105.0/2.0)
#angles = np.linspace(-half_rads, half_rads, 1280)
#cos_angles = np.cos(angles)
#sin_angles = np.sin(angles)

x_range = (np.arange(1280) - 640.0) / 619.0

_a_map = make_map(800, 800, 100)

#amap = np.copy(_a_map)
#cv.imshow('a map', amap)
#cv.moveWindow('a map', 560, 0)
#cv.waitKey(1)


def render_map(scanline):
    assert scanline.shape == x_range.shape
    amap = np.copy(_a_map)
    for i, z in enumerate(scanline):
        z = scanline[i] * depth_scale
        x = z * x_range[i]

        pixel_x = int(round(400 + 100*x))
        pixel_y = int(round(800 - 100*z))
        if pixel_x >= 0 and pixel_x < 800:
            if pixel_y >= 0 and pixel_y < 800:
                amap[pixel_y, pixel_x] = (200, 0, 0)

    cv.imshow('amap', amap)
    cv.waitKey(1)
    if WRITE_VIDEO:
        vid_out.write(amap)



while True:
    inputs, outputs, errors = select.select([data_sock], [], [])
    for oneInput in inputs:
        if oneInput == data_sock:

            data, addr = get_last_packet(data_sock)
            scanline = np.frombuffer(data, dtype='<u2')

            render_map(scanline)


