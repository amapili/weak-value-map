'use strict';

const fs = require("fs");

if (fs.existsSync('./build/Release/cppmap.node')) {
    module.exports = require('./build/Release/cppmap').WeakValueMap;
} else if (fs.existsSync('./build/Debug/cppmap.node')) {
    console.log("weak-value-map loaded debug build");
    module.exports = require('./build/Debug/cppmap').WeakValueMap;
} else {
    console.error("weak-value-map not built!");
    process.exit(1);
}
