{
  "targets": [
    {
      "target_name": "libseamcarving",
      "type": "shared_library",
      "sources": ["../seamcarvinglib/myseamcarving.h","../seamcarvinglib/myseamcarving.cpp","../seamcarvinglib/exchange.cpp","../seamcarvingapi.cpp"],
      "cflags": ["-Wall", "-std=c++11"],
      "include_dirs" : ['../seamcarvinglib'],
      "conditions": [
        [ 'OS=="mac"', {
            "xcode_settings": {
                'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                'OTHER_LDFLAGS': ['-stdlib=libc++'],
                'MACOSX_DEPLOYMENT_TARGET': '10.7' }
            }
        ]
      ]
    }
  ]
}
