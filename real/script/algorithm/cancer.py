from math import ceil
from algorithm.algorithm import Algorithm
from base.controller import Controller
from base.file_system import FileSystem
from base.configs import Configs
from utils.commands import Commands
import os
import re
import numpy as np
class Cancer(Algorithm):
    def __init__(self, controller:Controller) -> None:
        super().__init__()
        self.name = "cancer"
        self.__controller = controller
        self.__controller.addAlgorithm(self)

    def __calculateRank(self, u):
        return ceil(8/((1-u)/0.1))

    def __translateCancerPatterns(self, experiment_path): #pattern = [{}, {}, {}]
        dimensions = Configs.getDimensions()
        cancer_patterns = [] # [[{},{}], [{},{}]]
        lambda_0 = self.__controller.current_dataset.getDensity()

        experiment_folder = re.sub("/cancer.experiment", "", experiment_path)
        temp_folder = f"{experiment_folder}/temp"

        plain_patterns = os.listdir(temp_folder)
        for plain_pattern in plain_patterns:
            tuples = [set() for dimension in range(dimensions)]
            pattern_path = f"{temp_folder}/{plain_pattern}"
            
            data = ""
            with open(pattern_path) as pattern_file:
                for line in pattern_file:
                    data += line
            data = data.strip('[]')

            numpy_pattern = np.array(np.matrix(data))
            for index, value in np.ndenumerate(numpy_pattern):
                # if value < 0.5: # dont add dimensions to tuple
                if value <= lambda_0: # dont add dimensions to tuple
                    continue
                for n in range(len(index)): # iterates over all indices of the index
                    nth_tuple = tuples[n]
                    nth_tuple.add(index[n])

            cancer_patterns.append(tuples)

        return cancer_patterns

    def __createCancerFile(self):
        cancer_patterns = self.__translateCancerPatterns(self.experiment_path)

        with open(self.experiment_path, "a") as cancer_file:
            for pattern in cancer_patterns: #pattern = [{}, {}, {}]
                line = ""
                for d_tuple in pattern:
                    if len(d_tuple) == 0:
                        continue

                    line += str(d_tuple).replace("{","").replace("}","").replace(" ","") # d_tuple = {}
                    line += " "
                line = line.strip()
                if line == "":
                    print("CANCER found no patterns")
                    continue

                line += f" {self.__controller.current_dataset.calculateTuplesDensity(pattern):.6f}"
                line += "\n"
                cancer_file.write(line)

    def run(self, u, timeout):
        if Configs.getDimensions() > 2:
            return True

        matlab_folder = ""
        # if self.__controller.ufmgMode():
            # matlab_folder = Configs.ufmgMatlabFolder()

        rank = 7

        configuration_name = self.__controller.current_configuration_name

        current_experiment = self.__controller.current_experiment
        temp_base_folder = f"../iteration/{configuration_name}"

        translated_tensor_path = f"{self.__controller.base_dataset_path}.mat"
        self.experiment_path = f"../iteration/{configuration_name}/output/{current_experiment}/experiments/cancer.experiment"
        temp_folder = f"../iteration/{configuration_name}/output/{current_experiment}/experiments/temp"
        self.log_path = f"../iteration/{configuration_name}/output/{current_experiment}/logs/cancer.log"

        command = f"/usr/bin/time -o {self.log_path} -f 'Memory (kb): %M' "
        command += f"{matlab_folder}matlab -nodisplay -r 'cd(\"algorithm\"); "
        command += f"cancer({rank},\"../{translated_tensor_path}\","
        command += f"\"../{temp_base_folder}\","
        command += f"\"{current_experiment}\");exit' | tail -n +11 "

        print(command)
        timedout = Commands.executeWithTimeout(command, timeout)     

        if timedout is False:
            self.__createCancerFile()
            
        FileSystem.delete(temp_folder)
        return timedout

        