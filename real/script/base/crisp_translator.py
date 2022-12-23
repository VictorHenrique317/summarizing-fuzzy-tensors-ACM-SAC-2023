import re
import numpy as np

class CrispTranslator:
    def __init__(self, controller) -> None:
        self.__controller = controller

    def run(self):
        fuzzy_path = self.__controller.current_iteration_folder
        fuzzy_path += f"{self.__controller.dataset_folder}/retweets-sparser-processed.txt"
        
        crisp_tensor_path = self.__controller.current_iteration_folder
        crisp_tensor_path +=  f"{self.__controller.dataset_folder}/retweets-sparser-processed-crisp.txt"

        with open(fuzzy_path) as fuzzy_file:
            with open(crisp_tensor_path, 'w') as crisp_file:
                for line in fuzzy_file:
                    
                    line = [float(character) for character in line.split(" ")]
                    density = line[-1]
                    density = 1 if density > 0.5 else 0

                    line = [f"{int(value)}" for value in line]
                    line[-1] = f"{density}"

                    new_line = " ".join(line)
                    new_line += "\n"
                    crisp_file.write(new_line)

        print("Translated fuzzy tensor to crisp tensor")
        print("-"*120)