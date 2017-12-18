from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    name="baduk",
    ext_modules=cythonize(Extension(
        "baduk",
        sources=[
            "baduk/*.pyx",
            "cppsrc/baduk/agent.cpp",
            "cppsrc/baduk/board.cpp",
            "cppsrc/baduk/game.cpp",
            "cppsrc/baduk/point.cpp",
        ],
        language="c++",
        extra_compile_args=['-std=c++1z', '-I./cppsrc'],
    ))
)
