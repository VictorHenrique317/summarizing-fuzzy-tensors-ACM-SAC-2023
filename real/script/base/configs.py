import json
import os
class Configs():
    __configs = None
    def __init__(self) -> None:
        pass

    @staticmethod
    def getAlgorithms():
        return ["Getf", "Cancer", "NclusterBox", "TriBiclusterBox", "NclusterBoxNoPerformanceImp"]
        
    @staticmethod
    def readConfigFile(path):
         with open(path, "r") as file:
            Configs.__configs = json.load(file)

    @staticmethod
    def setDimension(dimension):
        Configs.__dimension = dimension

    @staticmethod
    def getDimensions():
        return Configs.__dimension

    @staticmethod
    def getParameter(parameter):
        return Configs.__configs[parameter]
