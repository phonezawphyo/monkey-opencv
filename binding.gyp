{
  "targets": [ 
    { 
      "target_name": "fast_template_matcher",

      "sources": [
         "src/addon.cc",
         "src/template_matcher.cc",
         "src/addon.cc",
         "src/async.cc",
         "src/contours.cc",
         "src/matrix.cc",
         "src/opencv.cc",
       ],

      "include_dirs": [ "<!(node -e \"require('nan')\")" ],

      # For windows

      "include_dirs": [
        "<!@(node utils/find-opencv.js --cflags)",
        "<!(node -e \"require('nan')\")"
      ],

      "cflags!" : [ "-fno-exceptions"],
      "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"],

      "conditions": [
        [ "OS==\"linux\" or OS==\"freebsd\" or OS==\"openbsd\" or OS==\"solaris\" or OS==\"aix\"", {
            "libraries": [
              "<!@(node utils/find-opencv.js --libs)",
            ],

            "cflags": [
              "<!@(node utils/find-opencv.js --cflags)",
              "-Wall"
            ]
        }],
        [ "OS==\"win\"", {
            "libraries": [
              "<!@(node utils/find-opencv.js --libs)",
            ],

            "cflags": [
              "-Wall"
            ],
            "defines": [
                "WIN"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": "2",
                "DisableSpecificWarnings": [ "4530", "4506", "4244" ],
              },
            }
        }],
        [ # cflags on OS X are stupid and have to be defined like this
          "OS==\"mac\"", {

            "libraries": [
              "<!@(node utils/find-opencv.js --libs | sed -e \"s/-l-framework/-framework/g\")",
            ],

            "xcode_settings": {
              "OTHER_CFLAGS": [
                "-mmacosx-version-min=10.7",
                "-std=c++11",
                "-stdlib=libc++",
                "<!@(node utils/find-opencv.js --cflags)",
              ],
              #"OTHER_LDFLAGS":[
              #  "-Wl,-bind_at_load"
              #],
              "GCC_ENABLE_CPP_RTTI": "YES",
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",

              "MACOSX_DEPLOYMENT_TARGET":"10.8",
              "CLANG_CXX_LIBRARY": "libc++",
              "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
              "GCC_VERSION": "com.apple.compilers.llvm.clang.1_0"
            },
          }
        ]
      ]
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [ "<(module_name)" ],
      "copies": [
      {
        "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
        "destination": "<(module_path)"
      }
      ]
    } 
  ]
}
