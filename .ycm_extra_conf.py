
# Copyright 2015 the V8 project authors. All rights reserved.
# Copyright 2015 Robert Bragg
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of Google Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#
# VIM YouCompleteMe config for projects built with Ninja
#
# This script is used by the YouCompleteMe plugin to determine what
# cflags to pass to libclang when parsing files.
#
#
# USAGE:
#
#   1. Install YCM [https://github.com/Valloric/YouCompleteMe]
#
#   2. For best results configure Ninja to build with clang so
#      compiler flags are most likely to be compatible with the
#      version of clang used by YouCompleteMe.
#
#   3. Save this .ycm_extra_conf.py file at the top of your repo
#
# NOTES:
#
#   The script assumes your project uses Git and looks for a .git
#   directory to find the project root.
#
#   The script queries Ninja as if building under 'out/Debug'
#   which may affect the determined compiler flags. (The out_dir
#   variable below can be changed if necessary)
#
# Hacking notes:
#
#   * The purpose of this script is to construct an accurate enough
#     command line for YCM to pass to clang so it can build and extract
#     the symbols.
#
#   * Right now, we only pull the -I and -D flags. That seems to be
#     sufficient for everything I've used it for.
#

import os
import os.path
import subprocess
import sys


default_cflags = [
'-DUSE_CLANG_COMPLETER',
'-std=c11',
]

out_dir = 'out/Debug'

def PathExists(*args):
  return os.path.exists(os.path.join(*args))


def FindTopSrcFromFilename(filename):
  """Searches for the root of the checkout.

  Simply checks parent directories until it finds .git

  Args:
    filename: (String) Path to source file being edited.

  Returns:
    (String) Path containing .git
  """
  curdir = os.path.normpath(os.path.dirname(filename))
  while not PathExists(curdir, '.git'):
    nextdir = os.path.normpath(os.path.join(curdir, '..'))
    if nextdir == curdir:
      return None
    curdir = nextdir
  return curdir


def GetClangCommandFromNinjaForFilename(top_srcdir, filename):
  """Returns the command line to build |filename|.

  Asks ninja how it would build the source file. If the specified file is a
  header, tries to find its companion source file first.

  Args:
    top_srcdir: (String) Path to v8/.
    filename: (String) Path to source file being edited.

  Returns:
    (List of Strings) Command line arguments for clang.
  """
  cflags = []

  if not top_srcdir:
    return cflags

  # Version of Clang used to compile can be newer then version of libclang that
  # YCM uses for completion. So it's possible that YCM's libclang doesn't know
  # about some used warning options, which causes compilation warnings (and
  # errors, because of '-Werror');
  cflags.append('-Wno-unknown-warning-option')

  # Header files can't be built. Instead, try to match a header file to its
  # corresponding source file.
  if filename.endswith('.h'):
    alternates = ['.cc', '.cpp']
    for alt_extension in alternates:
      alt_name = filename[:-2] + alt_extension
      if os.path.exists(alt_name):
        filename = alt_name
        break
    else:
      if filename.endswith('-inl.h'):
        for alt_extension in alternates:
          alt_name = filename[:-6] + alt_extension
          if os.path.exists(alt_name):
            filename = alt_name
            break;
        else:
          # If this is a standalone -inl.h file with no source, the best we can
          # do is try to use the default flags.
          return cflags
      else:
        # If this is a standalone .h file with no source, the best we can do is
        # try to use the default flags.
        return cflags

  # Ninja needs the path to the source file relative to the output build
  # directory.
  rel_filename = os.path.relpath(os.path.realpath(filename), out_dir)

  # Ask ninja how it would build our source file.
  p = subprocess.Popen(['ninja', '-v', '-C', out_dir, '-t',
                        'commands', rel_filename + '^'],
                       stdout=subprocess.PIPE)
  stdout, stderr = p.communicate()
  if p.returncode:
    return cflags

  # Ninja might execute several commands to build something. We want the last
  # clang command.
  clang_line = None
  for line in reversed(stdout.split('\n')):
    if 'cc' in line:
      compile_line = line
      break
  else:
    return cflags

  # Parse flags that are important for YCM's purposes.
  for flag in compile_line.split(' '):
    if flag.startswith('-I'):
      # Relative paths need to be resolved, because they're relative to the
      # output dir, not the source.
      if flag[2] == '/':
        cflags.append(flag)
      else:
        abs_path = os.path.normpath(os.path.join(out_dir, flag[2:]))
        cflags.append('-I' + abs_path)
    elif flag.startswith('-std'):
      cflags.append(flag)
    elif flag.startswith('-') and flag[1] in 'DWFfmO':
      if flag == '-Wno-deprecated-register' or flag == '-Wno-header-guard':
        # These flags causes libclang (3.3) to crash. Remove it until things
        # are fixed.
        continue
      cflags.append(flag)

  return cflags


def FlagsForFile(filename):
  """This is the main entry point for YCM. Its interface is fixed.

  Args:
    filename: (String) Path to source file being edited.

  Returns:
    (Dictionary)
      'flags': (List of Strings) Command line flags.
      'do_cache': (Boolean) True if the result should be cached.
  """

  top_srcdir = FindTopSrcFromFilename(filename)
  cflags = GetClangCommandFromNinjaForFilename(top_srcdir, filename)
  final_cflags = default_cflags + cflags

  return {
    'flags': final_cflags,
    'do_cache': True
  }
