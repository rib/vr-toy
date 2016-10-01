{
  'target_defaults': {
    'cflags_c': [
      '-std=c11',
      '-Wno-sign-compare',
    ],
    'xcode_settings': {
      'WARNING_CFLAGS': [
        '-Wno-unused-parameter'
      ],
      'OTHER_LDFLAGS': [
      ],
      'OTHER_CFLAGS': [
        '-g',
        '-std=c11',
      ],
    }
  },

  'targets': [
    {
      'target_name': 'freetype',
      'type': 'shared_library',
      'dependencies': [
        'zlib/zlib.gyp:zlib'
       ],
      'include_dirs': [
          'freetype/include',
          'freetype/include/freetype',
          'freetype/objs',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'freetype/include',
          'freetype/include/freetype',
          'freetype/objs',
        ],
        'conditions': [
          [ 'OS!="win"', {
              'include_dirs': [
                'freetype/builds/unix'
              ]
          },{
              'include_dirs': [
                'freetype/include/freetype/config'
              ]
          }]
        ],
#'link_settings': {
#          'libraries': [ '-lz' ],
#        }
      },
#      'link_settings': {
#        'libraries': [ '-lzlib' ],
#      },
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
        'FT_CONFIG_CONFIG_H=<ftconfig.h>',
        'FT2_BUILD_LIBRARY',
        'FT_CONFIG_OPTION_SYSTEM_ZLIB',
      ],
      'sources': [
        'freetype/src/base/ftdebug.c',
        'freetype/src/base/ftinit.c',
        'freetype/src/base/ftbase.c',
        'freetype/src/base/ftbbox.c',
        'freetype/src/base/ftbdf.c',
        'freetype/src/base/ftbitmap.c',
        'freetype/src/base/ftcid.c',
        'freetype/src/base/ftfstype.c',
        'freetype/src/base/ftgasp.c',
        'freetype/src/base/ftglyph.c',
        'freetype/src/base/ftgxval.c',
        'freetype/src/base/ftlcdfil.c',
        'freetype/src/base/ftmm.c',
        'freetype/src/base/ftotval.c',
        'freetype/src/base/ftpatent.c',
        'freetype/src/base/ftpfr.c',
        'freetype/src/base/ftstroke.c',
        'freetype/src/base/ftsynth.c',
        'freetype/src/base/fttype1.c',
        'freetype/src/base/ftwinfnt.c',
        'freetype/src/base/ftxf86.c',
        'freetype/src/truetype/truetype.c',
        'freetype/src/type1/type1.c',
        'freetype/src/cff/cff.c',
        'freetype/src/cid/type1cid.c',
        'freetype/src/pfr/pfr.c',
        'freetype/src/type42/type42.c',
        'freetype/src/winfonts/winfnt.c',
        'freetype/src/pcf/pcf.c',
        'freetype/src/bdf/bdf.c',
        'freetype/src/sfnt/sfnt.c',
        'freetype/src/autofit/autofit.c',
        'freetype/src/pshinter/pshinter.c',
        'freetype/src/raster/raster.c',
        'freetype/src/smooth/smooth.c',
        'freetype/src/cache/ftcache.c',
        'freetype/src/gzip/ftgzip.c',
        'freetype/src/lzw/ftlzw.c',
        'freetype/src/psaux/psaux.c',
        'freetype/src/psnames/psmodule.c',
      ],
      'conditions': [
        [ 'OS!="win"', {
          'include_dirs': [
            'freetype/builds/unix'
          ],
          'sources': [
            'freetype/builds/unix/ftsystem.c'
          ]
        },{
          'include_dirs': [
            'freetype/include/freetype/config'
          ],
          'sources': [
            'freetype/src/base/ftsystem.c'
          ]
        }]
      ]
    },
    {
      'target_name': 'hb',
      'type': 'static_library',
      'dependencies': [
         'freetype',
         'icu/source/icu.gyp:icuuc'
       ],
      'include_dirs': [
        'harfbuzz/src',
        'harfbuzz/src/hb-ucdn'
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'harfbuzz/src'
        ],
      },
      'defines': [
        'HAVE_ATEXIT=1',
        'HAVE_FALLBACK=1',
        'HAVE_FONTCONFIG=1',
        'HAVE_FREETYPE=1',
        'HAVE_GETPAGESIZE=1',
        'HAVE_ICU=1',
        'HAVE_INTEL_ATOMIC_PRIMITIVES=1',
        'HAVE_ISATTY=1',
        'HAVE_OT=1',
        'HAVE_UCDN=1',
      ],
      'conditions': [
        [ 'OS=="win"', {
          # GYP 'cflags' are ignore on windows :-/ ...
          'msvs_settings': {
            'VCCLCompilerTool': {
              'AdditionalOptions': [
                '-FIintrin.h'
              ]
            }
          }
        },{ # !win
          'defines': [
            '_ALL_SOURCE=1',
            '_GNU_SOURCE=1',
            'STDC_HEADERS=1',
            'HAVE_MMAP=1',
            'HAVE_MPROTECT=1',
            'HAVE_PTHREAD=1',
            'HAVE_PTHREAD_PRIO_INHERIT=1',
            'HAVE_SYSCONF=1',
            'HAVE_SYS_MMAN_H=1',
            'HAVE_UNISTD_H=1',
          ]
        }]
      ],
      'libraries': [
      ],
      'sources': [
        'harfbuzz/src/hb-atomic-private.hh',
        'harfbuzz/src/hb-blob.cc',
        'harfbuzz/src/hb-buffer-deserialize-json.rl',
        'harfbuzz/src/hb-buffer-deserialize-json.hh',
        'harfbuzz/src/hb-buffer-deserialize-text.rl',
        'harfbuzz/src/hb-buffer-deserialize-text.hh',
        'harfbuzz/src/hb-buffer-private.hh',
        'harfbuzz/src/hb-buffer-serialize.cc',
        'harfbuzz/src/hb-buffer.cc',
        'harfbuzz/src/hb-cache-private.hh',
        'harfbuzz/src/hb-common.cc',
        'harfbuzz/src/hb-face-private.hh',
        'harfbuzz/src/hb-face.cc',
        'harfbuzz/src/hb-font-private.hh',
        'harfbuzz/src/hb-font.cc',
        'harfbuzz/src/hb-mutex-private.hh',
        'harfbuzz/src/hb-object-private.hh',
        'harfbuzz/src/hb-open-file-private.hh',
        'harfbuzz/src/hb-open-type-private.hh',
        'harfbuzz/src/hb-ot-cmap-table.hh',
        'harfbuzz/src/hb-ot-glyf-table.hh',
        'harfbuzz/src/hb-ot-head-table.hh',
        'harfbuzz/src/hb-ot-hhea-table.hh',
        'harfbuzz/src/hb-ot-hmtx-table.hh',
        'harfbuzz/src/hb-ot-maxp-table.hh',
        'harfbuzz/src/hb-ot-name-table.hh',
        'harfbuzz/src/hb-ot-tag.cc',
        'harfbuzz/src/hb-private.hh',
        'harfbuzz/src/hb-set-private.hh',
        'harfbuzz/src/hb-set.cc',
        'harfbuzz/src/hb-shape.cc',
        'harfbuzz/src/hb-shape-plan-private.hh',
        'harfbuzz/src/hb-shape-plan.cc',
        'harfbuzz/src/hb-shaper-list.hh',
        'harfbuzz/src/hb-shaper-impl-private.hh',
        'harfbuzz/src/hb-shaper-private.hh',
        'harfbuzz/src/hb-shaper.cc',
        'harfbuzz/src/hb-unicode-private.hh',
        'harfbuzz/src/hb-unicode.cc',
        'harfbuzz/src/hb-utf-private.hh',
        'harfbuzz/src/hb-warning.cc',

        'harfbuzz/src/hb-ot-font.cc',
        'harfbuzz/src/hb-ot-layout.cc',
        'harfbuzz/src/hb-ot-layout-common-private.hh',
        'harfbuzz/src/hb-ot-layout-gdef-table.hh',
        'harfbuzz/src/hb-ot-layout-gpos-table.hh',
        'harfbuzz/src/hb-ot-layout-gsubgpos-private.hh',
        'harfbuzz/src/hb-ot-layout-gsub-table.hh',
        'harfbuzz/src/hb-ot-layout-jstf-table.hh',
        'harfbuzz/src/hb-ot-layout-private.hh',
        'harfbuzz/src/hb-ot-map.cc',
        'harfbuzz/src/hb-ot-map-private.hh',
        'harfbuzz/src/hb-ot-shape.cc',
        'harfbuzz/src/hb-ot-shape-complex-arabic.cc',
        'harfbuzz/src/hb-ot-shape-complex-arabic-fallback.hh',
        'harfbuzz/src/hb-ot-shape-complex-arabic-private.hh',
        'harfbuzz/src/hb-ot-shape-complex-arabic-table.hh',
        'harfbuzz/src/hb-ot-shape-complex-arabic-win1256.hh',
        'harfbuzz/src/hb-ot-shape-complex-default.cc',
        'harfbuzz/src/hb-ot-shape-complex-hangul.cc',
        'harfbuzz/src/hb-ot-shape-complex-hebrew.cc',
        'harfbuzz/src/hb-ot-shape-complex-indic.cc',
        'harfbuzz/src/hb-ot-shape-complex-indic-machine.rl',
        'harfbuzz/src/hb-ot-shape-complex-indic-machine.hh',
        'harfbuzz/src/hb-ot-shape-complex-indic-private.hh',
        'harfbuzz/src/hb-ot-shape-complex-indic-table.cc',
        'harfbuzz/src/hb-ot-shape-complex-myanmar.cc',
        'harfbuzz/src/hb-ot-shape-complex-myanmar-machine.rl',
        'harfbuzz/src/hb-ot-shape-complex-myanmar-machine.hh',
        'harfbuzz/src/hb-ot-shape-complex-thai.cc',
        'harfbuzz/src/hb-ot-shape-complex-tibetan.cc',
        'harfbuzz/src/hb-ot-shape-complex-use.cc',
        'harfbuzz/src/hb-ot-shape-complex-use-machine.rl',
        'harfbuzz/src/hb-ot-shape-complex-use-machine.hh',
        'harfbuzz/src/hb-ot-shape-complex-use-private.hh',
        'harfbuzz/src/hb-ot-shape-complex-use-table.cc',
        'harfbuzz/src/hb-ot-shape-complex-private.hh',
        'harfbuzz/src/hb-ot-shape-normalize-private.hh',
        'harfbuzz/src/hb-ot-shape-normalize.cc',
        'harfbuzz/src/hb-ot-shape-fallback-private.hh',
        'harfbuzz/src/hb-ot-shape-fallback.cc',
        'harfbuzz/src/hb-ot-shape-private.hh',

        'harfbuzz/src/hb-fallback-shape.cc',

        'harfbuzz/src/hb-ft.cc',
        'harfbuzz/src/hb-ft.h',

        'harfbuzz/src/hb-icu.cc',
        'harfbuzz/src/hb-icu.h',

        'harfbuzz/src/hb-ucdn.cc',
        'harfbuzz/src/hb-ucdn/ucdn.h',
        'harfbuzz/src/hb-ucdn/ucdn.c',
        'harfbuzz/src/hb-ucdn/unicodedata_db.h',

      ],
      'rules': [
        {
          'rule_name': 'ragel',
	  'extension': 'rl',
	  'outputs': [ '<(RULE_INPUT_DIRNAME)/<(RULE_INPUT_ROOT).hh' ],
          'action': [ 'ragel', '-e', '-F1', '-o', '<@(_outputs)', '<(RULE_INPUT_PATH)' ]
        }
      ],
    },
    {
      'target_name': 'fc-glyphname',
      'type': 'executable',
      'toolsets': [ 'host' ],
      'include_dirs': [
        'fontconfig',
        'fontconfig/src'
      ],
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
        'FLEXIBLE_ARRAY_MEMBER=/**/',
      ],
      'sources': [
        'fontconfig/fc-glyphname/fc-glyphname.c',
      ],
    },
    {
      'target_name': 'fc-case',
      'type': 'executable',
      'toolsets': [ 'host' ],
      'include_dirs': [
        'fontconfig',
        'fontconfig/src'
      ],
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
        'FLEXIBLE_ARRAY_MEMBER=/**/',
      ],
      'sources': [
        'fontconfig/fc-case/fc-case.c',
      ],
    },
    {
      'target_name': 'fc-case-gen',
      'type': 'none',
      'toolsets': [ 'host', 'target' ],
      'dependencies': [ 'fc-case#host' ],
      'sources': [ 'fontconfig/fc-case/fccase.h' ],
      'actions': [
        {
          'action_name': 'fccase.h',
          'template': 'fontconfig/fc-case/fccase.tmpl.h',
          'casefolding': 'fontconfig/fc-case/CaseFolding.txt',
          'inputs': [ '<(_template)', '<(_casefolding)' ],
          'outputs': [ 'fontconfig/fc-case/fccase.h' ],
          'action': [ '<(PRODUCT_DIR)/fc-case<(EXECUTABLE_SUFFIX)', '<(_template)', '<@(_outputs)', '<(_casefolding)' ]
        },
      ]
    },
    {
      'target_name': 'fc-lang',
      'dependencies': [ 'fc-case-gen' ],
      'type': 'executable',
      'toolsets': [ 'host' ],
      'include_dirs': [
        'fontconfig',
        'fontconfig/src'
      ],
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
        'FLEXIBLE_ARRAY_MEMBER=/**/',
      ],
      'sources': [
        'fontconfig/fc-case/fccase.h',

        'fontconfig/fc-lang/fc-lang.c',
      ],
    },
    {
      'target_name': 'fontconfig',
      'type': 'static_library',
      'dependencies': [
        'freetype',
        'fc-glyphname#host',
        'fc-case#host',
        'fc-case-gen',
        'fc-lang#host',
        'libxml2/libxml.gyp:libxml'
       ],
      'include_dirs': [
        'fontconfig'
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'fontconfig'
        ],
      },
      'defines': [
        'USE_ICONV=0',
        'ENABLE_LIBXML2=1',
        'VERSION="2.11.93"',
        'FC_DEFAULT_FONTS="/usr/share/fonts"',
        'FC_ADD_FONTS="yes"',
        'FC_CACHEDIR="/usr/local/var/cache/fontconfig"',
        'FONTCONFIG_PATH="/usr/local/etc/fonts"',
        'FLEXIBLE_ARRAY_MEMBER=/**/',
        'HAVE_FT_BITMAP_SIZE_Y_PPEM=1',
        'HAVE_FT_GET_BDF_PROPERTY=1',
        'HAVE_FT_GET_NEXT_CHAR=1',
        'HAVE_FT_GET_PS_FONT_INFO=1',
        'HAVE_FT_GET_X11_FONT_FORMAT=1',
        'HAVE_FT_HAS_PS_GLYPH_NAMES=1',
        'HAVE_FT_SELECT_SIZE=1',
        'HAVE_STDINT_H=1',
        'HAVE_FCNTL_H=1',
        'HAVE_RAND=1',
#'HAVE_CONFIG_H=1',
      ],
      'cflags': [
        '-include config-fixups.h'
      ],
      'sources': [
        # Generated...
        'fontconfig/fc-glyphname/fcglyphname.h',
        'fontconfig/fc-glyphname/fccase.h',
        'fontconfig/fc-glyphname/fclang.h',

        'fontconfig/src/fcarch.h',
        'fontconfig/src/fcatomic.c',
        'fontconfig/src/fcatomic.h',
        'fontconfig/src/fcblanks.c',
        'fontconfig/src/fccache.c',
        'fontconfig/src/fccfg.c',
        'fontconfig/src/fccharset.c',
        'fontconfig/src/fccompat.c',
        'fontconfig/src/fcdbg.c',
        'fontconfig/src/fcdefault.c',
        'fontconfig/src/fcdir.c',
        'fontconfig/src/fcformat.c',
        'fontconfig/src/fcfreetype.c',
        'fontconfig/src/fcfs.c',
        'fontconfig/src/fcinit.c',
        'fontconfig/src/fclang.c',
        'fontconfig/src/fclist.c',
        'fontconfig/src/fcmatch.c',
        'fontconfig/src/fcmatrix.c',
        'fontconfig/src/fcmutex.h',
        'fontconfig/src/fcname.c',
        'fontconfig/src/fcobjs.c',
        'fontconfig/src/fcobjs.h',
        'fontconfig/src/fcobjshash.h',
        'fontconfig/src/fcpat.c',
        'fontconfig/src/fcrange.c',
        'fontconfig/src/fcserialize.c',
        'fontconfig/src/fcstat.c',
        'fontconfig/src/fcstr.c',
        'fontconfig/src/fcweight.c',
        'fontconfig/src/fcwindows.h',
        'fontconfig/src/fcxml.c',
        'fontconfig/src/ftglue.h',
        'fontconfig/src/ftglue.c',
      ],
      'conditions': [
        [ 'OS!="win"', {
          'defines': [
            '_ALL_SOURCE=1',
            '_GNU_SOURCE=1',
            'STDC_HEADERS=1',
            'HAVE_FCNTL_H=1',
            'HAVE_FSTATVFS=1',
            'HAVE_FSTATFS=1',
            'HAVE_PTHREAD=1',
            'HAVE_STRUCT_STATFS_F_FLAGS=1',
            'HAVE_GETOPT=1',
            'HAVE_GETOPT_LONG=1',
            'HAVE_LINK=1',
            'HAVE_LSTAT=1',
            'HAVE_MKDTEMP=1',
            'HAVE_MKOSTEMP=1',
            'HAVE_MKSTEMP=1',
            'HAVE_MMAP=1',
            'HAVE_POSIX_FADVISE=1',
            'HAVE_RAND=1',
            'HAVE_RAND_R=1',
            'HAVE_RANDOM=1',
            'HAVE_RANDOM_R=1',
            'HAVE_LRAND48=1',
            'HAVE_STRUCT_DIRENT_D_TYPE=1',
            'HAVE_SYS_MOUNT_H=1',
            'HAVE_SYS_PARAM_H=1',
            'HAVE_SYS_STATFS_H=1',
            'HAVE_SYS_STATVFS_H=1',
            'HAVE_SYS_STAT_H=1',
            'HAVE_SYS_TYPES_H=1',
            'HAVE_SYS_VFS_H=1',
            'HAVE_WARNING_CPP_DIRECTIVE=1',
            'HAVE_INTEL_ATOMIC_PRIMITIVES=1',
          ]
        },{
          'defines': [
            'HAVE_WARNING_CPP_DIRECTIVE=1',
            'HAVE_INTEL_ATOMIC_PRIMITIVES=1',
            'close=_close'
          ],
          'sources': [
            'win/dirent.h'
          ],
          'include_dirs': [
            'fontconfig/win'
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
                'AdditionalOptions': [
                    '/FIconfig-fixups.h',
                    '/FIintrin.h'
                ]
            }
          }
        }]
      ],
      'actions': [
        {
          'action_name': 'fcblanks.h',
          'inputs': [ 'fontconfig/fc-blanks/fcblanks.tmpl.h' ],
          'outputs': [ 'fontconfig/fc-blanks/fcblanks.h' ],
          'action': [ 'python', 'fontconfig/fc-blanks/fc-blanks.py', '<@(_inputs)', '<@(_outputs)' ]
        },
        {
          'action_name': 'fcglyphname.h',
          'inputs': [ 'fontconfig/fc-glyphname/fcglyphname.tmpl.h' ],
          'outputs': [ 'fontconfig/fc-glyphname/fcglyphname.h' ],
          'action': [ '<(PRODUCT_DIR)/fc-glyphname<(EXECUTABLE_SUFFIX)', '<@(_inputs)', '<@(_outputs)' ]
        },
        {
          'action_name': 'fclang.h',
          'ortho': [
	    'fontconfig/fc-lang/aa.orth',
	    'fontconfig/fc-lang/ab.orth',
	    'fontconfig/fc-lang/af.orth',
	    'fontconfig/fc-lang/am.orth',
	    'fontconfig/fc-lang/ar.orth',
	    'fontconfig/fc-lang/as.orth',
	    'fontconfig/fc-lang/ast.orth',
	    'fontconfig/fc-lang/av.orth',
	    'fontconfig/fc-lang/ay.orth',
	    'fontconfig/fc-lang/az_az.orth',
	    'fontconfig/fc-lang/az_ir.orth',
	    'fontconfig/fc-lang/ba.orth',
	    'fontconfig/fc-lang/bm.orth',
	    'fontconfig/fc-lang/be.orth',
	    'fontconfig/fc-lang/bg.orth',
	    'fontconfig/fc-lang/bh.orth',
	    'fontconfig/fc-lang/bho.orth',
	    'fontconfig/fc-lang/bi.orth',
	    'fontconfig/fc-lang/bin.orth',
	    'fontconfig/fc-lang/bn.orth',
	    'fontconfig/fc-lang/bo.orth',
	    'fontconfig/fc-lang/br.orth',
	    'fontconfig/fc-lang/bs.orth',
	    'fontconfig/fc-lang/bua.orth',
	    'fontconfig/fc-lang/ca.orth',
	    'fontconfig/fc-lang/ce.orth',
	    'fontconfig/fc-lang/ch.orth',
	    'fontconfig/fc-lang/chm.orth',
	    'fontconfig/fc-lang/chr.orth',
	    'fontconfig/fc-lang/co.orth',
	    'fontconfig/fc-lang/cs.orth',
	    'fontconfig/fc-lang/cu.orth',
	    'fontconfig/fc-lang/cv.orth',
	    'fontconfig/fc-lang/cy.orth',
	    'fontconfig/fc-lang/da.orth',
	    'fontconfig/fc-lang/de.orth',
	    'fontconfig/fc-lang/dz.orth',
	    'fontconfig/fc-lang/el.orth',
	    'fontconfig/fc-lang/en.orth',
	    'fontconfig/fc-lang/eo.orth',
	    'fontconfig/fc-lang/es.orth',
	    'fontconfig/fc-lang/et.orth',
	    'fontconfig/fc-lang/eu.orth',
	    'fontconfig/fc-lang/fa.orth',
	    'fontconfig/fc-lang/fi.orth',
	    'fontconfig/fc-lang/fj.orth',
	    'fontconfig/fc-lang/fo.orth',
	    'fontconfig/fc-lang/fr.orth',
	    'fontconfig/fc-lang/ff.orth',
	    'fontconfig/fc-lang/fur.orth',
	    'fontconfig/fc-lang/fy.orth',
	    'fontconfig/fc-lang/ga.orth',
	    'fontconfig/fc-lang/gd.orth',
	    'fontconfig/fc-lang/gez.orth',
	    'fontconfig/fc-lang/gl.orth',
	    'fontconfig/fc-lang/gn.orth',
	    'fontconfig/fc-lang/gu.orth',
	    'fontconfig/fc-lang/gv.orth',
	    'fontconfig/fc-lang/ha.orth',
	    'fontconfig/fc-lang/haw.orth',
	    'fontconfig/fc-lang/he.orth',
	    'fontconfig/fc-lang/hi.orth',
	    'fontconfig/fc-lang/ho.orth',
	    'fontconfig/fc-lang/hr.orth',
	    'fontconfig/fc-lang/hu.orth',
	    'fontconfig/fc-lang/hy.orth',
	    'fontconfig/fc-lang/ia.orth',
	    'fontconfig/fc-lang/ig.orth',
	    'fontconfig/fc-lang/id.orth',
	    'fontconfig/fc-lang/ie.orth',
	    'fontconfig/fc-lang/ik.orth',
	    'fontconfig/fc-lang/io.orth',
	    'fontconfig/fc-lang/is.orth',
	    'fontconfig/fc-lang/it.orth',
	    'fontconfig/fc-lang/iu.orth',
	    'fontconfig/fc-lang/ja.orth',
	    'fontconfig/fc-lang/ka.orth',
	    'fontconfig/fc-lang/kaa.orth',
	    'fontconfig/fc-lang/ki.orth',
	    'fontconfig/fc-lang/kk.orth',
	    'fontconfig/fc-lang/kl.orth',
	    'fontconfig/fc-lang/km.orth',
	    'fontconfig/fc-lang/kn.orth',
	    'fontconfig/fc-lang/ko.orth',
	    'fontconfig/fc-lang/kok.orth',
	    'fontconfig/fc-lang/ks.orth',
	    'fontconfig/fc-lang/ku_am.orth',
	    'fontconfig/fc-lang/ku_ir.orth',
	    'fontconfig/fc-lang/kum.orth',
	    'fontconfig/fc-lang/kv.orth',
	    'fontconfig/fc-lang/kw.orth',
	    'fontconfig/fc-lang/ky.orth',
	    'fontconfig/fc-lang/la.orth',
	    'fontconfig/fc-lang/lb.orth',
	    'fontconfig/fc-lang/lez.orth',
	    'fontconfig/fc-lang/ln.orth',
	    'fontconfig/fc-lang/lo.orth',
	    'fontconfig/fc-lang/lt.orth',
	    'fontconfig/fc-lang/lv.orth',
	    'fontconfig/fc-lang/mg.orth',
	    'fontconfig/fc-lang/mh.orth',
	    'fontconfig/fc-lang/mi.orth',
	    'fontconfig/fc-lang/mk.orth',
	    'fontconfig/fc-lang/ml.orth',
	    'fontconfig/fc-lang/mn_cn.orth',
	    'fontconfig/fc-lang/mo.orth',
	    'fontconfig/fc-lang/mr.orth',
	    'fontconfig/fc-lang/mt.orth',
	    'fontconfig/fc-lang/my.orth',
	    'fontconfig/fc-lang/nb.orth',
	    'fontconfig/fc-lang/nds.orth',
	    'fontconfig/fc-lang/ne.orth',
	    'fontconfig/fc-lang/nl.orth',
	    'fontconfig/fc-lang/nn.orth',
	    'fontconfig/fc-lang/no.orth',
	    'fontconfig/fc-lang/nr.orth',
	    'fontconfig/fc-lang/nso.orth',
	    'fontconfig/fc-lang/ny.orth',
	    'fontconfig/fc-lang/oc.orth',
	    'fontconfig/fc-lang/om.orth',
	    'fontconfig/fc-lang/or.orth',
	    'fontconfig/fc-lang/os.orth',
	    'fontconfig/fc-lang/pa.orth',
	    'fontconfig/fc-lang/pl.orth',
	    'fontconfig/fc-lang/ps_af.orth',
	    'fontconfig/fc-lang/ps_pk.orth',
	    'fontconfig/fc-lang/pt.orth',
	    'fontconfig/fc-lang/rm.orth',
	    'fontconfig/fc-lang/ro.orth',
	    'fontconfig/fc-lang/ru.orth',
	    'fontconfig/fc-lang/sa.orth',
	    'fontconfig/fc-lang/sah.orth',
	    'fontconfig/fc-lang/sco.orth',
	    'fontconfig/fc-lang/se.orth',
	    'fontconfig/fc-lang/sel.orth',
	    'fontconfig/fc-lang/sh.orth',
	    'fontconfig/fc-lang/shs.orth',
	    'fontconfig/fc-lang/si.orth',
	    'fontconfig/fc-lang/sk.orth',
	    'fontconfig/fc-lang/sl.orth',
	    'fontconfig/fc-lang/sm.orth',
	    'fontconfig/fc-lang/sma.orth',
	    'fontconfig/fc-lang/smj.orth',
	    'fontconfig/fc-lang/smn.orth',
	    'fontconfig/fc-lang/sms.orth',
	    'fontconfig/fc-lang/so.orth',
	    'fontconfig/fc-lang/sq.orth',
	    'fontconfig/fc-lang/sr.orth',
	    'fontconfig/fc-lang/ss.orth',
	    'fontconfig/fc-lang/st.orth',
	    'fontconfig/fc-lang/sv.orth',
	    'fontconfig/fc-lang/sw.orth',
	    'fontconfig/fc-lang/syr.orth',
	    'fontconfig/fc-lang/ta.orth',
	    'fontconfig/fc-lang/te.orth',
	    'fontconfig/fc-lang/tg.orth',
	    'fontconfig/fc-lang/th.orth',
	    'fontconfig/fc-lang/ti_er.orth',
	    'fontconfig/fc-lang/ti_et.orth',
	    'fontconfig/fc-lang/tig.orth',
	    'fontconfig/fc-lang/tk.orth',
	    'fontconfig/fc-lang/tl.orth',
	    'fontconfig/fc-lang/tn.orth',
	    'fontconfig/fc-lang/to.orth',
	    'fontconfig/fc-lang/tr.orth',
	    'fontconfig/fc-lang/ts.orth',
	    'fontconfig/fc-lang/tt.orth',
	    'fontconfig/fc-lang/tw.orth',
	    'fontconfig/fc-lang/tyv.orth',
	    'fontconfig/fc-lang/ug.orth',
	    'fontconfig/fc-lang/uk.orth',
	    'fontconfig/fc-lang/ur.orth',
	    'fontconfig/fc-lang/uz.orth',
	    'fontconfig/fc-lang/ve.orth',
	    'fontconfig/fc-lang/vi.orth',
	    'fontconfig/fc-lang/vo.orth',
	    'fontconfig/fc-lang/vot.orth',
	    'fontconfig/fc-lang/wa.orth',
	    'fontconfig/fc-lang/wen.orth',
	    'fontconfig/fc-lang/wo.orth',
	    'fontconfig/fc-lang/xh.orth',
	    'fontconfig/fc-lang/yap.orth',
	    'fontconfig/fc-lang/yi.orth',
	    'fontconfig/fc-lang/yo.orth',
	    'fontconfig/fc-lang/zh_cn.orth',
	    'fontconfig/fc-lang/zh_hk.orth',
	    'fontconfig/fc-lang/zh_mo.orth',
	    'fontconfig/fc-lang/zh_sg.orth',
	    'fontconfig/fc-lang/zh_tw.orth',
	    'fontconfig/fc-lang/zu.orth',
	    'fontconfig/fc-lang/ak.orth',
	    'fontconfig/fc-lang/an.orth',
	    'fontconfig/fc-lang/ber_dz.orth',
	    'fontconfig/fc-lang/ber_ma.orth',
	    'fontconfig/fc-lang/byn.orth',
	    'fontconfig/fc-lang/crh.orth',
	    'fontconfig/fc-lang/csb.orth',
	    'fontconfig/fc-lang/dv.orth',
	    'fontconfig/fc-lang/ee.orth',
	    'fontconfig/fc-lang/fat.orth',
	    'fontconfig/fc-lang/fil.orth',
	    'fontconfig/fc-lang/hne.orth',
	    'fontconfig/fc-lang/hsb.orth',
	    'fontconfig/fc-lang/ht.orth',
	    'fontconfig/fc-lang/hz.orth',
	    'fontconfig/fc-lang/ii.orth',
	    'fontconfig/fc-lang/jv.orth',
	    'fontconfig/fc-lang/kab.orth',
	    'fontconfig/fc-lang/kj.orth',
	    'fontconfig/fc-lang/kr.orth',
	    'fontconfig/fc-lang/ku_iq.orth',
	    'fontconfig/fc-lang/ku_tr.orth',
	    'fontconfig/fc-lang/kwm.orth',
	    'fontconfig/fc-lang/lg.orth',
	    'fontconfig/fc-lang/li.orth',
	    'fontconfig/fc-lang/mai.orth',
	    'fontconfig/fc-lang/mn_mn.orth',
	    'fontconfig/fc-lang/ms.orth',
	    'fontconfig/fc-lang/na.orth',
	    'fontconfig/fc-lang/ng.orth',
	    'fontconfig/fc-lang/nv.orth',
	    'fontconfig/fc-lang/ota.orth',
	    'fontconfig/fc-lang/pa_pk.orth',
	    'fontconfig/fc-lang/pap_an.orth',
	    'fontconfig/fc-lang/pap_aw.orth',
	    'fontconfig/fc-lang/qu.orth',
	    'fontconfig/fc-lang/quz.orth',
	    'fontconfig/fc-lang/rn.orth',
	    'fontconfig/fc-lang/rw.orth',
	    'fontconfig/fc-lang/sc.orth',
	    'fontconfig/fc-lang/sd.orth',
	    'fontconfig/fc-lang/sg.orth',
	    'fontconfig/fc-lang/sid.orth',
	    'fontconfig/fc-lang/sn.orth',
	    'fontconfig/fc-lang/su.orth',
	    'fontconfig/fc-lang/ty.orth',
	    'fontconfig/fc-lang/wal.orth',
	    'fontconfig/fc-lang/za.orth',
	    'fontconfig/fc-lang/lah.orth',
	    'fontconfig/fc-lang/nqo.orth',
	    'fontconfig/fc-lang/brx.orth',
	    'fontconfig/fc-lang/sat.orth',
	    'fontconfig/fc-lang/doi.orth',
  	    'fontconfig/fc-lang/mni.orth',
          ],
          'template': 'fontconfig/fc-lang/fclang.tmpl.h',
          'inputs': [ '<(_template)', '<@(_ortho)' ],
          'outputs': [ 'fontconfig/fc-lang/fclang.h' ],
          'action': [ '<(PRODUCT_DIR)/fc-lang<(EXECUTABLE_SUFFIX)', '<(_template)', '<@(_outputs)', '-d', 'fontconfig/fc-lang', '<@(_ortho)' ]
        }
      ],
    },
    {
      'target_name': 'xdgmime',
      'type': 'static_library',
      'dependencies': [
         'clib/clib.gyp:clib',
       ],
      'include_dirs': [
          'xdgmime'
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'xdgmime'
        ],
      },
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
      ],
      'sources': [
        'xdgmime/glob.c',
        'xdgmime/glob.h',
        'xdgmime/magic.c',
        'xdgmime/magic.h',
        'xdgmime/xdgmime-query.c',
        'xdgmime/xdgmime.c',
        'xdgmime/xdgmime.h',
      ],

      'conditions': [
        [ 'is_nodejs_build!=1', {
          'dependencies': [
            'libuv/uv.gyp:libuv'
          ],
        }],
        [ 'OS=="win"', {
          'link_settings': {
            'libraries': [
              '-lShlwapi'
            ]
          }
        }]
      ]
    },
    {
      'target_name': 'rig',
      'type': 'static_library',
      'dependencies': [
         'clib/clib.gyp:clib',
         'cglib/cglib.gyp:cglib',
         'xdgmime',
         'freetype',
         'fontconfig',
         'icu/source/icu.gyp:icuuc',
         'icu/source/icu.gyp:icudata',
         'hb',
         'protobuf-c/protobuf-c.gyp:protoc-c#host',
         'protobuf-c/protobuf-c.gyp:protobuf-c',
         'test-fixtures/test-fixtures.gyp:libtest-fixtures',
         #'LibOVR/libovr.gyp:libovr',
         'wslay/wslay.gyp:libwslay',
         'h2o/h2o.gyp:libh2o',
         'nsgif/nsgif.gyp:nsgif',
       ],
      'include_dirs': [
        '.',
        'rut',
        'rig',
        'rig/protobuf-c-rpc',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'rut',
          'rig',
        ],
      },
      'actions': [
        {
          'action_name': 'rig-proto',
          'inputs': [
            'rig/rig.proto'
          ],
          'outputs': [
            'rig/rig.pb-c.h',
            'rig/rig.pb-c.c'
          ],
          'action': [ '<(PRODUCT_DIR)/protoc-c<(EXECUTABLE_SUFFIX)', '--c_out=rig', '-Irig', '<@(_inputs)' ]
        }
      ],
      'sources': [

        'rut/rut.h',
        'rut/rut-types.h',
        'rut/rut-keysyms.h',
        'rut/color-table.h',
        'rut/rut-exception.h',
        'rut/rut-exception.c',
        'rut/rut-os.h',
        'rut/rut-os.c',
        'rut/rut-color.h',
        'rut/rut-color.c',
        'rut/rut-type.h',
        'rut/rut-type.c',
        'rut/rut-object.h',
        'rut/rut-object.c',
        'rut/rut-interfaces.h',
        'rut/rut-interfaces.c',
        'rut/rut-graphable.h',
        'rut/rut-graphable.c',
        'rut/rut-matrix-stack.h',
        'rut/rut-matrix-stack.c',
        'rut/rut-transform-private.h',
        'rut/rut-shell.h',
        'rut/rut-shell.c',
        'rut/rut-headless-shell.h',
        'rut/rut-headless-shell.c',
        'rut/rut-settings.h',
        'rut/rut-settings.c',
        'rut/rut-texture-cache.h',
        'rut/rut-texture-cache.c',
        'rut/rut-bitmask.h',
        'rut/rut-bitmask.c',
        'rut/rut-flags.h',
        'rut/rut-memory-stack.h',
        'rut/rut-memory-stack.c',
        'rut/rut-magazine.h',
        'rut/rut-magazine.c',
        'rut/rut-queue.h',
        'rut/rut-queue.c',
        'rut/rut-util.h',
        'rut/rut-util.c',
        'rut/rut-geometry.h',
        'rut/rut-geometry.c',
        'rut/rut-closure.h',
        'rut/rut-closure.c',
        'rut/rut-gaussian-blurrer.h',
        'rut/rut-gaussian-blurrer.c',
        'rut/rut-mesh.h',
        'rut/rut-mesh.c',
#'rut/rply.c',
#        'rut/rply.h',
        'rut/rut-mesh-ply.h',
        'rut/rut-mesh-ply.c',
        'rut/rut-graph.h',
        'rut/rut-graph.c',
        'rut/rut-refcount-debug.h',
        'rut/rut-refcount-debug.c',
        'rut/rut-mimable.h',
        'rut/rut-mimable.c',
        'rut/rut-input-region.h',
        'rut/rut-input-region.c',
        'rut/rut-camera.h',
        'rut/rut-camera.c',
        'rut/rut-poll.h',
        'rut/rut-poll.c',
        'rut/rut-inputable.h',
        'rut/rut-meshable.h',
        'rut/rut-gradient.h',
        'rut/rut-gradient.c',
        'rut/edid-parse.h',
        'rut/edid-parse.c',

        'rig/protobuf-c-rpc/rig-protobuf-c-stream.h',
        'rig/protobuf-c-rpc/rig-protobuf-c-stream.c',
        'rig/protobuf-c-rpc/rig-protobuf-c-rpc.h',
        'rig/protobuf-c-rpc/rig-protobuf-c-rpc.c',

        'rig/components/rig-camera.h',
        'rig/components/rig-camera.c',
        'rig/components/rig-light.h',
        'rig/components/rig-light.c',
        'rig/components/rig-mesh.h',
        'rig/components/rig-mesh.c',
        'rig/components/rig-source.h',
        'rig/components/rig-source.c',
        'rig/components/rig-material.h',
        'rig/components/rig-material.c',
        'rig/components/rig-nine-slice.h',
        'rig/components/rig-nine-slice.c',
        'rig/components/rig-button-input.h',
        'rig/components/rig-button-input.c',
        'rig/components/rig-text.h',
        'rig/components/rig-text.c',
        'rig/rig-logs.h',
        'rig/rig-logs.c',
        'rig/rig-property.h',
        'rig/rig-property.c',
        'rig/rig-introspectable.h',
        'rig/rig-introspectable.c',
        'rig/rig-entity.h',
        'rig/rig-entity.c',
        'rig/rig-asset.h',
        'rig/rig-asset.c',
        'rig/rig-downsampler.h',
        'rig/rig-downsampler.c',
        'rig/rig-dof-effect.h',
        'rig/rig-dof-effect.c',
        'rig/rig-engine.c',
        'rig/rig-engine-op.h',
        'rig/rig-engine-op.c',
        'rig/rig-timeline.h',
        'rig/rig-timeline.c',
        'rig/rig-node.c',
        'rig/rig-node.h',
        'rig/rig-path.c',
        'rig/rig-path.h',
        'rig/rig-controller.c',
        'rig/rig-controller.h',
        'rig/rig-binding.h',
        'rig/rig-binding.c',
        'rig/rig-pb.h',
        'rig/rig-pb.c',
        'rig/rig-load-save.h',
        'rig/rig-load-save.c',
        'rig/rig-camera-view.h',
        'rig/rig-camera-view.c',
        'rig/rig-types.h',
        'rig/rut-renderer.h',
        'rig/rut-renderer.c',
        'rig/rig-renderer.h',
        'rig/rig-renderer.c',
        'rig/rig-engine.h',
        'rig/rig-rpc-network.h',
        'rig/rig-rpc-network.c',
        'rig/rig-slave-address.h',
        'rig/rig-slave-address.c',
        'rig/rig-simulator.h',
        'rig/rig-simulator-impl.c',
        'rig/rig-frontend.c',
        'rig/rig-frontend.h',
        'rig/rig-ui.h',
        'rig/rig-ui.c',
        'rig/rig-code.h',
        'rig/rig-code.c',
        'rig/rig-code-module.h',
        'rig/rig-code-module.c',
        'rig/rig-c.h',
        'rig/rig-c.c',
        'rig/rig-c-mesh.h',
        'rig/rig-c-mesh.c',
        'rig/usc_impl.h',
        'rig/usc_impl.c',
        'rig/rig-text-pipeline-cache.h',
        'rig/rig-text-pipeline-cache.c',
        'rig/rig-text-engine-funcs.h',
        'rig/rig-text-engine-funcs.c',
        'rig/rig-text-engine.h',
        'rig/rig-text-engine-private.h',
        'rig/rig-text-engine.c',
        'rig/rig-text-renderer.h',
        'rig/rig-text-renderer.c',

        'rig/rig.pb-c.h',
        'rig/rig.pb-c.c',
      ],
      'conditions': [
        [ 'OS=="win"', {
          'sources': [
            'rut/rut-win-shell.h',
            'rut/rut-win-shell.c'
          ],
          'include_dirs': [
            'C:/Users/Robert/local/ffmpeg-cl/include'
          ],
          'link_settings': {
            'libraries': [
              '-lavcodec.lib',
              '-lavformat.lib',
              '-lavutil.lib',
              '-lswscale.lib',
              '-lswresample.lib',
            ],
            'library_dirs': [
              'C:/Users/Robert/local/ffmpeg-cl/bin' # HACK
            ],
          }
        }, { # Not Windows i.e. POSIX
          'cflags': [
            '-g',
            '-std=c99',
            '-Wno-unused-parameter',
          ],
          'link_settings': {
            'libraries': [ '-lm' ],
            'conditions': [
              ['OS != "android"', {
                'ldflags': [ '-pthread' ],
              }],
            ],
          },
          'conditions': [
            ['_type=="shared_library"', {
              'cflags': [ '-fPIC' ],
              'defines': [
                'ENABLE_UNIT_TESTS'
              ],
            }],
          ],
        }],
        [ 'OS=="emscripten"', {
          'sources': [
          ],
        }],
        [ 'OS=="android"', {
          'sources': [
          ],
        }],
        [ 'OS=="mac"', {
          'defines': [
            '_DARWIN_USE_64_BIT_INODE=1'
          ],
        }],
        [ 'OS!="mac"', {
          # Enable on all platforms except OS X. The antique gcc/clang that
          # ships with Xcode emits waaaay too many false positives.
          'cflags': [ '-Wstrict-aliasing' ],
        }],
        [ 'enable_uv==1 and is_nodejs_build!=1', {
          'dependencies': [
            'libuv/uv.gyp:libuv'
          ],
        }],
        [ 'enable_uv==1', {
          'defines': [ 'USE_UV=1' ],
          'sources': [
            'rig/components/rig-native-module.h',
            'rig/components/rig-native-module.c',
          ]
        }],
        [ 'enable_glib==1', {
          'sources': [
          ]
        }],
        [ 'enable_sdl==1', {
          'sources': [
          ]
        }],
        [ 'enable_x11==1', {
          'sources': [
            'rut/rut-x11-shell.h',
            'rut/rut-x11-shell.c'
          ]
        }],
#[ 'enable_oculus_rift==1', {
#          'include_dirs': [
#            'LibOVR/Src'
#          ]
#        }],
        [ 'enable_alsa==1', {
          'defines': [ 'USE_ALSA=1' ],
          'sources': [
            'rut/rut-alsa-shell.h',
            'rut/rut-alsa-shell.c',
          ]
        }],
        [ 'enable_ncurses==1', {
          'sources': [
            'rig/rig-curses-debug.h',
            'rig/rig-curses-debug.c'
          ]
        }],
      ]
    },
    {
      'target_name': 'rig-hello',
      'type': 'executable',
      'dependencies': [
        'rig',
      ],
      'defines': [
        '_ALL_SOURCE=1',
        '_GNU_SOURCE=1',
      ],
      'sources': [
        'toys/rig-hello.c'
      ],
      'ldflags': [
        '-export-dynamic'
      ]
    }

  ]
}
