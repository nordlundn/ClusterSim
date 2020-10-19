#!/bin/bash

rm *.so
python setup2.py clean
python setup2.py build_ext -i
