@ECHO OFF
:: This .bat script runs all the docker images in "order"
docker run --rm -v "%CD%/data":/data cpp-ic_generator 2>&1 && docker run --rm -v "%CD%/data":/data cpp-solver 2>&1 && docker run --rm -v "%CD%/data":/data python-visualizer 2>&1