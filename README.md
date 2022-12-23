# Introduction
This project aims to create a experimental environment to test and analyse different box clustering algorithms.
With this code base it is possible to specify:

- Tensor size.
- Timeout for algorithm execution.
- Size of randomly planted patterns.
- Number of randomly planted patterns.
- Levels of noise that will be applied on the clean tensor.
- Number of iterations.

And then run the desired algorithms on these noised tensors. On each iteration one tensor with randomly planted patterns is created, and then different levels of noise are applied on the same tensor. The defined set of algorithms is executed on each noised tensor created from this "clean" one, every iteration repeats this process with a different clean tensor. 

When the executions are finished the user can also extract and plot some metrics (RSS evolution, quality, execution time and number of found patterns) from the experiment files. The extracted (and plotted) data is an average across all iterations.

# How to use
The experimental environment can be configured in script/configs/configs.json:

- configuration_name: The name of this experimental environment, multiple environments can be defined by creating multiple configuration files. To do so just copy configs.json and paste on the same directory with a different name and configuration_name.

- timeout: Maximum time allowed for EACH algorithm execution (in seconds).

- dataset_size: Size of the synthetic tensor.

- pattern_size: Size of the synthetic patterns, all dimensions have equal size.

- n_patterns: Number of randomly planted synthetic patterns.

- correct_obs: Levels of applied noise (Inverse cumulative beta distribution), the lower the noisier.

- nb_iterations: Number of iterations.

To specify which algorithms to use go to script/main.py and comment/uncomment lines 11 to 15. The experiments can be made by uncommenting line 18, and the data extraction/plotting can be made by uncommenting line 19.

The graphs are saved under post_analysis/{tensor_name}/ , and the averaged metrics in post_analysis/{tensor_name}/plotting_data/. It's possible to customize the y axis range for each different metric by changing lines 119, 124, 129 and 135 in script/base/controller.py.

# Dependencies

Depedencies for python3 can be installed via pip, these are:
- numpy.
- psutil.
- scipy.
- matplotlib.
- pandas.

Other:
- libboost is required to compile NclusterBox and Tri/BiclusterBox.
- R language and reticulate package is required to run GETF.
- MATLAB is required to run Cancer.

# Setting up

- It is necessary to compile NclusterBox and Tri/BiclusterBox before using it, to do so execute "make" in libs/nclusterbox and libs/tribiclusterbox.

- It is necessary to compile helper scripts in order to generate tensors and noise then, to do so execute "make" in libs/gennsets and libs/num-noise.

- It is necessary to give execution permission for the input readers (fiber-input and slice-input) in libs/nclusterbox and libs/tribiclusterbox.
