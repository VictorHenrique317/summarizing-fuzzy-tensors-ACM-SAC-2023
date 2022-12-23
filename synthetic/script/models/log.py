import re
import ast
from base.configs import Configs
from models.attribute import Attribute

from post_analysis.rss_evolution import RssEvolution


class Log():
    def __init__(self, path):
            self.path = path
            self.__attributes = None
            self.__algorithm = None
            # self.__configs = Configs()
            self.__initialize()

    def getAttributes(self):
        return self.__attributes
    
    def getAlgorithm(self):
        return self.__algorithm

    def __initialize(self):
        self.__setAlgorithmFromPath()

        data = dict() # {attribute: value}
        with open(self.path) as log:
            
            for line in log:
                match = re.findall("(.*):(.*)", line)
                if len(match) == 0 or len(line) == 0:
                    continue
                
                translated_attribute = self.__translateAttribute(match[0][0])
                if translated_attribute is None: # attribute not relevant
                    continue

                value = self.__translateValue(translated_attribute, match[0][1])
                # print(translated_attribute, value)
                data[translated_attribute] = value
        self.__attributes = data

    def __setAlgorithmFromPath(self):
        algorithm = self.path.split("/")[-1]
        algorithm = algorithm.split(".")[0]
        self.__algorithm = algorithm

    def __translateAttribute(self, attribute): # translates different names for same attribute 
        # attributes_dict = Configs.getParameter("plot_attributes")
        attributes_dict = {"Nb of patterns": "Nb of selected patterns", "Run time": "Total time",
                            "Selection time": "Selection time", "Quality": "Quality",
                            "Truncated quality": "Truncated quality", "RSS Evolution": "RssEvolution"}

        for translated_attribute, variants in attributes_dict.items():
            if self.__algorithm == "getf":
                if attribute == variants:
                    return translated_attribute     
            
            if self.__algorithm == "cancer":
                if attribute == variants:
                    return translated_attribute     

            if self.__algorithm == "nclusterbox":
                if attribute == variants:
                    return translated_attribute

            if self.__algorithm == "tribiterbox":
                if attribute == variants:
                    return translated_attribute

            if self.__algorithm == "nclusterboxnoperformanceimp":
                if attribute == variants:
                    return translated_attribute

            if attribute == translated_attribute: # generic case
                return translated_attribute

    def __translateValue(self, attribute, value):
        number_regex = "(\d*\.*\d*e*-*\+*\d*)"
        list_regex = "(\[.*\])"
        value = value.strip()

        if "[" in value: # rss evolution
            value = ast.literal_eval(value)
            return value
        else:  # other attributes
            value = re.findall(number_regex, value)[0]
            value = float(value)

        if attribute == "Memory (mb)":
            value /= 1000

        return value

    def deleteLastTwoLines(self):
        lines = None
        with open(self.path, 'r') as log:
            lines = [line for line in log]
            try: # novo
            	del lines[-1]
            	del lines[-2]
            except IndexError:
            	pass

        with open(self.path, 'w+') as new_log:
            for line in lines:
                new_log.write(line)

    def getAttributeValue(self, attribute:Attribute):
        return self.__attributes.get(attribute.value, 0)

    def writeAttribute(self, attribute, value):
        if attribute in self.__attributes: # delete last two lines
            # self.deleteLastTwoLines()
            raise ValueError(f"Attribute {attribute} already exists")
        with open(self.path, "a") as file:
            line = f"\n{attribute}:{value}\n"
            file.write(line)
        self.__initialize()


class AveragedLog():
    def __init__(self, algorithm, averaged_attributes) -> None:
        self.__averaged_attributes = averaged_attributes
        self.__algorithm = algorithm

    def getAttributeValue(self, attribute:Attribute):
        return self.__averaged_attributes.get(attribute.value, 0)

    def getAttributes(self):
        return self.__averaged_attributes

    def getAlgorithm(self):
        return self.__algorithm

    @staticmethod
    def average(log_groups): # twin logs from different iterations [[m_log, p_log], [m_log, p_log]]
        averaged_algorithm_attributes = dict() # {algorithm: {attr1:value1, attr2:value2}}
        average_rss_evolution_sizes = RssEvolution.calculateAverageRssEvolutionSize(log_groups)

        for log_group in log_groups:
            for log in log_group: # per algorithm
                algorithm = log.getAlgorithm()
                averaged_attributes = averaged_algorithm_attributes.setdefault(algorithm, dict())
                average_rss_evolution_size = average_rss_evolution_sizes.get(algorithm, 0)

                attributes = log.getAttributes()
                for attribute, value in attributes.items(): # sum phase
                    if type(value) == list: # rss evolution
                        averaged_list = averaged_attributes.setdefault(attribute, [])
                        if average_rss_evolution_size == 0: # not calculated]
                            continue

                        averaged_list = RssEvolution.sumRssEvolutionLists(averaged_list, value, average_rss_evolution_size)
                        averaged_attributes[attribute] = averaged_list
                        continue

                    averaged_value = averaged_attributes.setdefault(attribute, 0)
                    averaged_value += value
                    averaged_attributes[attribute] = averaged_value

        nb_iterations = len(log_groups)
        averaged_logs = []
        for algorithm, attributes in averaged_algorithm_attributes.items(): # division phase
            for attribute, value in attributes.items():
                if type(value) == list: # rss evolution
                    rss_evolution_list = attributes[attribute]
                    attributes[attribute] = [rss/nb_iterations for rss in rss_evolution_list]
                    continue

                attributes[attribute] = value/nb_iterations
            
            averaged_log = AveragedLog(algorithm, attributes)
            averaged_logs.append(averaged_log)

        return averaged_logs
