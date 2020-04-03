{
    'conditions': [
        ['OS=="mac"', {
            'cflags+': ['-fvisibility=hidden'],
            'xcode_settings': {
                'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES'
            }
        }]
    ],
    'targets': [
        {
            'target_name': 'wvm',
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
            'cflags': ['-O3'],
            'sources': ['src/map.cc'],
        }
    ]
}
