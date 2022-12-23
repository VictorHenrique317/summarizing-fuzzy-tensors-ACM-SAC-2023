from typing import List

import numpy as np
from models.pattern import Pattern
from math import log

from base.retweets_dataset import RetweetsDataset


class Aicc():
    @staticmethod
    def calculatePatternRSS(dataset_matrix, pattern: Pattern):
        predicted_value = pattern.getDensity()
        pattern_rss = 0

        for dims in pattern.getIndices():
            dims = [int(dim) for dim in dims]

            actual_value = dataset_matrix[dims[0], dims[1], dims[2]]
            pattern_rss += (actual_value - predicted_value) ** 2

        return pattern_rss

    @staticmethod
    def calculateCellRSS(dataset_matrix, dims, prediction):
        actual_value = dataset_matrix[dims[0], dims[1], dims[2]]
        return (actual_value - prediction) ** 2

    @staticmethod
    def calculateModelRss(dataset, patterns: List[Pattern]):
        total_rss = dataset.getEmptyModelRss()
        prediction_matrix = dict()

        dataset_matrix = dataset.getMatrix()
        counter = 0
        for pattern in patterns:
            counter += 1
            if counter % 100_000 == 0:
                print(f"    {counter} done...")

            current_prediction = pattern.getDensity()
            for dims in pattern.getIndices():
                dims = tuple([int(dim) for dim in dims])

                if dims in prediction_matrix: # predição será o max
                    previous_prediction = prediction_matrix[dims]
                    prediction_matrix[dims] = max(previous_prediction, current_prediction)

                else:
                    prediction_matrix[dims] = current_prediction

        for dims, prediction in prediction_matrix.items():

            cell_prediction_rss = Aicc.calculateCellRSS(dataset_matrix, dims, prediction)
            cell_lambda0_rss = Aicc.calculateCellRSS(dataset_matrix, dims, dataset.getDensity())

            total_rss -= cell_lambda0_rss
            total_rss += cell_prediction_rss

        return total_rss, counter

    @staticmethod
    def calculate(dataset, patterns: List[Pattern], nb_parameters=0):
        dataset_matrix = dataset.getMatrix()
        total_rss, counter = Aicc.calculateModelRss(dataset, patterns)

        k = counter + nb_parameters + 1# temporario
        n = dataset_matrix.size
        aic = 2 * k + n * log(total_rss)

        aicc = 2 * k ** 2 + 2 * k
        aicc /= n - k - 1
        aicc += aic
        return aicc
