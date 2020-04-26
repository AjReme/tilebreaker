subdir('render')
subdir('gameserver')

launcher = files('launcher.c')
launcher_exe = executable(
  'game_launcher',
  launcher,
  link_with: [
    console_lib,
    input_lib
  ],
  dependencies: [
    gudp,
    thread_dep
  ],
  include_directories: inc
)
