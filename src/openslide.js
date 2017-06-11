var addon = require('bindings')('openslide');

console.log(addon.detect_vendor("~/Documents/projects/demo/openslide.db"));