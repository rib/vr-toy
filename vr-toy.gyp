{
  'target_defaults': {
    'cflags_c': [
      '-std=c11',
    ],
  },

  'targets': [
    {
      'target_name': 'vr-toy',
      'type': 'executable',
      'dependencies': [
	'rig/rig.gyp:rig',
      ],
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
      ],
      'sources': [
	'vr-toy.c'
      ],
      'ldflags': [
        '-export-dynamic'
      ]
    }
  ]
}
