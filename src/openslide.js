var addon = require('bindings')('openslide');

var obj = new addon.OpenSlideObject("/Users/gaoyongqing/Documents/projects/slideonly/openslide-node/data/CMU-1.svs");
var success = obj.open();
console.log("open result = " + success);

console.log("level Count = " + obj.levelCount);
console.log("level widths = ", obj.levelWidths);

//var vendor = addon.detect_vendor("/Users/gaoyongqing/Documents/projects/slideonly/openslide-node/data/CMU-1.svs"); 
//console.log(vendor);

