from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

import platform
import subprocess
import os
import sys

class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='.', **kwa):
        Extension.__init__(self, name, sources=[], **kwa)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)

class cmake_build_ext(build_ext):
    def build_extensions(self):
        # Ensure that CMake is present and working
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('Cannot find CMake executable')

        for ext in self.extensions:
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

            cmake_args = []

            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)

            # Config
            subprocess.check_call(['cmake', "."] + cmake_args, cwd=self.build_temp)

            # Build
            subprocess.check_call(['cmake', '--build', '.', '--config', "Release"], cwd=self.build_temp)

setup(name='hestia',
      packages=['hestia'],
      version="0.0.1",
      description='HESTIA',
      author='ICHEC',
      author_email='support@ichec.ie',
      #long_description=open('README.md').read(),
      long_description_content_type='text/markdown',
      ext_modules=[CMakeExtension("_hestia")],
      cmdclass={'build_ext': cmake_build_ext},
      zip_safe=False,
      classifiers=[
          "Programming Language :: Python :: 3",
          "License :: OSI Approved :: MIT License",
          "Operating System :: MacOS",
          "Operating System :: POSIX :: Linux",
      ],
      )