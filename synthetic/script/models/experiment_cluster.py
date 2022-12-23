import re
from models.log import Log, AveragedLog
from models.experiment import Experiment
from base.configs import Configs
from utils.commands import Commands

from models.attribute import Attribute


class ExperimentCluster():
    def __init__(self, base_folder, iteration, u, correct_observations) -> None:
        self.__experiments_folder = f"{base_folder}/experiments"
        self.__logs_folder = f"{base_folder}/logs"
        self.__base_folder = base_folder
        self.__experiments = []

        self.__iteration = int(iteration)
        self.__u = float(u)
        self.__correct_observations = int(correct_observations)
        self.__logs = []
        # self.__configs = Configs()
        self.__initialize()

    def __initialize(self):
        for log_file in Commands.listFolder(self.__logs_folder):
            log_path = f"{self.__logs_folder}/{log_file}"
            log = Log(log_path)
            self.__logs.append(log)

    def loadExperiments(self):
        dimension = len(Configs.getParameter("dataset_size"))
        for experiment_file in Commands.listFolder(self.__experiments_folder): # memory dump
            experiment_path = f"{self.__experiments_folder}/{experiment_file}"
            experiment = Experiment(experiment_path, self.__iteration, self.__correct_observations, self.__u, dimension) # memory dump
            self.__experiments.append(experiment)

    def getIteration(self):
        return self.__iteration

    def getU(self):
        return self.__u
    
    def getCorrectObservations(self):
        return self.__correct_observations

    def getLogs(self):
        return self.__logs

    def getExperiments(self):
        return self.__experiments

    def getBaseFolder(self):
        return self.__base_folder

class AveragedExperimentCluster():
    def __init__(self, u, correct_observations, averaged_logs) -> None:
        self.__u = u
        self.__correct_observations = correct_observations
        self.__averaged_logs = averaged_logs

    def getU(self):
        return self.__u
    
    def getCorrectObservations(self):
        return self.__correct_observations

    def getLogs(self):
        return self.__averaged_logs

    @staticmethod
    def __groupByIteration(clusters):
        twin_clusters = dict() # same clusters but different iterations
        for cluster in clusters:
            iteration = cluster.getIteration()
            twins = twin_clusters.setdefault(iteration, [])
            twins.append(cluster)
            twin_clusters[iteration] = twins

        return twin_clusters

    @staticmethod
    def average(clusters):
        averaged_clusters = [] # [AveragedExperimentCluster, AveragedExperimentCluster]
        twin_clusters = AveragedExperimentCluster.__groupByIteration(clusters) # {1: [cluster1, cluster2], 2:[cluster3, cluster4]}
        nb_clusters_per_iteration = len(twin_clusters[1])
        nb_iterations = len(twin_clusters)

        for ith_cluster in range(nb_clusters_per_iteration):
            clusters_to_average = []

            dummy_cluster = twin_clusters[1][ith_cluster]
            u = dummy_cluster.getU()
            correct_observations = dummy_cluster.getCorrectObservations()

            for iteration in range(1, nb_iterations+1):
                clusters_to_average.append(twin_clusters[iteration][ith_cluster])

            log_groups = [cluster.getLogs() for cluster in clusters_to_average] # [[m_log, p_log], [m_log, p_log]]

            averaged_cluster = AveragedExperimentCluster(u, correct_observations, AveragedLog.average(log_groups))
            averaged_clusters.append(averaged_cluster)

        # for averaged_cluster in averaged_clusters: # TESTING
        #     if averaged_cluster.getCorrectObservations() == 1 and averaged_cluster.getU() == 0.6:
        #         for log in averaged_cluster.getLogs():
        #             if log.getAlgorithm() == "getf":
        #                 print(log.getAttributeValue(Attribute.RSS_EVOLUTION))

        return averaged_clusters

        

    

           

    

