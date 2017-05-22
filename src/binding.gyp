{
  "targets": [
    {
      "target_name": "openslide",
      "sources": [ "openslide.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
	"/usr/local/include"
      ]
    }
  ]
}
