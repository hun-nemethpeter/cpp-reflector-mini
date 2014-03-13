top = '.'
out = '_build_'

def options(opt):
    opt.load('compiler_cxx waf_unit_test')

def configure(conf):
    conf.load('compiler_cxx waf_unit_test')
    conf.check_cfg(path='llvm-config',
                   package='',
                   args='--cppflags --libs core bitreader transformutils mcparser',
                   uselib_store='LLVM_LIBS')
    conf.check_cfg(path='llvm-config', package='', args='--ldflags', uselib_store='LLVM_FLAGS')

def build(bld):
    clang_libs = [
          'clangFrontendTool',
          'clangFrontend',
          'clangDriver',
          'clangSerialization',
          'clangCodeGen',
          'clangParse',
          'clangSema',
          'clangStaticAnalyzerFrontend',
          'clangStaticAnalyzerCheckers',
          'clangStaticAnalyzerCore',
          'clangAnalysis',
          'clangARCMigrate',
          'clangEdit',
          'clangAST',
          'clangASTMatchers',
          'clangLex',
          'clangBasic',
          'clangTooling',
          'clangRewriteCore',
          ]
    clang_flags = [
          '-std=c++0x',
          '-g',
          '-O0',
          '-fPIC',
          '-fvisibility-inlines-hidden',
          '-Wall',
          '-Wno-unused-parameter',
          '-pedantic',
          '-fno-rtti',
          '-Wno-variadic-macros'
          ]
    bld.stlib(
        target = 'lib_ipr',
        source = bld.path.ant_glob('ipr/*.C'),
        includes = ['.'],
        cxxflags = ['-g', '-O0', '-Wall', '-std=c++0x']
        )
    bld.program(
        target = 'generator',
        source = bld.path.ant_glob('*.cc'),
        includes = ['.'],
        cxxflags = clang_flags,
        use='lib_ipr',
        uselib = 'LLVM_LIBS LLVM_FLAGS',
        stlib = clang_libs,
    )
