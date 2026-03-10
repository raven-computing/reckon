#!/usr/bin/env node

function main(name) {
  const message = `Hello, ${name}`;
  console.log(message);
  return 0;
}

process.exit(main("world"));
