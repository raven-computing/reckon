// Golden sample: Contains TypeScript language features and constructs

import type { PathLike } from "node:fs"; // +1 (import statement)
import { readFileSync as readFile, promises as fsPromises } from "node:fs"; // +1 (import statement)
import * as path from "node:path"; // +1 (import statement)
import "node:events"; // +1 (import statement)

/* Block comment */

// --- Interfaces ---

interface Named {
    name: string;
    readonly id: number;
    describe(): string;
}

interface Configurable<T> {
    readonly options: T;
    configure(opts: Partial<T>): void;
}

// --- Type aliases ---

type StringOrNumber = string | number;
type Nullable<T> = T | null;
type EventMap = { [K: string]: unknown };

// --- Enums ---

enum Direction {
    Up = "UP",
    Down = "DOWN",
    Left = "LEFT",
    Right = "RIGHT",
}

const enum Flags {
    None = 0,
    Read = 1 << 0,
    Write = 1 << 1,
}

// --- Module state ---

export const moduleUrl: string = import.meta.url; // +1 (export statement)
export const constants = { // +1 (export statement)
    dec: 42 as number,
    float: 3.14,
    hex: 0xff,
    oct: 0o17,
    bin: 0b1010,
    big: 9007199254740993n,
    str: "text",
    tpl: `value:${1 + 2}`,
    re: /foo+/gi,
    nil: null as Nullable<string>,
    bool: true,
    dir: Direction.Up,
};

const [head, , ...tail]: number[] = [ // +1 (lexical declaration)
    1, 2, 3, 4, 5
];

const { // +1 (lexical declaration)
    dec: renamedDec,
    missing = "fallback",
    ...restObject
} = constants;

let mutable: number = 0; // +1 (lexical declaration)
mutable += head ?? 0; // +1 (expression statement)
const maybePath: PathLike | undefined = undefined; // +1 (lexical declaration)
const ternary: string = renamedDec > 0 ? "positive" : "non-positive"; // +1 (lexical declaration)
const sequence = (mutable++, mutable + 1); // +1 (lexical declaration)

// --- Generic functions ---

function identity<T>(value: T): T { // +1 (function declaration)
    return value; // +1 (return statement)
}

function regular( // +1 (function declaration)
    a: number = 1,
    b: number = 2,
    ...extra: number[]
): number {
    const sum = a + b + extra.length; // +1 (lexical declaration)
    return sum; // +1 (return statement)
}

function isString(value: unknown): value is string { // +1 (function declaration)
    return typeof value === "string"; // +1 (return statement)
}

// Function overload signatures (not counted as logical lines)
function format(value: number): string;
function format(value: string): string;
function format(value: StringOrNumber): string { // +1 (function declaration)
    return String(value); // +1 (return statement)
}

const arrow = (x: number): number => x * 2; // +1 (lexical declaration)
const conciseObject = (value: number) => ({ value, tail }); // +1 (lexical declaration)

function* generator(limit: number = 3): Generator<number> { // +1 (generator function declaration)
    let i: number = 0; // +1 (lexical declaration)
    while (i < limit) { // +1 (while statement)
        yield i++; // +1 (expression statement)
    }
    return i; // +1 (return statement)
}

async function* asyncGenerator<T>(values: T[]): AsyncGenerator<T> { // +1 (generator function declaration)
    for (const value of values) { // +1 (for in statement)
        yield await Promise.resolve(value); // +1 (expression statement)
    }
}

async function asyncFn(input: string): Promise<string> { // +1 (function declaration)
    const loaded = await import("node:os"); // +1 (lexical declaration)
    return loaded.platform() + input; // +1 (return statement)
}

// --- Classes ---

abstract class Base implements Named { // +1 (abstract class declaration)
    abstract describe(): string; // +1 (abstract method signature)
    readonly id: number; // +1 (public field definition)

    constructor(public readonly name: string, id: number) { // +1 (method definition)
        this.id = id; // +1 (expression statement)
    }
}

@sealed // +2 (export statement, decorator)
export class Sample extends Base implements Configurable<{ step: number }> {
    static counter: number = 0; // +1 (public field definition)
    static readonly ["computed" + "Name"]: string = "ok"; // +1 (public field definition)
    readonly options: { step: number } = { step: 1 }; // +1 (public field definition)
    #secret: number = 7; // +1 (public field definition)

    static { // +1 (class static block)
        this.counter = 1; // +1 (expression statement)
    }

    constructor(name: string = "sample") { // +1 (method definition)
        super(name, Sample.counter); // +1 (expression statement)
    }

    describe(): string { // +1 (method definition)
        return `Sample(${this.name})`; // +1 (return statement)
    }

    configure(opts: Partial<{ step: number }>): void { // +1 (method definition)
        Object.assign(this.options, opts); // +1 (expression statement)
    }

    get value(): number { // +1 (method definition)
        return this.#secret; // +1 (return statement)
    }

    set value(next: number) { // +1 (method definition)
        this.#secret = next; // +1 (expression statement)
    }

    #privateMethod(step: number = 1): void { // +1 (method definition)
        this.#secret += step; // +1 (expression statement)
    }

    *iter(times: number = 2): Generator<number> { // +1 (method definition)
        for (let i = 0; i < times; ++i) { // +1 (for statement)
            yield this.#secret + i; // +1 (expression statement)
        }
    }

    async work(values: number[]): Promise<number> { // +1 (method definition)
        for await (const item of asyncGenerator(values)) { // +1 (for in statement)
            if (item < 0) { // +1 (if statement)
                continue; // +1 (continue statement)
            }
            this.#privateMethod(item); // +1 (expression statement)
        }
        return this.#secret; // +1 (return statement)
    }

    method(value: number): number { // +1 (method definition)
        outer: for (const key in restObject) { // +1 (for in statement)
            if (!(key in constants)) { // +1 (if statement)
                break outer; // +1 (break statement)
            }
            if (key === "skip") { // +1 (if statement)
                continue; // +1 (continue statement)
            }
        }

        theblock: { // +1 (labeled statement)
            console.log("Inside a block"); // +1 (expression statement)
            break theblock; // +1 (break statement)
        }

        if (value > 0 && value !== 1) { // +1 (if statement)
            for (let i = 0; i < value; ++i) { // +1 (for statement)
                mutable ^= i; // +1 (expression statement)
            }
        } else if (value === 0 || Number.isNaN(value)) { // +1 (else clause)
            return 0; // +1 (return statement)
        } else { // +1 (else clause)
            do { // +2 (do statement)
                value++; // +1 (expression statement)
            } while (value < 0);
        }

        switch (value) { // +1 (switch statement)
            case 1: // +1 (switch case)
                value += 1; // +1 (expression statement)
                break; // +1 (break statement)
            default: // +1 (switch default)
                value = value ?? 1; // +1 (expression statement)
        }

        try { // +1 (try statement)
            if (value < 0) { // +1 (if statement)
                throw new RangeError("negative"); // +1 (throw statement)
            }
        } catch (error) { // +1 (catch clause)
            value = error instanceof Error ? -1 : -2; // +1 (expression statement)
        } finally { // +1 (finally clause)
            value = Math.trunc(value); // +1 (expression statement)
        }

        const narrowed = isString( // +1 (lexical declaration)
            identity<string>("test")
        ) ? String(value) : "";
        debugger; // +1 (debugger statement)
        delete (this as { extra?: unknown }).extra; // +1 (expression statement)
        void narrowed; // +1 (expression statement)
        return value; // +1 (return statement)
    }
}

// --- Namespace ---

namespace Utils { // +1 (expression statement)
    export function greet(name: string): string { // +1 (export statement)
        return `Hello, ${name}!`; // +1 (return statement)
    }

    export const version: string = "1.0"; // +1 (export statement)
}

// --- Default export ---

export default function makeSample(): object { // +1 (export statement)
    const sample = new Sample(path.basename(moduleUrl)); // +1 (lexical declaration)
    sample.value ??= 1; // +1 (expression statement)
    const flags: number = Flags.Read | Flags.Write; // +1 (lexical declaration)
    const object = { // +1 (lexical declaration)
        __proto__: null,
        sample,
        regular,
        arrow,
        conciseObject,
        generator,
        asyncFn,
        readFile,
        fsPromises,
        maybePath,
        ternary,
        sequence,
        flags,
    };
    Object.defineProperty(object, "hidden", { // +1 (expression statement)
        value: true,
        enumerable: false
    });
    return object; // +1 (return statement)
}

export { // +1 (export statement)
    Base, Sample, regular, arrow, conciseObject,
    generator, asyncFn, tail, Utils
};

export type { Named, Configurable, StringOrNumber }; // +1 (export statement)
