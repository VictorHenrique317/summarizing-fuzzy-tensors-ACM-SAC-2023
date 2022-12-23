from typing import List

import numpy as np
from models.pattern import Pattern
from math import log
from post_analysis.rss_evolution import RssEvolution
from base.random_dataset import RandomDataset


class Aicc:

    @staticmethod
    def calculate(dataset:RandomDataset, patterns: List[Pattern], nb_parameters=0):
        dataset_matrix = dataset.getMatrix()
        total_rss, counter = RssEvolution.calculateModelRss(dataset, patterns)

        k = counter + nb_parameters + 1 # lambda_0
        n = dataset_matrix.size
        aic = 2 * k + n * log(total_rss)

        aicc = 2 * k ** 2 + 2 * k
        aicc /= n - k - 1
        aicc += aic
        return aicc
