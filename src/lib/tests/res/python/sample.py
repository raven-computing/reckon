"""
Golden sample: Contains Python language features and constructs

Only Python 3 is covered.
"""

from __future__ import annotations

import math

import sys as _sys

from dataclasses import dataclass, field
from contextlib import contextmanager, asynccontextmanager, nullcontext
from typing import *

INT_DEC: Final[int] = 1_000_000
INT_HEX = 0xff
INT_BIN = 0b1010
FLT = 1.25e-3
CPLX = 1 + 2j
ELL = ...

S1 = "adjacent " "string" # Concatenated literals
S2 = r"raw\string"
S3 = b"bytes\x00"
S4 = f"fstring: {INT_DEC=:_} {math.pi:0.3f} {S1!r}"
S5 = """triple
quoted
string
"""

LIST = [0, 1, *[2, 3], *(4, 5)]
TUP = (1, 2, 3)
SET = {1, 2, 3}
DICT = {"a": 1, **{"b": 2}, "c": 3}

# Line continuation forms
X = (1 +
     2 +
     3)
# Y = 1 + \
#     2

type UserId = int
type Pair[T] = tuple[T, T]

T = TypeVar("T")
U = TypeVar("U")

def deco(fn):
    def wrapper(*a, **k):
        return fn(*a, **k)

    return wrapper

def deco_with_arg(tag: str):
    def d(fn):
        fn.__tag__ = tag
        return fn

    return d


@deco
@deco_with_arg("tagged")
def func_posonly_kwonly[T](
    x: T,
    /,
    y: int = 0,
    *,
    z: str = "z",
    **kw: Any
) -> T:
    """positional-only (/), keyword-only (*), annotations, type params.

    A docstring text.
    """ 
    # walrus operator + conditional expr
    msg = "big" if (n := y) > 10 else "small"
    assert isinstance(msg, str), "assert with message"

    # f-string format spec + conversion
    _ = f"{x!r} {n=:04d}"

    y += 1
    y -= 1
    y *= 2
    y //= 2
    y **= 2
    y >>= 1
    y <<= 1
    y |= 1
    y &= 1
    y ^= 1

    a = [0, 1, 2, 3, 4]
    _ = a[1:4:2]
    _ = a[-1]
    _ = a[:]
    _ = a[::]

    lc = [
        i * i for i in range(10) if i % 2 == 0
    ]
    sc = {i for i in range(5)}
    dc = {
        i: i * i
        for i in range(5)
    }
    gc = (i for i in range(3))

    first, *mid, last = [1, 2, 3, 4, 5]

    f = lambda t: t + 1
    _ = f(y)

    try:
        if y < 0:
            raise ValueError("neg")
    except ValueError as e:
        raise RuntimeError("wrapped") from e
    else:
        pass
    finally:
        y = y  # NO-OP

    return x


def gen() -> Iterator[int]:
    yield 1
    yield 2
    yield from (3, 4)
    return


G = 0


def scopes() -> int:
    global G
    G = 1
    x = 10

    def inner() -> int:
        nonlocal x
        x += 1
        return x

    y = inner()
    del y
    return x


@contextmanager
def cm(v: str) -> Iterator[str]:
    try:
        yield v
    finally:
        pass


class Base:
    def base(self) -> str:
        return "base"


@dataclass(slots=True)
class Point(Base):
    x: int
    y: int = 0
    tags: list[str] = field(
        default_factory=list
    )

    @property
    def norm(self) -> float:
        return (
            self.x * self.x + self.y * self.y
        ) ** 0.5

    @classmethod
    def origin(cls) -> Point:
        return cls(0, 0)

    @staticmethod
    def unit() -> Point:
        return Point(1, 1)

    def __call__(self, dx: int, dy: int) -> Point:
        return Point(
            self.x + dx,
            self.y + dy
        )


def matcher(
    obj: Any
) -> str:
    match obj:
        case 0 | 1:
            return "small-int"
        case [a, b, *rest]:
            return f"seq {a=} {b=} {rest=}"
        case {"k": v, **rest}:
            return f"map {v=} {rest=}"
        case Point(x=px, y=py) if px == py:
            return "diag"
        case Point() as p:
            return f"point {p.x},{p.y}"
        case _:
            return "other"


def loops(n: int) -> int:
    total = 0
    for i in range(n):
        if i == 2:
            continue

        if i == 5:
            break

        total += i

    else:
        total += 100

    j = 0
    while j < 3:
        j += 1
    else:
        pass

    return total


@asynccontextmanager
async def acm(v: str):
    yield v

async def agen() -> AsyncIterator[int]:
    for i in range(3):
        yield i

async def async_features() -> list[int]:
    out: list[int] = []
    async with acm("v") as v, acm(v + "2") as w:
        _ = (v, w)

    async for item in agen():
        out.append(item)

    return out


@overload
def ov(x: int) -> int: ...
@overload
def ov(x: str) -> str: ...
def ov(x):
    return x


def exception_groups_parse_only() -> None:
    if False:
        try:
            raise ExceptionGroup(
                "eg",
                [
                    ValueError("v"),
                    TypeError("t")
                ]
            )
        except* ValueError as eg:
            _ = eg
        except* TypeError:
            pass


def misc_parse_only() -> None:
    if False:
        s = "abc";
        _ = s.strip().upper()[0:2].encode("utf-8")

        a, b = 1, 2
        _ = (~a & b) | (a ^ b)
        _ = (a is not None) and (b in [1, 2, 3]) or (a not in (4, 5))

        _ = {
            k: (v := k * 2)
            for k in range(3)
            if v >= 0
        }
        _ = [
            i for i in range(10)
            if (j := i % 3) != 0
        ]

        with nullcontext(123) as v:
            _ = v

        exec("x_exec = 1\n")

        if a < 0:
            raise ValueError
        pass
    elif True:
        print("Yes")
