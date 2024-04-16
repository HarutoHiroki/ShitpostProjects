const fs = require("fs");
const genZFunctions = require("./mappings.js");
const unGenZFunctions = {};
let arguments = process.argv;
let option = arguments[2];
let fileDir = arguments[3];

for (const [key, value] of Object.entries(genZFunctions)) {
  unGenZFunctions[value] = key;
}

function genZify(file) {
  let code = fs.readFileSync(file, "utf8");
  const regex = new RegExp(Object.keys(unGenZFunctions).join("|"), "g");
  return code.replace(regex, matched => unGenZFunctions[matched]);
}

function ungenZify(file) {
  let code = fs.readFileSync(file, "utf8");
  const regex = new RegExp(Object.keys(genZFunctions).join("|"), "g");
  return code.replace(regex, matched => genZFunctions[matched]);
}

switch (option) {
  case "-t":
    fs.writeFileSync(fileDir, genZify(fileDir));
    break;
  case "-u":
    fs.writeFileSync(fileDir, ungenZify(fileDir));
    break;
  case "-r":
    eval(ungenZify(fileDir));
    break;
  case "-h":
    console.log("Usage: node genZtranslator [options] [script.js]");
    console.log("Options:");
    console.log("-t: Translate to Gen Z code");
    console.log("-u: Translate from Gen Z code");
    console.log("-r: Run code");
    break;
  default:
    console.log("Usage: node genZtranslator [options] [script.js]");
    console.log("Options:");
    console.log("-t: Translate to Gen Z code");
    console.log("-u: Translate from Gen Z code");
    console.log("-r: Run code");
    break;
}