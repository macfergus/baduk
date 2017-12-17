from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    name="baduk",
    ext_modules=cythonize(Extension(
        "baduk",
        sources=[
            "baduk.pyx",
            "baduk/agent.cpp",
            "baduk/board.cpp",
            "baduk/game.cpp",
            "baduk/point.cpp",
        ],
        language="c++",
        extra_compile_args=['-std=c++1z'],
    ))
)
