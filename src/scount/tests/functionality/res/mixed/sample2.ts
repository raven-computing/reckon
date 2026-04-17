#!/usr/bin/env node

import process from "node:process";

function main(name: string): number {
    const message: string = `Hello, ${name}`;
    console.log(message);
    return 0;
}

process.exit(
    main("world")
);
