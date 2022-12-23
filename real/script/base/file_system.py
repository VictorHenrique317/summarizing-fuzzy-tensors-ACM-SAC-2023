from email.mime import base
from models.experiment import Experiment
import time
from base.configs import Configs
from models.pattern import Pattern
from utils.commands import Commands
from models.experiment_cluster import ExperimentCluster
import re

class FileSystem:
    @staticmethod
    def createPostAnalysisFolder(configuration_name):
        post_analysis_folder = f"../post_analysis/{configuration_name}"
        plotting_data_folder = f"../post_analysis/{configuration_name}/plotting_data"
        Commands.createFolders([post_analysis_folder, plotting_data_folder])

    @staticmethod
    def createRssEvolutionFolders(configuration_name):
        post_analysis_folder = f"../post_analysis/{configuration_name}"
        folder_path = f"{post_analysis_folder}/rss-evolutions"
        Commands.createFolder(folder_path)

    @staticmethod
    def deletePostAnalysisFolder():
        post_analysis_folder = "../post_analysis"
        Commands.deleteFolder(post_analysis_folder)

    @staticmethod
    def createIterationFolder():
        Commands.createFolder("../iteration")

    @staticmethod
    def deleteIterationFolder():
        Commands.deleteFolder("../iteration")

    @staticmethod
    def createExperimentFolder(experiment, configuration_name):
        experiment_folder = f"../iteration/{configuration_name}/output/{experiment}/experiments"
        log_folder = f"../iteration/{configuration_name}/output/{experiment}/logs"

        folders = [experiment_folder, log_folder]
        Commands.createFolders(folders)

    @staticmethod
    def delete(folder):
        Commands.deleteFolder(folder)

    @staticmethod
    def getExperimentClusters(configuration_name) -> list:
        experiment_clusters = []
        iteration_folder = f"../iteration/{configuration_name}"
        print(f"Getting clusters of configuration {configuration_name}...")
        output_folder = f"{iteration_folder}/output"

        for cluster_folder in Commands.listFolder(output_folder):
            u = float(re.search("u(\d\.\d*)", cluster_folder)[1])
            base_folder = f"{output_folder}/{cluster_folder}"
            cluster = ExperimentCluster(base_folder, u)
            experiment_clusters.append(cluster)

        return experiment_clusters

    @staticmethod
    def getConfigurationNames():
        configuration_names = []
        configs_folder = "configs"
        for config_file in Commands.listFolder(configs_folder):
            Configs.readConfigFile(f"{configs_folder}/{config_file}")
            configuration_names.append(Configs.getParameter("configuration_name"))

        return configuration_names

    @staticmethod
    def getClusterFolderByU(base_folder, u):
        filtered_clusters = []
        pattern = f"u{u}"
        for cluster_folder in Commands.listFolder(base_folder):
            if re.search(pattern, cluster_folder) is not None: # found
                filtered_clusters.append(cluster_folder)
        return filtered_clusters

    @staticmethod
    def deleteExperiment(configuration_name, u, algorithm_name):
        base_folder = f"../iteration/{configuration_name}/output"
        cluster_folder = FileSystem.getClusterFolderByU(base_folder, u)[0]

        experiment_path = f"{base_folder}/{cluster_folder}/experiments/{algorithm_name}.experiment"
        log_path = f"{base_folder}/{cluster_folder}/logs/{algorithm_name}.log"

        Commands.deleteFile(experiment_path)
        Commands.deleteFile(log_path)