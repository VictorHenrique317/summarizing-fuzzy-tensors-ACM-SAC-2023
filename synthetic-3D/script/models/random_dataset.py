import itertools
import numpy as np
import pandas as pd


class RandomDataset():
    def __init__(self, path):
        self.__path = path
        self.__matrix = self.__toMatrix()
        self.__tensor_density = self.__calculateTensorDensity()
        self.__empty_model_rss = self.__calculateEmptyModelRss()

    def path(self):
        return self.__path

    def getMatrix(self):
        return self.__matrix

    def getEmptyModelRss(self):
        return self.__empty_model_rss

    def getDimension(self):
        columns = dict()
        with open(self.__path) as database:
            for line in database:
                line = line.strip().split(" ")

                for column in range(len(line)):
                    value = line[column]
                    column_set = columns.get(column, set())
                    column_set.add(value)

                    columns[column] = column_set

        columns = {column: len(column_set) for column, column_set in columns.items()}
        return [size for size in columns.values()][:-1]

    def getDensity(self):
        return self.__tensor_density

    def __calculateTensorDensity(self):
        tensor_sum = 0
        tensor_cells = 0

        for dims, actual_value in np.ndenumerate(self.__matrix):
            tensor_sum += actual_value
            tensor_cells += 1

        tensor_density = tensor_sum / tensor_cells
        print(f"    Tensor density is: {tensor_density}")
        return tensor_density

    def __toMatrix(self):
        dataset = pd.read_csv(self.__path, sep=' ', header=None)
        dataset = dataset.iloc[:, :].values
        matrix = np.zeros(self.getDimension())

        nb_indices = dataset.shape[0]
        # print("========================================")
        print(f"    Dataset path: {self.__path}")
        # print("Loading dataset matrix into memory...")
        for line in range(nb_indices):
            index = [int(dimension) for dimension in dataset[:, :-1][line]]
            density = dataset[:, -1][line]
            replacer_string = f"matrix{index} = {density}"
            exec(replacer_string)

        # print("Done!")
        return matrix

    def __calculateEmptyModelRss(self):
        # print("Calculating empty model rss..")
        rss = 0
        for dims, actual_value in np.ndenumerate(self.__matrix):
            rss += (actual_value - self.__tensor_density) ** 2

        # print("Done!")
        print(f"    Empty model rss: {rss}")
        return rss

    def calculateTuplesDensity(self, pattern):  # tuples = [{}, {}, {}]
        indices = [index for index in itertools.product(*pattern)]
        density = 0
        area = 0
        for index in indices:
            index = [int(i) for i in index]
            matrix = self.__matrix
            density_string = f"matrix{index}"
            density += eval(density_string)
            area += 1
        
        density /= area
        return density