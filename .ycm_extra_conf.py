def Settings( **kwargs ):
  return {
    'flags': ['-x', 'c++', '-Wall', '-Wextra', '-Werror', '-I/usr/lib/wx/include/gtk3-unicode-3.2', '-I/usr/include/wx-3.2', '-DWXUSINGDLL',
              '-D__WXGTK3__', '-D__WXGTK__', '-D_FILE_OFFSET_BITS=64', '-g', '-I~/Japanese-Study-Tool',],
  }
