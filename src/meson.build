subdir('render')
subdir('gamestate')

launcher = files('launcher.c')
launcher_exe = executable(
  'TileBreaker',
  launcher,
  link_with: [
    console_lib,
    input_lib,
    field_lib
  ],
  dependencies: [
    gudp,
    thread_dep
  ],
  include_directories: inc
)
