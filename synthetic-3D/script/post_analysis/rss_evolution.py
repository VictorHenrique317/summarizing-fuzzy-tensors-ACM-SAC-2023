from typing import List

from models.random_dataset import RandomDataset
from models.pattern import Pattern

from models.attribute import Attribute


class RssEvolution:

    @staticmethod
    def sumRssEvolutionLists(list1, list2, average_rss_evolution_size):
        final_list = [0 for i in range(0, average_rss_evolution_size)]
        max_list1_index = len(list1) - 1
        max_list2_index = len(list2) - 1

        for index in range(0, average_rss_evolution_size):
            if index <= max_list1_index:
                final_list[index] += list1[index]
            else: # adds last element
                if len(list1) == 0:
                    final_list[index] += 0
                else:
                    final_list[index] += list1[-1]

        for index in range(0, average_rss_evolution_size):
            if index <= max_list2_index:
                final_list[index] += list2[index]
            else: # adds last element
                if len(list2) == 0:
                    final_list[index] += 0
                else:
                    final_list[index] += list2[-1]

        return final_list

    @staticmethod
    def calculateAverageRssEvolutionSize(log_groups) -> dict:  # twin logs from different iterations [[m_log, p_log], [m_log, p_log]]
        average_rss_evolution_size = dict()  # {algorithm1: size, algorithm2: size}
        nb_iterations = len(log_groups)

        for log_group in log_groups:
            for log in log_group:
                rss_evolution = log.getAttributeValue(Attribute.RSS_EVOLUTION)

                if rss_evolution == 0:
                    continue

                algorithm = log.getAlgorithm()
                average_size = average_rss_evolution_size.setdefault(algorithm, 0)
                average_size += len(rss_evolution)
                average_rss_evolution_size[algorithm] = average_size

        average_rss_evolution_size = {algorithm: round(value/nb_iterations) for algorithm, value in average_rss_evolution_size.items()}
        return average_rss_evolution_size

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
    def calculateCellRSS(dataset_matrix, dims, prediction, dimension):
        actual_value = None

        if dimension == 2:
            actual_value = dataset_matrix[dims[0], dims[1]]
        elif dimension == 3:
            actual_value = dataset_matrix[dims[0], dims[1], dims[2]]
        elif dimension == 4:
            actual_value = dataset_matrix[dims[0], dims[1], dims[2], dims[3]]
        else:
            raise ValueError(f"Dimension {dimension} not supported")

        return (actual_value - prediction) ** 2

    @staticmethod
    def calculateModelRss(dataset:RandomDataset, patterns: List[Pattern]):
        total_rss = dataset.getEmptyModelRss()
        prediction_matrix = dict()
        dimension = len(dataset.getDimension())

        dataset_matrix = dataset.getMatrix()
        counter = 0
        for pattern in patterns:
            counter += 1

            current_prediction = pattern.getDensity()
            for dims in pattern.getIndices():
                dims = tuple([int(dim) for dim in dims])

                if dims in prediction_matrix: # predição será o max
                    previous_prediction = prediction_matrix[dims]
                    prediction_matrix[dims] = max(previous_prediction, current_prediction)

                else:
                    prediction_matrix[dims] = current_prediction

        for dims, prediction in prediction_matrix.items():

            cell_prediction_rss = RssEvolution.calculateCellRSS(dataset_matrix, dims, prediction, dimension)
            cell_lambda0_rss = RssEvolution.calculateCellRSS(dataset_matrix, dims, dataset.getDensity(), dimension)

            total_rss -= cell_lambda0_rss
            total_rss += cell_prediction_rss

        return total_rss, counter

    @staticmethod
    def calculate(dataset:RandomDataset, patterns:List[Pattern], max_pattern_nb=20):
        rss_evolution = [] # adicionando padrões da primeira linha a ultima
        considerated_patterns = []
        counter = 0
        for pattern in patterns:
            counter += 1

            if counter > max_pattern_nb:
                break

            print(f"    {counter} patterns done...")
            if counter % 100_000 == 0:
                print(f"    {counter} patterns done...")

            considerated_patterns.append(pattern)
            current_rss = RssEvolution.calculateModelRss(dataset, considerated_patterns)
            rss_evolution.append(current_rss)

        return rss_evolution
