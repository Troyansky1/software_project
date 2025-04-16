from setuptools import Extension, setup

module = Extension("symnmf", sources=['symnmfmodule.c', 'symnmf.c', 'lin_alg_utils.c', 'utils.c'])
setup(name='symnmf',
     version='1.0',
     description='Python wrapper for custom C extension',
     ext_modules=[module])
