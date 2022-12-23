from post_analysis.quality import Quality
from models.attribute import Attribute


class QualityNormalizer:
    def __init__(self, dataset, clusters):
        self.__clusters = clusters
        self.__dataset = dataset
        self.__values = None
        self.__initialize()

    def __initialize(self):
        self.__values = [0]
        for cluster in self.__clusters:
            for experiment in cluster.getExperiments():
                log = experiment.getLog()
                quality_value = log.getAttributeValue(Attribute.QUALITY)
                truncated_quality_value = log.getAttributeValue(Attribute.TRUNCATED_QUALITY) # 0 if doesnt exist

                self.__values.append(quality_value)
                self.__values.append(truncated_quality_value)

    def normalize(self, value):
        return 1 - (value / max(self.__values))