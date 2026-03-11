// Golden sample: Contains JavaScript language features and constructs

import defaultExport, { readFileSync as readFile, promises as fsPromises } from "node:fs"; // +1 (import statement)
import * as path from "node:path"; // +1 (import statement)
import "node:events"; // +1 (import statement)

/* Block comment */

export const moduleUrl = import.meta.url; // +1 (export statement)
export const constants = { // +1 (export statement)
    dec: 42,
    float: 3.14,
    hex: 0xff,
    oct: 0o17,
    bin: 0b1010,
    big: 9007199254740993n,
    str: "text",
    tpl: `value:${1 + 2}`,
    re: /foo+/gi,
    nil: null,
    bool: true,
};

const [head, , ...tail] = [1, 2, 3, 4, 5]; // +1 (lexical declaration)

const { // +1 (lexical declaration)
    dec: renamedDec,
    missing = "fallback",
    ...restObject
} = constants;

let mutable = 0; // +1 (lexical declaration)
mutable += head ?? 0; // +1 (expression statement)
const maybeFn = defaultExport?.existsSync; // +1 (lexical declaration)
const ternary = renamedDec > 0 ? "positive" : "non-positive"; // +1 (lexical declaration)
const sequence = (mutable++, mutable + 1); // +1 (lexical declaration)

function regular(a = 1, b = 2, ...extra) { // +1 (function declaration)
    const sum = a + b + extra.length; // +1 (lexical declaration)
    return sum; // +1 (return statement)
}

const arrow = (x) => x * 2; // +1 (lexical declaration)
const conciseObject = (value) => ({ value, tail }); // +1 (lexical declaration)

function* generator(limit = 3) { // +1 (generator function declaration)
    let i = 0; // +1 (lexical declaration)
    while (i < limit) { // +1 (while statement)
        yield i++; // +1 (expression statement)
    }
    return i; // +1 (return statement)
}

async function* asyncGenerator(values) { // +1 (generator function declaration)
    for (const value of values) { // +1 (for in statement)
        yield await Promise.resolve(value); // +1 (expression statement)
    }
}

async function asyncFn(input) { // +1 (function declaration)
    const loaded = await import("node:os"); // +1 (lexical declaration)
    return loaded.platform() + input; // +1 (return statement)
}

class Base { // +1 (class declaration)

    constructor(name) { // +1 (method definition)
        this.name = name; // +1 (expression statement)
    }

    describe() { // +1 (method definition)
        return `Base(${this.name})`; // +1 (return statement)
    }
}

@sealed // +2 (export statement, decorator)
export class Sample extends Base {
    static counter = 0; // +1 (field definition)
    static ["computed" + "Name"] = "ok"; // +1 (field definition)
    #secret = 7; // +1 (field definition)

    static { // +1 (class static block)
        this.counter = 1; // +1 (expression statement)
    }

    constructor(name = "sample") { // +1 (method definition)
        super(name); // +1 (expression statement)
    }

    get value() { // +1 (method definition)
        return this.#secret; // +1 (return statement)
    }

    set value(next) { // +1 (method definition)
        this.#secret = next; // +1 (expression statement)
    }

    #privateMethod(step = 1) { // +1 (method definition)
        this.#secret += step; // +1 (expression statement)
    }

    *iter(times = 2) { // +1 (method definition)
        for (let i = 0; i < times; ++i) { // +1 (for statement)
            yield this.#secret + i; // +1 (expression statement)
        }
    }

    async work(values) { // +1 (method definition)
        for await (const item of asyncGenerator(values)) { // +1 (for in statement)
            if (item < 0) { // +1 (if statement)
                continue; // +1 (continue statement)
            }
            this.#privateMethod(item); // +1 (expression statement)
        }
        return this.#secret; // +1 (return statement)
    }

    method(value) { // +1 (method definition)
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
            with ({ value }) { // +1 (with statement)
                debugger; // +1 (debugger statement)
                value = value + 1; // +1 (expression statement)
            }
        } catch (error) { // +1 (catch clause)
            value = error instanceof Error ? -1 : -2; // +1 (expression statement)
        } finally { // +1 (finally clause)
            value = Math.trunc(value); // +1 (expression statement)
        }

        delete this.extra; // +1 (expression statement)
        void value; // +1 (expression statement)
        return value; // +1 (return statement)
    }
}

export default function makeSample() { // +1 (export statement)
    const sample = new Sample(path.basename(moduleUrl)); // +1 (lexical declaration)
    sample.value ??= 1; // +1 (expression statement)
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
        maybeFn,
        ternary,
        sequence,
    };
    Object.defineProperty(object, "hidden", { // +1 (expression statement)
        value: true,
        enumerable: false
    });
    return object; // +1 (return statement)
}

export { // +1 (export statement)
    Base, regular, arrow, conciseObject,
    generator, asyncFn, tail 
};
