@ECHO OFF
:: This .bat script builds all the docker images in this project
docker build -t cpp-ic_generator generation 2>&1 && docker build -t cpp-solver computation 2>&1 && docker build -t python-visualizer visualization 2>&1

