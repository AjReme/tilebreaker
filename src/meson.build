subdir('expect')
subdir('gameserver')
subdir('gameclient')
subdir('gamestate')
subdir('gameresponse')
subdir('render')

launcher = files('launcher.c')
launcher_exe = executable(
  'TileBreaker',
  launcher,
  link_with: [
    gameserver_lib,
    gameclient_lib,
    gamestate_lib,
    gameresponse_lib,
    render_lib,
    expect_lib
  ],
  dependencies: [
    gudp,
    thread_dep
  ],
  include_directories: inc
)
