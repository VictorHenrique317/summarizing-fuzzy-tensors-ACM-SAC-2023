from algorithm.algorithm import Algorithm
from base.controller import Controller
from base.file_system import FileSystem
from models.pattern import Pattern
from utils.commands import Commands
import os
import re
import numpy as np
class Getf(Algorithm):
    def __init__(self, controller:Controller) -> None:
        super().__init__()
        self.name = "getf"
        self.__controller = controller
        self.__controller.addAlgorithm(self)

    def __calculateNoiseEndurance(self, u):
        return u

    def __translateNumpyPatterns(self, experiment_path):
        dimensions = len(self.__controller.getParameter("dataset_size"))
        getf_patterns = [] # [[{},{}], [{},{}]]

        experiment_folder = re.sub("/getf.experiment", "", experiment_path)
        temp_folder = f"{experiment_folder}/temp"

        numpy_patterns = os.listdir(temp_folder)
        for numpy_pattern in numpy_patterns:
            tuples = [set() for dimension in range(dimensions)]
            pattern_path = f"{temp_folder}/{numpy_pattern}"

            numpy_pattern = np.load(pattern_path)
            for index, value in np.ndenumerate(numpy_pattern):
                if value == 0: # dont add dimensions to tuple
                    continue
                for n in range(len(index)): # iterates over all indices of the index
                    nth_tuple = tuples[n]
                    nth_tuple.add(index[n])

            getf_patterns.append(tuples)

        return getf_patterns

    def __correctLogFile(self, pattern_nb):
        lines = []
        new_lines = []
        with open(self.log_path, 'r') as log_file:
            lines = [line for line in log_file]

        for line in lines:
            if "Nb of patterns" in line:
                line = f"Nb of patterns: {pattern_nb}\n"

            new_lines.append(line)

        Commands.deleteFile(self.log_path)
        with open(self.log_path, 'w') as log_file:
            for line in new_lines:
                log_file.write(line)

    def __createGetfFile(self):
        getf_patterns = self.__translateNumpyPatterns(self.experiment_path)
        pattern_nb = len(getf_patterns)

        with open(self.experiment_path, "a") as getf_file:
            for pattern in getf_patterns: #pattern = [{}, {}, {}]
                line = ""
                for d_tuple in pattern:
                    line += str(d_tuple).replace("{","").replace("}","").replace(" ","") # d_tuple = {}
                    line += " "
                line = line.strip()
                line += f" {self.__controller.current_dataset.calculateTuplesDensity(pattern):.6f}"
                line += "\n"
                getf_file.write(line)

        self.__correctLogFile(pattern_nb)

    def run(self, u, observations, timeout):
        max_pattern_number = self.__controller.getParameter("n_patterns")
        noise_endurance = self.__calculateNoiseEndurance(u)

        current_experiment = self.__controller.current_experiment
        current_iteration_folder = self.__controller.current_iteration_folder

        translated_tensor_path = f"{current_iteration_folder}/tensors/numpy/dataset-co{observations}.npy"
        self.experiment_path = f"{current_iteration_folder}/output/{current_experiment}/experiments/getf.experiment"
        temp_folder = f"{current_iteration_folder}/output/{current_experiment}/experiments/temp"
        self.log_path = f"{current_iteration_folder}/output/{current_experiment}/logs/getf.log"
        
        command = f"/usr/bin/time -o {self.log_path} -f 'Memory (kb): %M' "
        command += f"Rscript algorithm/getf.R {translated_tensor_path} "
        command += f"{noise_endurance} "
        command += f"{max_pattern_number} "
        command += f"{current_iteration_folder} "
        command += f"{current_experiment}"
        print(command)
        timedout = Commands.executeWithTimeout(command, timeout)              

        if timedout is False:
            self.__createGetfFile()
        FileSystem.delete(temp_folder)

        return timedout
                
