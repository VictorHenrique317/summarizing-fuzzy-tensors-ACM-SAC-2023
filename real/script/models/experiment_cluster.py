import re
from models.log import Log
from models.experiment import Experiment
from base.configs import Configs
from utils.commands import Commands


class ExperimentCluster():
    def __init__(self, base_folder, u) -> None:
        self.__experiments_folder = f"{base_folder}/experiments"
        self.__logs_folder = f"{base_folder}/logs"
        self.__experiments = []

        self.__u = float(u)
        self.__logs = []
        # self.__configs = Configs()
        self.__initialize()

    def __initialize(self):
        for log_file in Commands.listFolder(self.__logs_folder):
            log_path = f"{self.__logs_folder}/{log_file}"
            log = Log(log_path)
            self.__logs.append(log)

        # self.loadExperiments()

    def loadExperiments(self):
        dimension = Configs.getDimensions()
        for experiment_file in Commands.listFolder(self.__experiments_folder):  # memory dump
            experiment_path = f"{self.__experiments_folder}/{experiment_file}"
            experiment = Experiment(experiment_path, self.__u, dimension)  # memory dump
            self.__experiments.append(experiment)

    def getU(self):
        return self.__u

    def getLogs(self):
        return self.__logs

    def getExperiments(self):
        return self.__experiments