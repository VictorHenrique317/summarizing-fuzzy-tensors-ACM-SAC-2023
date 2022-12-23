import numpy as np
from scipy.io import savemat
class MatTranslator():
    def __init__(self, controller) -> None:
        self.__controller = controller

    def __toNumpy(self, fuzzy_path):
        dataset_size = self.__controller.getParameter("dataset_size")
        translated_tensor = np.zeros(dataset_size)
        # depth, row, column
        
        with open(fuzzy_path) as file:
            for line in file:
                line = [float(character) for character in line.split(" ")]
                value = line[-1] 
                dims = [int(dim) for dim in line[:-1]]

                replacer_string = f"translated_tensor{dims} = {value}"
                exec(replacer_string)

        return translated_tensor

    def run(self, dataset):
        fuzzy_path = dataset.path()

        # numpy_tensor = self.__toNumpy(fuzzy_path)
        numpy_tensor = np.load(f"{self.__controller.base_dataset_path}.npy")
        numpy_tensor = {"matrix": numpy_tensor, "label":"matrix"}

        mat_path = self.__controller.dataset_folder
        mat_path += "/retweets-sparser-2D-processed.mat"
        savemat(mat_path, numpy_tensor)
        print("Translated fuzzy tensor to mat matrix")
        print("-"*120)