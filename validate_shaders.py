#!/usr/bin/python

from __future__ import print_function
import argparse
import os
import subprocess as sp
import sys

_VALIDATOR = 'glslangValidator'
_EXTS = ['.vert', '.tesc', '.tese', '.geom', '.frag', '.comp']


def validate_shaders(dir_, version_string):
    for dirpath, _, filenames in os.walk(dir_):
        for file_name in filenames:
            ext = os.path.splitext(file_name)[1]
            if ext in _EXTS:
                name = os.path.join(dirpath, file_name)
                with open(name, r'r') as shader:
                    source = version_string + shader.read()
                    p = sp.Popen(
                        [_VALIDATOR, '--stdin', '-S', ext[1:]], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)
                    error = p.communicate(input=source)[0]
                    if p.returncode != 0:
                        print(file_name)
                        # validator prints 'stdin' first when printing
                        # errors in source read from stdin, trim that off
                        print(error.split('\n', 1)[1])
                        return p.returncode
    return 0


def main():
    with open(os.devnull) as devnull:
        try:
            sp.call([_VALIDATOR, '-v'], stdout=devnull, stderr=devnull)
        except OSError:
            print("warning: {} not installed, shaders cannot be validated at build time".format(
                _VALIDATOR), file=sys.stderr)
            exit(0)

    parser = argparse.ArgumentParser()
    parser.add_argument('directory')
    parser.add_argument('versionMajor')
    parser.add_argument('versionMinor')
    args = parser.parse_args()

    version_string = '#version {}{}0 core\n'.format(
        args.versionMajor, args.versionMinor)

    exit(validate_shaders(args.directory, version_string))


if __name__ == "__main__":
    main()
