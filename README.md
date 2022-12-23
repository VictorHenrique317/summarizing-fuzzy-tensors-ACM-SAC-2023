# Introduction
This directory contains everything to rerun all the experiments described in the article "Summarizing Fuzzy Tensors with Sub-Tensors", presented at ACM SAC 2023.

# Synthetic section
The synthetic part of this project project (under synthetic/) aims to create a experimental environment to test and analyse different box clustering algorithms. With this code base it is possible to specify:

- Tensor size.
- Timeout for algorithm execution.
- Size of randomly planted patterns.
- Number of randomly planted patterns.
- Levels of noise that will be applied on the clean tensor.
- Number of iterations.

And then run the desired algorithms on these noised tensors. On each iteration one tensor with randomly planted patterns is created, and then different levels of noise are applied on the same tensor. The defined set of algorithms is executed on each noised tensor created from this "clean" one, every iteration repeats this process with a different clean tensor. 

When the executions are finished the user can also extract and plot some metrics (RSS evolution, quality, execution time and number of found patterns) from the experiment files. The extracted (and plotted) data is an average across all iterations.

# Real section
The real part of this project project (under real/) aims to create a experimental environment to test and analyse different box clustering algorithms. With this code base it is possible to specify:

- Timeout for algorithm execution.

Here only the timout time is configurable, as there is no synthetic tensor or noise adding. There is just one iteration and only the RSS evolution graphs are plotted, despite the other metrics also being generated. To take a look at the other metrics (after the algorithm is executed) go to real/iteration/{dataset_name}/output/u0.0/logs/{algorithm}.log, where: 

- {dataset_name} is retweets for the 3D case and retweets-2d for the 2D case.
- {algorithm} is the desired algorithm name.

# How to use
The two sections provided here are independent, configurations or/and executions made on one will not affect the other. To begin the experiments just run synthetic/script/main.py or real/script/main.py. The default configurations are the ones used in the article, the process can be slow so the default number of iterations is set to one on the synthetic section to facilitate reproduction (30 were done in the article).

The experimental environment can be configured in synthetic/script/configs/configs.json and in real/script/configs/configs.json:

- configuration_name (synthetic-only): The name of this experimental environment, multiple environments can be defined by creating multiple configuration files. To do so just copy configs.json and paste on the same directory with a different name and configuration_name.

- timeout: Maximum time allowed for EACH algorithm execution (in seconds).

- dataset_size (synthetic-only): Size of the synthetic tensor.

- pattern_size (synthetic-only): Size of the synthetic patterns, all dimensions have equal size.

- n_patterns (synthetic-only): Number of randomly planted synthetic patterns.

- correct_obs (synthetic-only): Levels of applied noise (Inverse cumulative beta distribution), the lower the noisier.

- nb_iterations (synthetic-only): Number of iterations.

To specify which algorithms to use go to synthetic/script/main.py or real/script/main.py and comment/uncomment lines 11 to 15. The experiments can be made by uncommenting line 18, and the data extraction/plotting can be made by uncommenting line 19.

The graphs are saved under synthetic/post_analysis/{tensor_name}/ and real/post_analysis/{tensor_name}/. It's possible to customize the y axis range for each different metric by changing lines 119, 124, 129 and 135 in synthetic/script/base/controller.py.

# Dependencies

Depedencies for python3 can be installed via pip, these are:
- numpy.
- psutil.
- scipy.
- matplotlib.
- sklearn.
- pandas.

Other:
- libboost is required to compile NclusterBox and Tri/BiclusterBox.
- R language and reticulate package is required to run GETF.
- MATLAB is required to run Cancer.

# Setting up
The compiled programs are provided for quick reproduction, but it's source code is also available for manual compilation if necessary:

- To compile NclusterBox and Tri/BiclusterBox execute "make" in synthetic/algorithms/nclusterbox, synthetic/algorithms/tribiclusterbox and in real/algorithms/nclusterbox, real/algorithms/tribiclusterbox.

- To compile the programs to generate tensors and to add noise to them execute "make" in synthetic/algorithms/gennsets, synthetic/algorithms/num-noise and in real/algorithms/gennsets, real/algorithms/num-noise.

It is necessary to give execution permission for the input readers (fiber-input and slice-input) in synthetic/algorithms/nclusterbox, synthetic/algorithms/tribiclusterbox and in real/algorithms/nclusterbox, real/algorithms/tribiclusterbox.