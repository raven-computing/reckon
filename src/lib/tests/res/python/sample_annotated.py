"""
Golden sample: Contains Python language features and constructs

Only Python 3 is covered.
"""

from __future__ import annotations # +1 (future import statement)

import math # +1 (import statement)

import sys as _sys # +1 (import statement)

from dataclasses import dataclass, field # +1 (import from statement)
from contextlib import contextmanager, asynccontextmanager, nullcontext # +1 (import from statement)
from typing import * # +1 (import from statement)

INT_DEC: Final[int] = 1_000_000 # +1 (expression statement)
INT_HEX = 0xff # +1 (expression statement)
INT_BIN = 0b1010 # +1 (expression statement)
FLT = 1.25e-3 # +1 (expression statement)
CPLX = 1 + 2j # +1 (expression statement)
ELL = ... # +1 (expression statement)

S1 = "adjacent " "string" # Concatenated literals # +1 (expression statement)
S2 = r"raw\string" # +1 (expression statement)
S3 = b"bytes\x00" # +1 (expression statement)
S4 = f"fstring: {INT_DEC=:_} {math.pi:0.3f} {S1!r}" # +1 (expression statement)
S5 = """triple # +1 (expression statement)
quoted
string
"""

LIST = [0, 1, *[2, 3], *(4, 5)] # +1 (expression statement)
TUP = (1, 2, 3) # +1 (expression statement)
SET = {1, 2, 3} # +1 (expression statement)
DICT = {"a": 1, **{"b": 2}, "c": 3} # +1 (expression statement)

# Line continuation forms
X = (1 + # +1 (expression statement)
     2 +
     3)
# Y = 1 + \
#     2

type UserId = int # +1 (type alias statement)
type Pair[T] = tuple[T, T] # +1 (type alias statement)

T = TypeVar("T") # +1 (expression statement)
U = TypeVar("U") # +1 (expression statement)

def deco(fn): # +1 (function definition)
    def wrapper(*a, **k): # +1 (function definition)
        return fn(*a, **k) # +1 (return statement)

    return wrapper # +1 (return statement)

def deco_with_arg(tag: str): # +1 (function definition)
    def d(fn): # +1 (function definition)
        fn.__tag__ = tag # +1 (expression statement)
        return fn # +1 (return statement)

    return d # +1 (return statement)


@deco # +1 (decorator)
@deco_with_arg("tagged") # +1 (decorator)
def func_posonly_kwonly[T]( # +1 (function definition)
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
    msg = "big" if (n := y) > 10 else "small" # +1 (expression statement)
    assert isinstance(msg, str), "assert with message" # +1 (assert statement)

    # f-string format spec + conversion
    _ = f"{x!r} {n=:04d}" # +1 (expression statement)

    y += 1 # +1 (expression statement)
    y -= 1 # +1 (expression statement)
    y *= 2 # +1 (expression statement)
    y //= 2 # +1 (expression statement)
    y **= 2 # +1 (expression statement)
    y >>= 1 # +1 (expression statement)
    y <<= 1 # +1 (expression statement)
    y |= 1 # +1 (expression statement)
    y &= 1 # +1 (expression statement)
    y ^= 1 # +1 (expression statement)

    a = [0, 1, 2, 3, 4] # +1 (expression statement)
    _ = a[1:4:2] # +1 (expression statement)
    _ = a[-1] # +1 (expression statement)
    _ = a[:] # +1 (expression statement)
    _ = a[::] # +1 (expression statement)

    lc = [ # +1 (expression statement)
        i * i for i in range(10) if i % 2 == 0
    ]
    sc = {i for i in range(5)} # +1 (expression statement)
    dc = { # +1 (expression statement)
        i: i * i
        for i in range(5)
    }
    gc = (i for i in range(3)) # +1 (expression statement)

    first, *mid, last = [1, 2, 3, 4, 5] # +1 (expression statement)

    f = lambda t: t + 1 # +1 (expression statement)
    _ = f(y) # +1 (expression statement)

    try: # +1 (try statement)
        if y < 0: # +1 (if statement)
            raise ValueError("neg") # +1 (raise statement)
    except ValueError as e: # +1 (except clause)
        raise RuntimeError("wrapped") from e # +1 (raise statement)
    else: # +1 (else clause)
        pass # +1 (pass statement)
    finally: # +1 (finally clause)
        y = y  # NO-OP # +1 (expression statement)

    return x # +1 (return statement)


def gen() -> Iterator[int]: # +1 (function definition)
    yield 1 # +1 (expression statement)
    yield 2 # +1 (expression statement)
    yield from (3, 4) # +1 (expression statement)
    return # +1 (return statement)


G = 0 # +1 (expression statement)


def scopes() -> int: # +1 (function definition)
    global G # +1 (global statement)
    G = 1 # +1 (expression statement)
    x = 10 # +1 (expression statement)

    def inner() -> int: # +1 (function definition)
        nonlocal x # +1 (nonlocal statement)
        x += 1 # +1 (expression statement)
        return x # +1 (return statement)

    y = inner() # +1 (expression statement)
    del y # +1 (delete statement)
    return x # +1 (return statement)


@contextmanager # +1 (decorator)
def cm(v: str) -> Iterator[str]: # +1 (function definition)
    try: # +1 (try statement)
        yield v # +1 (expression statement)
    finally: # +1 (finally clause)
        pass # +1 (pass statement)


class Base: # +1 (class definition)
    def base(self) -> str: # +1 (function definition)
        return "base" # +1 (return statement)


@dataclass(slots=True) # +1 (decorator)
class Point(Base): # +1 (class definition)
    x: int # +1 (expression statement)
    y: int = 0 # +1 (expression statement)
    tags: list[str] = field( # +1 (expression statement)
        default_factory=list
    )

    @property # +1 (decorator)
    def norm(self) -> float: # +1 (function definition)
        return ( # +1 (return statement)
            self.x * self.x + self.y * self.y
        ) ** 0.5

    @classmethod # +1 (decorator)
    def origin(cls) -> Point: # +1 (function definition)
        return cls(0, 0) # +1 (return statement)

    @staticmethod # +1 (decorator)
    def unit() -> Point: # +1 (function definition)
        return Point(1, 1) # +1 (return statement)

    def __call__(self, dx: int, dy: int) -> Point: # +1 (function definition)
        return Point( # +1 (return statement)
            self.x + dx,
            self.y + dy
        )


def matcher( # +1 (function definition)
    obj: Any
) -> str:
    match obj: # +1 (match statement)
        case 0 | 1: # +1 (case clause)
            return "small-int" # +1 (return statement)
        case [a, b, *rest]: # +1 (case clause)
            return f"seq {a=} {b=} {rest=}" # +1 (return statement)
        case {"k": v, **rest}: # +1 (case clause)
            return f"map {v=} {rest=}" # +1 (return statement)
        case Point(x=px, y=py) if px == py: # +1 (case clause)
            return "diag" # +1 (return statement)
        case Point() as p: # +1 (case clause)
            return f"point {p.x},{p.y}" # +1 (return statement)
        case _: # +1 (case clause)
            return "other" # +1 (return statement)


def loops(n: int) -> int: # +1 (function definition)
    total = 0 # +1 (expression statement)
    for i in range(n): # +1 (for statement)
        if i == 2: # +1 (if statement)
            continue # +1 (continue statement)

        if i == 5: # +1 (if statement)
            break # +1 (break statement)

        total += i # +1 (expression statement)

    else: # +1 (else clause)
        total += 100 # +1 (expression statement)

    j = 0 # +1 (expression statement)
    while j < 3: # +1 (while statement)
        j += 1 # +1 (expression statement)
    else: # +1 (else clause)
        pass # +1 (pass statement)

    return total # +1 (return statement)


@asynccontextmanager # +1 (decorator)
async def acm(v: str): # +1 (function definition)
    yield v # +1 (expression statement)

async def agen() -> AsyncIterator[int]: # +1 (function definition)
    for i in range(3): # +1 (for statement)
        yield i # +1 (expression statement)

async def async_features() -> list[int]: # +1 (function definition)
    out: list[int] = [] # +1 (expression statement)
    async with acm("v") as v, acm(v + "2") as w: # +1 (with statement)
        _ = (v, w) # +1 (expression statement)

    async for item in agen(): # +1 (for statement)
        out.append(item) # +1 (expression statement)

    return out # +1 (return statement)


@overload # +1 (decorator)
def ov(x: int) -> int: ... # +2 (function definition, expression statement)
@overload # +1 (decorator)
def ov(x: str) -> str: ... # +2 (function definition, expression statement)
def ov(x): # +1 (function definition)
    return x # +1 (return statement)


def exception_groups_parse_only() -> None: # +1 (function definition)
    if False: # +1 (if statement)
        try: # +1 (try statement)
            raise ExceptionGroup( # +1 (raise statement)
                "eg",
                [
                    ValueError("v"),
                    TypeError("t")
                ]
            )
        except* ValueError as eg: # +1 (except clause)
            _ = eg # +1 (expression statement)
        except* TypeError: # +1 (except clause)
            pass # +1 (pass statement)


def misc_parse_only() -> None: # +1 (function definition)
    if False: # +1 (if statement)
        s = "abc"; # +1 (expression statement)
        _ = s.strip().upper()[0:2].encode("utf-8") # +1 (expression statement)

        a, b = 1, 2 # +1 (expression statement)
        _ = (~a & b) | (a ^ b) # +1 (expression statement)
        _ = (a is not None) and (b in [1, 2, 3]) or (a not in (4, 5)) # +1 (expression statement)

        _ = { # +1 (expression statement)
            k: (v := k * 2)
            for k in range(3)
            if v >= 0
        }
        _ = [ # +1 (expression statement)
            i for i in range(10)
            if (j := i % 3) != 0
        ]

        with nullcontext(123) as v: # +1 (with statement)
            _ = v # +1 (expression statement)

        exec("x_exec = 1\n") # +1 (expression statement)

        if a < 0: # +1 (if statement)
            raise ValueError # +1 (raise statement)
        pass # +1 (pass statement)
    elif True: # +1 (elif clause)
        print("Yes") # +1 (expression statement)
