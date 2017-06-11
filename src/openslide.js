var addon = require('bindings')('openslide');

var vendor = addon.detect_vendor("/Users/gaoyongqing/Documents/projects/slideonly/openslide-node/data/CMU-1.svs"); 
console.log(vendor);