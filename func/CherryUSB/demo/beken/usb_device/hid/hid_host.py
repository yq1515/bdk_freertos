#!/usr/bin/env python3

import hid
import os

h = hid.device()
h.open(0xffff, 0xffff)

while True:
    h.write(b'1234567890abcdefghijklmnopqrst')
    r = h.read(100)
    print(r)
