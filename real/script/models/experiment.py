from models.log import Log
from models.pattern import Pattern
import itertools
from utils.commands import Commands
from models.attribute import Attribute
from base.pattern_sorter import PatternSorter

from base.retweets_dataset import RetweetsDataset


class Experiment():
    def __init__(self, experiment_path, u, dimension) -> None:
        self.__path:str = experiment_path
        # self.__patterns = []
        self.__log = None
        self.__algorithm = None
        self.__u = u
        self.__dimension = dimension
        self.__dataset = None
        self.__initialize()

    def __initialize(self):
        algorithm = self.__path.split("/")[-1].split(".")[0]
        self.__algorithm = algorithm

        log_name = f"{algorithm}.log"
        log_path = self.__path.replace(f"/experiments/{algorithm}.experiment", "")
        log_path = f"{log_path}/logs/{log_name}"
        self.__log = Log(log_path)

    def rewritePatterns(self, new_patterns):
        Commands.deleteFile(self.__path)
        print("Rewriting ordered patterns...")

        with open(self.__path, 'w') as experiment_file:
            for pattern in new_patterns:
                experiment_file.write(f"{pattern.getPatternString()}\n")

    def sortPatterns(self, dataset):
        nb_sorted_patterns = self.getLog().getAttributeValue(Attribute.PATTERN_NUMBER)
        sorted_patterns = PatternSorter.sort(dataset, self, nb_sorted_patterns)
        self.rewritePatterns(sorted_patterns)

    # def getDataset(self):
    #     if self.__dataset is None:
    #         self.__dataset = RetweetsDataset()
    #
    #     return self.__dataset

    def getPatterns(self):
        return (Pattern(pattern, self.__dimension) for pattern in open(self.__path))

    def getFirstNPatterns(self, n):
        return itertools.islice(self.getPatterns(), n)

    def getLog(self):
        return self.__log

    def getU(self):
        return self.__u
    
    def getAlgorithm(self):
        return self.__algorithm

    def getDimension(self):
        return self.__dimension

    def getPath(self):
        return self.__path
