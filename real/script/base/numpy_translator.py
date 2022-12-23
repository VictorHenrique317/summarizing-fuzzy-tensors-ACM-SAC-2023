import numpy as np
import re
class NumpyTranslator():
    def __init__(self, controller) -> None:
        self.__controller = controller

    def run(self, dataset):
        translated_tensor = np.zeros(dataset.getDimension())
        dataset_path = dataset.path()
        # depth, row, column
        
        with open(dataset_path) as file:
            for line in file:
                line = [float(character) for character in line.split(" ")]
                value = line[-1] 
                value = 1 if value > 0.5 else 0
                dims = [int(dim) for dim in line[:-1]]

                replacer_string = f"translated_tensor{dims} = {value}"
                exec(replacer_string)

        print("Translated dataset to numpy tensor")
        output_folder = self.__controller.dataset_folder
        filename = dataset_path.split("/")[-1]
        filename = re.sub(".txt", "", filename)

        translated_tensor_path = f"{output_folder}/{filename}"
        np.save(translated_tensor_path, translated_tensor)
        print("-"*120)