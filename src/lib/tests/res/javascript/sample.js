// Golden sample: Contains JavaScript language features and constructs

import fs from "node:fs";

@sealed
export class Sample {
    static counter = 0;

    static {
        Sample.counter = 1;
    }

    method(value) {
        if (value > 0) {
            for (let i = 0; i < value; ++i) {
                console.log(i);
            }
        } else if (value === 0) {
            return 0;
        } else {
            do {
                value++;
            } while (value < 0);
        }

        switch (value) {
            case 1:
                break;
            default:
                throw new Error("bad");
        }

        try {
            with ({ value }) {
                debugger;
            }
        } catch (error) {
            return value;
        } finally {
            value = 1;
        }

        return value;
    }
}

export function makeSample() {
    const sample = new Sample();
    return sample;
}
