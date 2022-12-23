from algorithm.algorithm import Algorithm
from base.controller import Controller
from base.file_system import FileSystem
from utils.commands import Commands
from base.configs import Configs
import os
import re
import numpy as np
from models.pattern import Pattern


class Getf(Algorithm):
    def __init__(self, controller:Controller) -> None:
        super().__init__()
        self.log_path = None
        self.experiment_path = None
        self.name = "getf"
        self.__controller = controller
        self.__controller.addAlgorithm(self)

    def __translateNumpyPatterns(self, experiment_path):
        dimensions = len(self.__controller.dataset.getDimension())
        getf_patterns = [] # [[{},{}], [{},{}]]

        experiment_folder = re.sub("/getf.experiment", "", experiment_path)

        return getf_patterns

    def __createGetfFile(self):
        getf_patterns = [] # [[{},{}], [{},{}]]
        with open(self.experiment_path, "r") as getf_file:
            for line in getf_file:
                line = line.replace("\n", "")
                pattern = Pattern(line, 3, density=1).get()

                getf_patterns.append(pattern)

        Commands.deleteFile(self.experiment_path)

        with open(self.experiment_path, "w") as getf_file:
            for pattern in getf_patterns:
                line = ""
                for d_tuple in pattern:
                    d_tuple = [int(i) for i in d_tuple]
                    line += str(d_tuple).replace("[","").replace("]","").replace(" ","") # d_tuple = []
                    line += " "

                line = line.strip()
                line += f" {self.__controller.current_dataset.calculateTuplesDensity(pattern):.6f}"
                line += "\n"
                getf_file.write(line)

    def run(self, u, timeout):
        if Configs.getDimensions() < 3:
            return True

        max_pattern_number = 7
        noise_endurance = 0.6

        current_experiment = self.__controller.current_experiment
        configuration_name = self.__controller.current_configuration_name

        translated_tensor_path = f"{self.__controller.base_dataset_path}.npy"
        self.experiment_path = f"../iteration/{configuration_name}/output/{current_experiment}/experiments/getf.experiment"
        # temp_folder = f"../iteration/{configuration_name}/output/{current_experiment}/experiments/temp"
        self.log_path = f"../iteration/{configuration_name}/output/{current_experiment}/logs/getf.log"
        
        command = f"/usr/bin/time -o {self.log_path} -f 'Memory (kb): %M' "
        command += f"Rscript algorithm/getf.R {translated_tensor_path} "
        command += f"{noise_endurance} "
        command += f"{max_pattern_number} "
        command += f"../iteration/{configuration_name} "
        command += f"{current_experiment} "
        command += f"{self.experiment_path}"
        print(command)
        timedout = Commands.executeWithTimeout(command, timeout)              

        if timedout is False:
            print("GETF was executed sucessfully!")
            self.__createGetfFile()
        # FileSystem.delete(temp_folder)

        return timedout
                
