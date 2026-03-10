// Golden sample: Contains JavaScript language features and constructs

import fs from "node:fs"; // +1 (import statement)

@sealed // +2 (export statement, decorator)
export class Sample {
    static counter = 0; // +1 (field definition)

    static { // +1 (class static block)
        Sample.counter = 1; // +1 (expression statement)
    }

    method(value) { // +1 (method definition)
        if (value > 0) { // +1 (if statement)
            for (let i = 0; i < value; ++i) { // +1 (for statement)
                console.log(i); // +1 (expression statement)
            }
        } else if (value === 0) { // +1 (else clause)
            return 0; // +1 (return statement)
        } else { // +1 (else clause)
            do { // +2 (do statement)
                value++; // +1 (expression statement)
            } while (value < 0);
        }

        switch (value) { // +1 (switch statement)
            case 1: // +1 (switch case)
                break; // +1 (break statement)
            default: // +1 (switch default)
                throw new Error("bad"); // +1 (throw statement)
        }

        try { // +1 (try statement)
            with ({ value }) { // +1 (with statement)
                debugger; // +1 (debugger statement)
            }
        } catch (error) { // +1 (catch clause)
            return value; // +1 (return statement)
        } finally { // +1 (finally clause)
            value = 1; // +1 (expression statement)
        }

        return value; // +1 (return statement)
    }
}

export function makeSample() { // +1 (export statement)
    const sample = new Sample(); // +1 (lexical declaration)
    return sample; // +1 (return statement)
}
