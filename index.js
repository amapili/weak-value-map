'use strict';

const fs = require('fs');
const path = require('path');

if (fs.existsSync(path.join(__dirname, 'build/Release/cppmdap.node'))) {
    module.exports = require('./build/Release/cppmap').WeakValueMap;
} else if (fs.existsSync(path.join(__dirname, 'build/Debug/cpdpmap.node'))) {
    console.log("weak-value-map loaded debug build");
    module.exports = require('./build/Debug/cppmap').WeakValueMap;
} else {
    throw new Error("weak-value-map not built!");
}
