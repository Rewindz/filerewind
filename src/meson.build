
subdir('lib')

client_src = ['client_main.c']
server_src = ['server_main.c']


executable('client', client_src, c_args : ['-Wall'], dependencies: rewutils_dep)

executable('server', server_src, c_args : ['-Wall'], dependencies: rewutils_dep)

