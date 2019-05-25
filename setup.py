import numpy as np
import platform
from setuptools import setup, Extension
from Cython.Build import cythonize

include_dirs = [np.get_include(), './cppsrc']
extra_compile_args = []
extra_link_args = []
if platform.system() == 'Darwin':
    # When building with clang, this is required in order to use the
    # full C++17 library (specifically, std::variant)
    extra_compile_args.append('-mmacosx-version-min=10.14')
    extra_link_args.append('-mmacosx-version-min=10.14')
for inc_dir in include_dirs:
    extra_compile_args.append('-I' + inc_dir)

setup(
    name="baduk",
    include_dirs=include_dirs,
    ext_modules=cythonize(Extension(
        "baduk",
        sources=[
            "baduk/*.pyx",
            "cppsrc/baduk/agent.cpp",
            "cppsrc/baduk/board.cpp",
            "cppsrc/baduk/counter.cpp",
            "cppsrc/baduk/game.cpp",
            "cppsrc/baduk/gostring.cpp",
            "cppsrc/baduk/neighbor.cpp",
            "cppsrc/baduk/point.cpp",
            "cppsrc/baduk/pointset.cpp",
            "cppsrc/baduk/scoring.cpp",
            "cppsrc/baduk/zobrist/codes.cpp",
            "cppsrc/baduk/zobrist/zobrist.cpp",
        ],
        language="c++",
        extra_compile_args=['-O3', '-std=c++17'] + extra_compile_args,
        extra_link_args=extra_link_args,
    )),
    install_requires=[
        'numpy',
    ]
)
