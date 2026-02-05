#!/usr/bin/env python

import sys


def main(argc: int, argv: list[str]) -> int:
    print("This is a second sample program written in Python")
    print("Here is another line")
    return 0


if __name__ == "__main__":
    sys.exit(main(len(sys.argv), sys.argv))
