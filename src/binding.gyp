{
  "targets": [
    {
      "target_name": "openslide",
      "sources": [ "openslide.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
	"/usr/local/include"
      ],
      "libraries":[
        "/usr/local/Cellar/openslide/3.4.1_2/lib/libopenslide.dylib"
      ]
    }
  ]
}
