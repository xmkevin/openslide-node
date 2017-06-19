var addon = require('bindings')('openslide');

var obj = new addon.OpenSlideObject("/Users/gaoyongqing/Documents/projects/slideonly/openslide-node/data/CMU-1.svs");
var success = obj.open();
console.log("open result = ", success);

//var vendor = addon.detect_vendor("/Users/gaoyongqing/Documents/projects/slideonly/openslide-node/data/CMU-1.svs"); 
//console.log(vendor);

