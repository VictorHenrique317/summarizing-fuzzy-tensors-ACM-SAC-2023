from models.log import Log
from models.pattern import Pattern
from models.random_dataset import RandomDataset
from models.attribute import Attribute
from base.pattern_sorter import PatternSorter
from utils.commands import Commands


class Experiment():
    def __init__(self, experiment_path, iteration, correct_observations, u, dimension) -> None:
        self.__path:str = experiment_path
        self.__log = None
        self.__algorithm = None
        self.__iteration = iteration
        self.__correct_observations = correct_observations
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

    def getDataset(self):
        if self.__dataset is None:
            dataset_name = f"dataset-co{self.__correct_observations}.fuzzy_tensor"
            dataset_path = self.__path.replace(f"output/co{self.__correct_observations}/experiments/{self.__algorithm}.experiment", "")
            dataset_path = f"{dataset_path}/tensors/numnoise/{dataset_name}"
            self.__dataset = RandomDataset(dataset_path)

        return self.__dataset

    def rewritePatterns(self, new_patterns):
        # print(new_patterns)
        Commands.deleteFile(self.__path)

        with open(self.__path, 'w') as experiment_file:
            for pattern in new_patterns:
                # if pattern is None:
                #     continue
                experiment_file.write(f"{pattern.getPatternString()}\n")

    def sortPatterns(self, dataset):
        nb_sorted_patterns = self.getLog().getAttributeValue(Attribute.PATTERN_NUMBER)
        sorted_patterns = PatternSorter.sort(dataset, self, nb_sorted_patterns)
        self.rewritePatterns(sorted_patterns)

    def getPatterns(self):
        return (Pattern(pattern, self.__dimension) for pattern in open(self.__path))

    def getLog(self):
        return self.__log

    def getIteration(self):
        return self.__iteration

    def getCorrectObservations(self):
        return self.__correct_observations

    def getU(self):
        return self.__u
    
    def getAlgorithm(self):
        return self.__algorithm

    def getPath(self):
        return self.__path
