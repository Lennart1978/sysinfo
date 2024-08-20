project('sysinfo', 'c', version: '1.0.0', meson_version: '>= 0.60.0')
executable(
    'sysinfo',
    ['main.c', 'sysinfo.c', 'center_text.c', 'selfie.c'],
    include_directories: include_directories('.'),
)
executable(
    'picinfo',
    ['picinfo.c', 'selfie.c'],
    include_directories: include_directories('.'),
)
executable(
    'shuffle-ascii',
    ['shuffle-ascii.c', 'ascii.c'],
    include_directories: include_directories('.'),
)
executable(
    'ascii2c',
    ['ascii2c.c'],
    include_directories: include_directories('.'),
)

