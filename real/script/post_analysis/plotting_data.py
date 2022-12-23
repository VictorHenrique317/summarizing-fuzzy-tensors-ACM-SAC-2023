from numpy import average
from models.attribute import Attribute
from base.file_system import FileSystem
import collections
class PlottingData():
    def __init__(self, configuration_name) -> None:
        self.__configuration_name = configuration_name
        self.__algorithm = None
        self.__attribute = None

        self.__original_clusters = None
        self.__initialize()
    
    def __initialize(self):
        print("Creating plotting data")
        self.__original_clusters = FileSystem.getExperimentClusters(self.__configuration_name)

    def setAttribute(self, attribute: Attribute):
        self.__attribute = attribute

    def setAlgorithm(self, algorithm):
        self.__algorithm = algorithm

    def getXY(self):
        if self.__attribute is None or self.__algorithm is None:
            print("Set u, attribute and algorithm first")
            return

        filtered_logs = self.__filterLogsByAlgorithm(self.__original_clusters, self.__algorithm)
        attribute_values = self.__filterLogsByAttribute(filtered_logs, self.__attribute)

        self.__attribute = None
        self.__algorithm = None

        attribute_values = collections.OrderedDict(sorted(attribute_values.items()))
        xy = (attribute_values.keys(), attribute_values.values())
        return xy

    def __filterClustersByU(self, clusters, u):
        return [cluster for cluster in clusters if cluster.getU() == u]

    def __filterLogsByAlgorithm(self, filtered_clusters, algorithm):
        filtered_logs = dict() #{u: log, u: log}
        for cluster in filtered_clusters:
            logs = cluster.getLogs()
            for log in logs:
                if log.getAlgorithm().lower() != algorithm.lower().replace(" ", ""):
                    continue

                filtered_logs[cluster.getU()] = log

        return filtered_logs

    def __filterLogsByAttribute(self, filtered_logs, attribute):
        attribute_values = dict()
        for u, log in filtered_logs.items():
            attribute_values[u] = log.getAttributeValue(attribute)

        return attribute_values
