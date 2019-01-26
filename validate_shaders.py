#!/usr/bin/python

from __future__ import print_function
import argparse
import os
import subprocess
import sys

_VALIDATOR = 'glslangValidator'
_EXTS = ['.vert', '.tesc', '.tese', '.geom', '.frag', '.comp']


def validate_shaders(dir_):
    for file_name in os.listdir(dir_):
        if os.path.splitext(file_name)[1] in _EXTS:
            name = os.path.join(dir_, file_name)
            error = subprocess.call([_VALIDATOR, name])
            if error != 0:
                return error
    return 0


def main():
    with open(os.devnull) as devnull:
        try:
            subprocess.call([_VALIDATOR, '-v'], stdout=devnull, stderr=devnull)
        except OSError:
            print("warning: {} not installed, shaders cannot be validated at build time".format(_VALIDATOR), file=sys.stderr)
            exit(0)

    parser = argparse.ArgumentParser()
    parser.add_argument('directory')
    args = parser.parse_args()

    exit(validate_shaders(args.directory))


if __name__ == "__main__":
    main()
