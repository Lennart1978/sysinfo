# Hauptprojekt-Name
project(
    'sysinfo',
    'c',
    version: '0.1',
    meson_version: '>= 0.60.0' # Stelle sicher, dass du eine kompatible Meson-Version hast,,,,
)

# Erstelle eine Bibliothek (optional, falls du eine statische Bibliothek benötigst)
# libsysinfo = library('sysinfo', 'sysinfo.c', include_directories: include_directories('.'))

# Erstelle das ausführbare Programm
executable(
    'sysinfo',
    ['main.c', 'sysinfo.c', 'center_text.c', 'selfie.c'],
    include_directories: include_directories('.'),
)
