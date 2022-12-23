from algorithm.algorithm import Algorithm
from base.controller import Controller
import time
from utils.commands import Commands
from models.experiment import Experiment
from base.configs import Configs
import re

class TriBiclusterBox(Algorithm):
    def __init__(self, controller: Controller) -> None:
        super().__init__()
        self.name = "tribiclusterbox"
        self.__controller = controller
        self.__controller.addAlgorithm(self)

    def __deleteEqualPatterns(self, experiment):
        print("Deleting equal patterns...")
        unique_patterns_strings = []
        unique_patterns = []
        deletions_counter = 0

        for pattern in experiment.getPatterns():
            pattern_string = pattern.getPatternString()

            if pattern_string in unique_patterns_strings:  # already exists
                deletions_counter += 1
                continue

            unique_patterns_strings.append(pattern_string)
            unique_patterns.append(pattern)

        print(f"{deletions_counter} duplicated patterns deleted!")
        experiment.rewritePatterns(unique_patterns)

    def __writeLog(self, elapsed_time):
        nb_patterns = None
        with open(self.experiment_path) as file:
            nb_patterns = sum([1 for line in file])

        total_time = None
        with open(self.log_path, "r") as file:
            total_time = file.readlines()[-1]

        selection_time = None
        with open(self.log_path, "r") as file:
            selection_time = file.readlines()[-2]

        total_time = float(re.findall("(\d*\.\d*)s", total_time)[0])

        match = re.findall("(\d*\.\d*)s", selection_time)
        if len(match) > 0:
            selection_time = float(match[0])
            print(f"Selection time: {selection_time}")
        else:
            print("Selection time is negligible")
            selection_time = 0

        total_time -= selection_time
        total_time = f"Total time: {total_time}s\n"

        memory = None
        with open(self.log_path, "r") as file:
            memory = file.readlines()[0]

        with open(self.log_path, "w") as file:
            # file.write(f"Total time: {elapsed_time}s\n")
            file.write(total_time)
            file.write(memory)
            file.write(f"Nb of patterns: {nb_patterns}")

    def run(self, u, timeout):
        current_experiment = self.__controller.current_experiment
        current_iteration_folder = self.__controller.current_iteration_folder
        dimension = Configs.getDimensions()

        self.experiment_path = f"{current_iteration_folder}/output/{current_experiment}/experiments/tribiclusterbox.experiment"
        self.log_path = f"{current_iteration_folder}/output/{current_experiment}/logs/tribiclusterbox.log"
        dataset_path = self.__controller.current_dataset_path

        if dimension == 3:
            tube_dim = 2
        if dimension == 2:
            tube_dim = 1
    
        command = f"/usr/bin/time -o {self.log_path} -f 'Memory (kb): %M' "
        command += f"../algorithms/tribiclusterbox/slice-input {dataset_path} {tube_dim} | "
        command += f"../algorithms/tribiclusterbox/nclusterbox -f {dataset_path} -o {self.experiment_path}"
        command += f">> {self.log_path}"

        print(command)
        start = time.time()
        timedout = Commands.executeWithTimeout(command, timeout)
        end = time.time()
        elapsed_time = end - start

        if timedout is False:
            experiment = Experiment(self.experiment_path, u, dimension)
            #self.__deleteEqualPatterns(experiment)
            self.__writeLog(elapsed_time)
            pass

        return timedout
