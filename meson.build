project(
  'TileBreaker',
  'c',
  default_options: [
    'warning_level=3',
    'buildtype=debugoptimized'
  ],
  version : '0.0.1',
  license: 'GPLv3'
)

gudp = dependency(
  'gudp',
  fallback : [
    'gudp',
    'gudp_dep'
  ]
)

defines = []

target_os = target_machine.system()
if target_os == 'linux'
  defines += [
    '-D__LINUX__',
    '-D__PLATFORM__=\"linux\"'
  ]
else
  error('Current platform is not supported by TileBreaker.')
endif

defines += [
  '-D__' + get_option('domain-type').to_upper() + '__',
  '-D__NETCODE__=' + get_option('domain-type')
]

inc = [
  include_directories('include'),
  include_directories('subprojects/gudp/include')
]

add_project_arguments(defines, language: 'c')
thread_dep = dependency('threads')

subdir('src')

meson.add_install_script('install.sh')
