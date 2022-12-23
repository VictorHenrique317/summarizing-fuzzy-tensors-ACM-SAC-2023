from utils.commands import Commands
class Numnoise():
    def __init__(self, controller) -> None:
        self.__controller = controller

    def run(self, observations):
        dataset_size = self.__controller.getParameter("dataset_size")
        dataset_size = self.__controller.getParameter("dataset_size")
        dimension = len(dataset_size)
        dataset_size = " ".join([str(dataset_size[i]) for i in range(dimension)]).strip()

        tensor_name = "dataset.tensor"

        gennsets_folder = f"{self.__controller.current_iteration_folder}/tensors/gennsets"
        base_folder = self.__controller.current_iteration_folder
        base_folder += "/tensors/numnoise"

        fuzzy_name = f"dataset-co{observations}.fuzzy_tensor"
        
        command = f"cat {gennsets_folder}/{tensor_name} "
        command += f"| ../libs/num-noise/num-noise '{dataset_size}' "
        command += f"{observations} 0 > {base_folder}/{fuzzy_name}"
        print(command)
        Commands.execute(command)
        print("-"*120)
