from typing import List

from base.file_system import FileSystem
from post_analysis.quality import Quality
from base.configs import Configs
from utils.quality_normalizer import QualityNormalizer

from models.attribute import Attribute

from post_analysis.aicc import Aicc

from post_analysis.rss_evolution import RssEvolution


class ExperimentAnalysis:
    def __init__(self, configuration_name, dataset) -> None:
        self.__configuration_name = configuration_name
        self.__dataset = dataset
        self.__normalizer = None # implementar

    def __setQualityForExperiment(self, experiment):
        truncate_number = None
        quality = Quality.calculate(self.__dataset, experiment, truncate_number=truncate_number)
        experiment.getLog().writeAttribute("Quality", quality)

    def __setQualityForExperimentCluster(self, cluster):
        cluster.loadExperiments()
        for experiment in cluster.getExperiments():
            self.__setQualityForExperiment(experiment)

    def __normalizeQualities(self, experiment_clusters):
        print("Normalizing qualities...")

        normalizer = QualityNormalizer(self.__dataset.getMatrix(), experiment_clusters)
        for cluster in experiment_clusters:
            for experiment in cluster.getExperiments():
                log = experiment.getLog()
                log.writeAttribute(Attribute.QUALITY.value, normalizer.normalize(log.getAttributeValue(Attribute.QUALITY)))

                if experiment.getAlgorithm == "paf":
                    log.writeAttribute(Attribute.TRUNCATED_QUALITY.value, normalizer.normalize(log.getAttributeValue(Attribute.TRUNCATED_QUALITY)))

    def setQualityForExperimentClusters(self):
        print(f"Calculating qualities for {self.__configuration_name}...")
        experiment_clusters = FileSystem.getExperimentClusters(self.__configuration_name)
        counter = 0

        empty_model_aicc = Aicc.calculate(self.__dataset, [])
        print(f"Empty model AICc: {empty_model_aicc}")

        for cluster in experiment_clusters:
            counter += 1
            self.__setQualityForExperimentCluster(cluster)
            print(f"{100*counter/len(experiment_clusters): .2f}% done")

        # self.__normalizeQualities(experiment_clusters)
    def __setRssEvolutionForExperiment(self, experiment):
        rss_evolution: List = RssEvolution.calculate(self.__dataset, experiment.getPatterns())
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


    