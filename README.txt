# The following are Windows commands

# Builds the docker visualization image:
```
docker build -t python-visualization visualization
```

# Run the visualization container mounted in the `visualization` folder:
```
docker run --rm -v "%CD%/data":/data python-visualization
```
**Replace ^ with \ for multi-line on unix-based system**
--rm # This deletes the container and volumes afterwards
--name Erkin_test # adds unecessary name


# Visualization
docker build -t python-visualizer visualization
docker run --rm -v "%CD%/data":/data python-visualizer

# Generation
# This is for creating starting conditions.
docker build -t cpp-ic_generator generation
docker run --rm -v "%CD%/data":/data cpp-ic_generator


# Computing
# This is for using starting conditions and computing actual results
docker build -t cpp-solver computation
docker run --rm -v "%CD%/data":/data cpp-solver 2>&1