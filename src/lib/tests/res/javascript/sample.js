// Golden sample: Contains JavaScript language features and constructs

import defaultExport, { readFileSync as readFile, promises as fsPromises } from "node:fs";
import * as path from "node:path";
import "node:events";

/* Block comment */

export const moduleUrl = import.meta.url;
export const constants = {
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

const [head, , ...tail] = [1, 2, 3, 4, 5];

const {
    dec: renamedDec,
    missing = "fallback",
    ...restObject
} = constants;

let mutable = 0;
mutable += head ?? 0;
const maybeFn = defaultExport?.existsSync;
const ternary = renamedDec > 0 ? "positive" : "non-positive";
const sequence = (mutable++, mutable + 1);

function regular(a = 1, b = 2, ...extra) {
    const sum = a + b + extra.length;
    return sum;
}

const arrow = (x) => x * 2;
const conciseObject = (value) => ({ value, tail });

function* generator(limit = 3) {
    let i = 0;
    while (i < limit) {
        yield i++;
    }
    return i;
}

async function* asyncGenerator(values) {
    for (const value of values) {
        yield await Promise.resolve(value);
    }
}

async function asyncFn(input) {
    const loaded = await import("node:os");
    return loaded.platform() + input;
}

class Base {

    constructor(name) {
        this.name = name;
    }

    describe() {
        return `Base(${this.name})`;
    }
}

@sealed
export class Sample extends Base {
    static counter = 0;
    static ["computed" + "Name"] = "ok";
    #secret = 7;

    static {
        this.counter = 1;
    }

    constructor(name = "sample") {
        super(name);
    }

    get value() {
        return this.#secret;
    }

    set value(next) {
        this.#secret = next;
    }

    #privateMethod(step = 1) {
        this.#secret += step;
    }

    *iter(times = 2) {
        for (let i = 0; i < times; ++i) {
            yield this.#secret + i;
        }
    }

    async work(values) {
        for await (const item of asyncGenerator(values)) {
            if (item < 0) {
                continue;
            }
            this.#privateMethod(item);
        }
        return this.#secret;
    }

    method(value) {
        outer: for (const key in restObject) {
            if (!(key in constants)) {
                break outer;
            }
            if (key === "skip") {
                continue;
            }
        }

        theblock: {
            console.log("Inside a block");
            break theblock;
        }

        if (value > 0 && value !== 1) {
            for (let i = 0; i < value; ++i) {
                mutable ^= i;
            }
        } else if (value === 0 || Number.isNaN(value)) {
            return 0;
        } else {
            do {
                value++;
            } while (value < 0);
        }

        switch (value) {
            case 1:
                value += 1;
                break;
            default:
                value = value ?? 1;
        }

        try {
            if (value < 0) {
                throw new RangeError("negative");
            }
            with ({ value }) {
                debugger;
                value = value + 1;
            }
        } catch (error) {
            value = error instanceof Error ? -1 : -2;
        } finally {
            value = Math.trunc(value);
        }

        delete this.extra;
        void value;
        return value;
    }
}

export default function makeSample() {
    const sample = new Sample(path.basename(moduleUrl));
    sample.value ??= 1;
    const object = {
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
    Object.defineProperty(object, "hidden", {
        value: true,
        enumerable: false
    });
    return object;
}

export {
    Base, regular, arrow, conciseObject,
    generator, asyncFn, tail 
};
