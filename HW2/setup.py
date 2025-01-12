from setuptools import Extension, setup

module = Extension('mykmeans', sources=['kmeansmodule.c', 'kmeans.c'])
setup(name='mykmeans',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])
