import numpy as np
import re
class NumpyTranslator():
    def __init__(self, controller) -> None:
        self.__controller = controller

    def run(self, observations):
        dataset_size = self.__controller.getParameter("dataset_size")
        fuzzy_path = self.__controller.current_iteration_folder
        fuzzy_path += f"/tensors/numnoise/dataset-co{observations}.fuzzy_tensor"
        translated_tensor =  np.zeros(dataset_size)
        # depth, row, column
        
        with open(fuzzy_path) as file:
            for line in file:
                line = [float(character) for character in line.split(" ")]
                value = line[-1] 
                value = 1 if value > 0.5 else 0
                dims = [int(dim) for dim in line[:-1]]

                replacer_string = f"translated_tensor{dims} = {value}"
                exec(replacer_string)

        print("Translated fuzzy tensor to numpy tensor")
        output_folder = self.__controller.current_iteration_folder
        output_folder += f"/tensors/numpy"
        filename = fuzzy_path.split("/")[-1]
        filename = re.sub(".fuzzy_tensor", "", filename)

        translated_tensor_path = f"{output_folder}/{filename}"
        np.save(translated_tensor_path, translated_tensor)
        print("-"*120)