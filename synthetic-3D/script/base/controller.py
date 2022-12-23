from numpy import True_
from base.gennsets import Gennsets
from base.numnoise import Numnoise
from base.configs import Configs
from base.numpy_translator import NumpyTranslator
from base.mat_translator import MatTranslator
from models.attribute import Attribute
from utils.commands import Commands
from post_analysis.grapher import Grapher
from base.file_system import FileSystem
from multiprocessing import Process
from models.experiment import Experiment
from models.random_dataset import RandomDataset
from base.crisp_translator import CrispTranslator


class Controller:
    def __init__(self) -> None:
        self.algorithms = []
        self.__sorting_blacklist = ['nclusterbox', 'triclusterbox', 'nclusterboxnoperformanceimp']
        #self.__sorting_blacklist = []
        self.__configs_folder = "configs"

        self.__gennsets = Gennsets(self)
        self.__numnoise = Numnoise(self)
        self.__crisp_translator = CrispTranslator(self)
        self.__numpy_translator = NumpyTranslator(self)
        self.__mat_translator = MatTranslator(self)

        self.__current_iteration_number = None
        self.__current_configuration_name = None
        self.current_iteration = None
        self.current_iteration_folder = None
        self.current_experiment = None
        self.current_dataset = None
        self.current_dataset_path = None

        self.__skip_gennsets = False
        self.__calculate_rss_evolution = False
        self.__calculate_quality = False

    def addAlgorithm(self, algorithm):
        if algorithm not in self.algorithms:
            self.algorithms.append(algorithm)
    
    def __runIterations(self):

        for iteration in range(1, self.__current_iteration_number+1):
            print("@"*120 + f" ITERATION = {iteration}")
            FileSystem.createIterationFolders(iteration, self.__current_configuration_name)
            self.current_iteration = iteration
            self.current_iteration_folder = f"../iterations/{self.__current_configuration_name}/{iteration}"
            if self.__skip_gennsets is False:
                self.__gennsets.run()

            for observations in Configs.getParameter("correct_obs"):
                print("#"*120 + f" CORRECT OBSEVATIONS = {observations}")
                if self.__skip_gennsets is False:
                    self.__numnoise.run(observations)
                    # self.__crisp_translator.run(observations)
                    self.__numpy_translator.run(observations)
                    self.__mat_translator.run(observations)

                self.current_dataset_path = f"{self.current_iteration_folder}/tensors/numnoise"
                self.current_dataset_path = f"{self.current_dataset_path}/dataset-co{observations}.fuzzy_tensor"

                self.current_dataset = RandomDataset(self.current_dataset_path)
                dimension = len(self.current_dataset.getDimension())

                u = 0.0
                self.current_experiment = f"co{observations}"
                FileSystem.createExperimentFolder(iteration, self.current_experiment, self.__current_configuration_name)

                for algorithm in self.algorithms:
                    if algorithm.hasTimedOut(u):
                        continue

                    timedout = algorithm.run(u, observations, Configs.getParameter("timeout"))
                    if timedout:
                        algorithm.timedOut(u)
                        FileSystem.deleteUFromAllIterations(self.__current_configuration_name, u, algorithm.name)
                    else:
                        if algorithm.name not in self.__sorting_blacklist:
                            experiment = Experiment(algorithm.experiment_path, iteration, observations, u, dimension)
                            experiment.sortPatterns(self.current_dataset)
                    print("-"*120)

    def __resetAlgorithms(self):
        for algorithm in self.algorithms:
            algorithm.resetTimeOutInfo()

    def initiateSession(self):
        delete_iterations = str(input("Delete previous iterations? Y/N: ")).strip().lower()
        if delete_iterations == "y":
            FileSystem.deleteIterationFolders()
        elif delete_iterations == "n":
            self.__skip_gennsets = True

        FileSystem.deletePostAnalysisFolder()

        for config_file in Commands.listFolder(self.__configs_folder):
            Configs.readConfigFile(f"{self.__configs_folder}/{config_file}")
            self.__current_configuration_name = Configs.getParameter("configuration_name")
            self.__current_iteration_number = Configs.getParameter("nb_iterations")
            self.__resetAlgorithms()
            self.__runIterations()
                    
    def __analyseConfiguration(self, configuration_name, save):
        FileSystem.createPostAnalysisFolder(configuration_name)
        FileSystem.createRssEvolutionFolders(configuration_name)

        post_analysis_folder = f"../post_analysis/{configuration_name}"
        dimension = len(Configs.getParameter("dataset_size"))
        grapher = Grapher(configuration_name, dimension, calculate_rss_evolution=self.__calculate_rss_evolution,
                          calculate_quality=self.__calculate_quality)

        print("Plotting pattern nb graph")
        grapher.setAttribute(Attribute.PATTERN_NUMBER)
        grapher.setYLimits(0.6, 2_000_000)
        grapher.drawGraphs(post_analysis_folder, save)

        print("Plotting run time graph")
        grapher.setAttribute(Attribute.RUN_TIME)
        grapher.setYLimits(1e-3, 3000)
        grapher.drawGraphs(post_analysis_folder, save)

        print("Plotting quality graph")
        grapher.setAttribute(Attribute.QUALITY)
        grapher.setYLimits(0, 1.1)
        grapher.drawGraphs(post_analysis_folder, save)
        grapher.drawGraphs(post_analysis_folder, save)

        print("Plotting RSS Evolution graph")
        grapher.setAttribute(Attribute.RSS_EVOLUTION)
        grapher.setYLimits(0, 80_000)
        grapher.drawGraphs(post_analysis_folder, save)

    def initiatePostAnalysis(self, save=True):
        print("#"*120)

        delete_post_analysis = str(input("Delete post analysis folder? Y/N: ")).strip().lower()
        if delete_post_analysis == "y":
            FileSystem.deletePostAnalysisFolder()

        calculate_rss_evolution = str(input("Calculate RSS evolution? Y/N: ")).strip().lower()
        if calculate_rss_evolution == "y":
            self.__calculate_rss_evolution = True

        calculate_quality = str(input("Calculate quality? Y/N: ")).strip().lower()
        if calculate_quality == "y":
            self.__calculate_quality = True

        for config_file in Commands.listFolder(self.__configs_folder):
            Configs.readConfigFile(f"{self.__configs_folder}/{config_file}")
            self.__current_configuration_name = Configs.getParameter("configuration_name")
            
            print(f"Initiating post analysis for {self.__current_configuration_name}...")
            self.__analyseConfiguration(self.__current_configuration_name, save=save)

    def getParameter(self, parameter):
        return Configs.getParameter(parameter)
