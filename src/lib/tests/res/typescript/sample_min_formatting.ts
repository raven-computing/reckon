// Golden sample: Contains TypeScript language features and constructs

import type { PathLike } from "node:fs";
import { readFileSync as readFile, promises as fsPromises } from "node:fs";
import * as path from "node:path";
import "node:events";

/* Block comment */

// --- Interfaces ---

interface Named { name: string; readonly id: number; describe(): string; }
interface Configurable<T> { readonly options: T; configure(opts: Partial<T>): void; }

// --- Type aliases ---

type StringOrNumber = string | number;
type Nullable<T> = T | null;
type EventMap = { [K: string]: unknown };

// --- Enums ---

enum Direction { Up = "UP", Down = "DOWN", Left = "LEFT", Right = "RIGHT", }
const enum Flags { None = 0, Read = 1 << 0, Write = 1 << 1, }

// --- Module state ---

export const moduleUrl: string = import.meta.url;
export const constants = { dec: 42 as number, float: 3.14, hex: 0xff, oct: 0o17, bin: 0b1010, big: 9007199254740993n, str: "text", tpl: `value:${1 + 2}`, re: /foo+/gi, nil: null as Nullable<string>, bool: true, dir: Direction.Up, };
const [head, , ...tail]: number[] = [1, 2, 3, 4, 5];
const { dec: renamedDec, missing = "fallback", ...restObject } = constants;

let mutable: number = 0;
mutable += head ?? 0;
const maybePath: PathLike | undefined = undefined;
const ternary: string = renamedDec > 0 ? "positive" : "non-positive";
const sequence = (mutable++, mutable + 1);

// --- Generic functions ---

function identity<T>(value: T): T { return value; }
function regular(a: number = 1, b: number = 2, ...extra: number[]): number {
    const sum = a + b + extra.length;
    return sum;
}
function isString(value: unknown): value is string { return typeof value === "string"; }

// Function overload signatures (not counted as logical lines)
function format(value: number): string;
function format(value: string): string;
function format(value: StringOrNumber): string { return String(value); }
const arrow = (x: number): number => x * 2;
const conciseObject = (value: number) => ({ value, tail });
function* generator(limit: number = 3): Generator<number> {
    let i: number = 0; while (i < limit) { yield i++; }
    return i;
}
async function* asyncGenerator<T>(values: T[]): AsyncGenerator<T> {
    for (const value of values) { yield await Promise.resolve(value); }
}
async function asyncFn(input: string): Promise<string> {
    const loaded = await import("node:os");
    return loaded.platform() + input;
}

// --- Classes ---

abstract class Base implements Named {
    abstract describe(): string;
    readonly id: number;
    constructor(public readonly name: string, id: number) { this.id = id; }
}
@sealed export class Sample extends Base implements Configurable<{ step: number }> {
    static counter: number = 0;
    static readonly ["computed" + "Name"]: string = "ok";
    readonly options: { step: number } = { step: 1 };
    #secret: number = 7;
    static { this.counter = 1; }
    constructor(name: string = "sample") { super(name, Sample.counter); }
    describe(): string { return `Sample(${this.name})`; }
    configure(opts: Partial<{ step: number }>): void { Object.assign(this.options, opts); }
    get value(): number { return this.#secret; }
    set value(next: number) { this.#secret = next; }
    #privateMethod(step: number = 1): void { this.#secret += step; }
    *iter(times: number = 2): Generator<number> {
        for (let i = 0; i < times; ++i) { yield this.#secret + i; }
    }
    async work(values: number[]): Promise<number> {
        for await (const item of asyncGenerator(values)) {
            if (item < 0) { continue; }
            this.#privateMethod(item);
        }
        return this.#secret;
    }
    method(value: number): number {
        outer: for (const key in restObject) {
            if (!(key in constants)) { break outer; }
            if (key === "skip") { continue; }
        }
        theblock: { console.log("Inside a block"); break theblock; }
        if (value > 0 && value !== 1) {
            for (let i = 0; i < value; ++i) { mutable ^= i; }
        } else if (value === 0 || Number.isNaN(value)) {
            return 0;
        } else {
            do { value++; } while (value < 0);
        }
        switch (value) {
            case 1: value += 1; break;
            default: value = value ?? 1;
        }
        try {
            if (value < 0) { throw new RangeError("negative"); }
        } catch (error) {
            value = error instanceof Error ? -1 : -2;
        } finally {
            value = Math.trunc(value);
        }
        const narrowed = isString(identity<string>("test")) ? String(value) : "";
        debugger;
        delete (this as { extra?: unknown }).extra;
        void narrowed;
        return value;
    }
}

// --- Namespace ---

namespace Utils {
    export function greet(name: string): string { return `Hello, ${name}!`; }
    export const version: string = "1.0";
}

// --- Default export ---

export default function makeSample(): object {
    const sample = new Sample(path.basename(moduleUrl));
    sample.value ??= 1;
    const flags: number = Flags.Read | Flags.Write;
    const object = { __proto__: null, sample, regular, arrow, conciseObject, generator, asyncFn, readFile, fsPromises, maybePath, ternary, sequence, flags, };
    Object.defineProperty(object, "hidden", { value: true, enumerable: false });
    return object;
}
export { Base, Sample, regular, arrow, conciseObject, generator, asyncFn, tail, Utils };
export type { Named, Configurable, StringOrNumber };
