rewutils_src = ['netutils.c', 'svrcmds.c']
rewutils_inc = include_directories('.')

librewutils = static_library('rewutils', rewutils_src, include_directories : rewutils_inc)

rewutils_dep = declare_dependency(include_directories: rewutils_inc, link_with: librewutils)
