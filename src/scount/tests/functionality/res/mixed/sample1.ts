#!/usr/bin/env node

import process from "node:process";

function main(): number {
    console.log(
        "This is a first sample program written in TypeScript"
    );
    return 0;
}

process.exit(
    main()
);
