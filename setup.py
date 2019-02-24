import numpy as np
from setuptools import setup, Extension
from Cython.Build import cythonize

setup(
    name="baduk",
    include_dirs = [np.get_include()],
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
        extra_compile_args=['-std=c++1z', '-I./cppsrc'],
    )),
    install_requires=[
        'numpy',
    ]
)
