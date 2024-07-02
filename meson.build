project('epaper-img', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

incdir = include_directories('lib/Config', 'lib/GUI', 'lib/e-Paper', 'lib/Fonts')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
lgpio_dep = dependency('lgpio')

executable('epaper-img',
           'lib/GUI/GUI_Paint.c',
           'lib/GUI/GUI_BMPfile.c',
           'lib/e-Paper/EPD_4in2b_V2.c',
           'lib/Config/DEV_Config.c',
           'src/EPD_4in2b_V2_test.c',
           'src/ImageData2.c',
           'src/ImageData.c',
           'src/ImageData.h',
           'src/main.c',
           dependencies : [ m_dep, lgpio_dep ],
           include_directories : incdir,
           install : true)
