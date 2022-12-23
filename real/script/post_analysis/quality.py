from base.file_system import FileSystem
from models.pattern import Pattern
import numpy as np
from base.configs import Configs
import itertools
import time
from statistics import mean

from post_analysis.aicc import Aicc

from base.retweets_dataset import RetweetsDataset


class Quality():
    @staticmethod
    def __createWrostTensor(dataset_matrix):
        worst_tensor = dataset_matrix.copy()
        print("Creating worst tensor")
        for index, value in np.ndenumerate(dataset_matrix):
            index = list(index)
            if value >= 0.5:
                replacer_string = f"worst_tensor{index} = 0"
                exec(replacer_string)

            elif value < 0.5:
                replacer_string = f"worst_tensor{index} = 1"
                exec(replacer_string)

        print("Done")
        np.save("../datasets/worst-reconstruction.npy", worst_tensor)
        return worst_tensor

    @staticmethod
    def __getWorstReconstruction():
        return np.load("../datasets/worst-reconstruction.npy")

    @staticmethod
    def getWorstQuality(dataset_matrix):
        reconstruction_error = dataset_matrix - Quality.__getWorstReconstruction()
        return Quality.__norm(reconstruction_error)

    @staticmethod
    def __reconstructTensor(patterns, dimensions) -> np.ndarray:
        print("Reconstructing tensor...")
        reconstructed_tensor = np.zeros(dimensions)
        t0 = time.time()
        for i, pattern in enumerate(patterns):  # 42 minutos para iterar 15 milhões de padrões pegando densidades e indices (iterando indices)

            indices = pattern.getIndices()
            density = pattern.getDensity() if pattern.getDensity() is not None else 1

            for index in indices:
                index = tuple([int(dimension) for dimension in index])
                reconstructed_tensor[index] = density
                # replacer_string = f"reconstructed_tensor{index} += {density}"

            if i % 500_000 == 0:
                print(f"Patterns done: {i} , Delta time: {(time.time() - t0)/60 : .2f} minutes")

        return reconstructed_tensor

    @staticmethod
    def __norm(reconstruction_error):  # shape = (12, 170670, 29)
        norm = 0
        for i in range(reconstruction_error.shape[0]):
            hyperplane = reconstruction_error[i]  # shape = (170670, 29)
            norm += np.linalg.norm(hyperplane, 'fro')

        return norm

    # @staticmethod
    # def calculate(dataset_matrix, experiment, truncate_number=None):
    #     patterns = None
    #     if truncate_number is not None:  # truncate
    #         print(truncate_number)
    #         patterns = experiment.getFirstNPatterns(truncate_number)
    #     else:
    #         patterns = experiment.getPatterns()
    #
    #     reconstructed_tensor = Quality.__reconstructTensor(patterns, dataset_matrix.shape)
    #
    #     reconstruction_error = dataset_matrix - reconstructed_tensor
    #     reconstruction_error = Quality.__norm(reconstruction_error)
    #     return reconstruction_error

    @staticmethod
    def calculate(dataset, experiment, truncate_number=None):
        patterns = None
        empty_model_aicc = Aicc.calculate(dataset, [])

        if truncate_number is not None:  # truncate
            patterns = experiment.getFirstNPatterns(truncate_number)
        else:
            patterns = experiment.getPatterns()

        aicc = Aicc.calculate(dataset, patterns)
        quality = empty_model_aicc - aicc
        print("=======================================")
        print(f"    Algorithm: {experiment.getAlgorithm()}")
        print(f"    AICc: {aicc: ,}")
        print(f"    Quality: {quality: ,}")
        return quality  # maior = melhor
