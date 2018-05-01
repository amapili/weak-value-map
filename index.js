'use strict';

const fs = require('fs');
const path = require('path');

if (fs.existsSync(path.join(__dirname, 'build/Release/weak-value-map.node'))) {
    module.exports = require('./build/Release/weak-value-map').WeakValueMap;
} else if (fs.existsSync(path.join(__dirname, 'build/Debug/weak-value-map.node'))) {
    console.log("weak-value-map loaded debug build");
    module.exports = require('./build/Debug/weak-value-map').WeakValueMap;
} else {
    throw new Error("weak-value-map not built!");
}
