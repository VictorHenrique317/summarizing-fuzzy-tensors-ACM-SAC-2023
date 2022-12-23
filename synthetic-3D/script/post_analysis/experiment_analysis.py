from typing import List

from base.file_system import FileSystem
from post_analysis.quality import Quality
from base.configs import Configs
from post_analysis.rss_evolution import RssEvolution
from models.attribute import Attribute


class ExperimentAnalysis:
    def __init__(self, configuration_name) -> None:
        self.__configuration_name = configuration_name

    def __setQualityForExperiment(self, experiment):
        iteration = experiment.getIteration()
        dataset = experiment.getDataset()
        planted_patterns = FileSystem.getPlantedPatterns(iteration, self.__configuration_name, dataset)

        truncate_number = Configs.getParameter("n_patterns")
        quality = Quality.calculate(experiment, planted_patterns, truncate_number=truncate_number)
        experiment.getLog().writeAttribute("Quality", quality)

    def __setQualityForExperimentCluster(self, cluster):
        cluster.loadExperiments()
        for experiment in cluster.getExperiments():
            self.__setQualityForExperiment(experiment)

    def setQualityForExperimentClusters(self):
        print(f"Calculating qualities for {self.__configuration_name}...")

        experiment_clusters = FileSystem.getExperimentClusters(self.__configuration_name)
        counter = 0
        for cluster in experiment_clusters:
            counter += 1
            self.__setQualityForExperimentCluster(cluster)
            print(f"\r{100 * counter / len(experiment_clusters): .2f}% done")

    def __setRssEvolutionForExperiment(self, experiment):
        rss_evolution: List = RssEvolution.calculate(experiment.getDataset(), experiment.getPatterns())
        rss_evolution = [item[0] for item in rss_evolution]
        experiment.getLog().writeAttribute("RssEvolution", rss_evolution)

    def __setRssEvolutionForExperimentCluster(self, cluster):
        cluster.loadExperiments()
        counter = 0
        for experiment in cluster.getExperiments():
            print()
            print(f"    Experiment: {experiment.getPath()}, nb_patterns: {experiment.getLog().getAttributeValue(Attribute.PATTERN_NUMBER)}")
            counter += 1
            self.__setRssEvolutionForExperiment(experiment)
            # print(f"\r{100 * counter / len(cluster.getExperiments()): .2f} experiments done")

    def setRssEvolutionForExperimentClusters(self):
        print(f"Calculating RSS evolution for {self.__configuration_name}...")

        experiment_clusters = FileSystem.getExperimentClusters(self.__configuration_name)
        counter = 0

        for cluster in experiment_clusters:
            counter += 1
            print("============================")
            print(f"{counter} cluster of {len(experiment_clusters)}:")

            self.__setRssEvolutionForExperimentCluster(cluster)
            # print(f"\r{100 * counter / len(experiment_clusters): .2f}% done")
