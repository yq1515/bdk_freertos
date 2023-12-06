#!/usr/bin/env python3

import hid
import os

h = hid.device()
h.open(0xffff, 0xffff)

#h.write(b'12345678')
# r = h.read(100)
# print(r)
