import time
from utils.commands import Commands
class Gennsets():
    def __init__(self, controller) -> None:
        self.__controller = controller
    
    def __createSizesFile(self):
        pattern_size = self.__controller.getParameter("pattern_size")
        n_patterns = self.__controller.getParameter("n_patterns")

        dataset_size = self.__controller.getParameter("dataset_size")
        dimension = len(dataset_size)
        line = " ".join([str(pattern_size) for i in range(dimension)]).strip()

        with open(f"{self.__controller.current_iteration_folder}/tensors/gennsets/sizes.gennsets", 'w') as sizes_file:
            for n in range(n_patterns):
                sizes_file.write(f"{line}\n")

        print(f"Created {n_patterns} patterns with size {line}")

    def run(self):
        time.sleep(1)
        self.__createSizesFile()
        
        dataset_size = self.__controller.getParameter("dataset_size")
        dimension = len(dataset_size)
        dataset_size = " ".join([str(dataset_size[i]) for i in range(dimension)]).strip()

        tensor_name = "dataset.tensor"
        base_folder = self.__controller.current_iteration_folder
        base_folder += "/tensors/gennsets"

        command = f"../algorithms/gennsets/gennsets '{dataset_size}' "
        command += f"{base_folder}/sizes.gennsets {base_folder}/{tensor_name} "
        command += f"> {base_folder}/log.txt"
        print(command)
        Commands.execute(command)
        print(f"Tensor file created")
        print("-"*120)
