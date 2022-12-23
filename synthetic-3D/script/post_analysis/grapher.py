import matplotlib

matplotlib.use('agg')
import matplotlib.pyplot as plt
import time
from post_analysis.plotting_data import PlottingData
from models.attribute import Attribute
from base.configs import Configs
import matplotlib.ticker
from post_analysis.experiment_analysis import ExperimentAnalysis
import json


class Grapher:
    def __init__(self, configuration_name, dimension, calculate_rss_evolution=False, calculate_quality=False) -> None:
        # self.__configs = Configs()
        self.__extra_curves = []
        self.__xscale = None
        self.__yscale = None
        self.__curves = None
        self.__attribute = None
        self.__ylimits = None
        self.__plotting_data = None
        self.__configuration_name = configuration_name
        self.__experiment_analysis = ExperimentAnalysis(self.__configuration_name)
        self.__dimension = dimension
        self.__initialize(calculate_rss_evolution, calculate_quality)

    def __initialize(self, calculate_rss_evolution, calculate_quality):
        if calculate_quality:
            self.__experiment_analysis.setQualityForExperimentClusters()

        if calculate_rss_evolution:
            self.__experiment_analysis.setRssEvolutionForExperimentClusters()

        self.__plotting_data = PlottingData(self.__configuration_name)

        algorithms = Configs.getAlgorithms()
        getf = algorithms[0]
        cancer = algorithms[1]
        nclusterbox = algorithms[2]
        tribiclusterbox = algorithms[3]
        nclusterboxminussect51 = algorithms[4]
        planted_patterns_number = "Number of planted patterns"

        self.__curves = {getf: "darkgreen", planted_patterns_number: "orange", nclusterbox: "blue",
                         cancer: "lime", tribiclusterbox: "red", nclusterboxminussect51: "slateblue"}

        self.__u_curves = {0.0: "deepskyblue", 0.6: "deepskyblue", 0.7: "blue", 0.8: "black"}

    def setAttribute(self, attribute: Attribute):
        self.__attribute = attribute
        self.__configureGraphForAttribute(attribute)

    def setYLimits(self, y_min, y_max):
        self.__ylimits = [y_min, y_max]

    def __configureGraphForAttribute(self, attribute: Attribute):
        if attribute == Attribute.PATTERN_NUMBER:
            self.__xscale = "log"
            self.__yscale = "log"
        elif attribute == Attribute.QUALITY:
            self.__xscale = "log"
            self.__yscale = "linear"
        elif attribute == Attribute.TRUNCATED_QUALITY:
            self.__xscale = "log"
            self.__yscale = "linear"
        elif attribute == Attribute.RUN_TIME:
            self.__xscale = "log"
            self.__yscale = "log"
        elif attribute == Attribute.RSS_EVOLUTION:
            self.__xscale = "linear"
            self.__yscale = "linear"
        else:
            print(f"No attribute configuration for {attribute.value}")

    def __configureCurve(self, x, y, curve, u, xlabel, ylabel, title, grouping_type, scatter=True, invert_x=True,
                         custom_x_ticks=None, custom_scatter=None):
        color = None
        if grouping_type == "algorithm":
            color = self.__curves[curve]
            plt.plot(x, y, color=color, label=curve)

        elif grouping_type == "u":
            color = self.__u_curves[u]
            plt.plot(x, y, color=color, label=f"u = {u}")

        else:
            raise ValueError(f"No grouping type: {grouping_type}")

        if scatter:
            plt.scatter(x, y, color=color)

        if custom_scatter is not None:
            for scatter in custom_scatter:
                plt.scatter(scatter[0], scatter[1], color=color)

        plt.legend()
        plt.grid()
        plt.title(title)
        plt.xlabel(xlabel)

        if invert_x:
            plt.xlim(max(x), min(x))

        plt.ylabel(ylabel)
        axis = plt.gca()
        axis.set_ylim(self.__ylimits)

        if self.__xscale == "log":
            plt.xscale(self.__xscale, base=2)
            # plt.xscale(self.__xscale, basex=2)
        elif self.__xscale == "linear":
            plt.xscale(self.__xscale)
        else:
            raise ValueError(f"No scale {self.__xscale}")

        plt.yscale(self.__yscale)
        axis.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        axis.get_yaxis().set_major_formatter(matplotlib.ticker.StrMethodFormatter('{x:.2f}'))

        if custom_x_ticks is not None:
            axis.set_xticks(custom_x_ticks)

    def __dataForCombinedRuntime(self, u):
        self.__plotting_data.setAlgorithm("Multidupehack")
        self.__plotting_data.setAttribute(Attribute.RUN_TIME)
        self.__plotting_data.setU(u)
        x1, y1 = self.__plotting_data.getXY()

        self.__plotting_data.setAlgorithm("Paf")
        self.__plotting_data.setAttribute(Attribute.RUN_TIME)
        self.__plotting_data.setU(u)
        x2, y2 = self.__plotting_data.getXY()

        y1 = [number for number in y1]
        y2 = [number for number in y2]
        print(len(y2))
        print(y1)
        print(y2)
        y3 = [y1[i] + y2[i] for i in range(len(y1))]
        return x1, y3

    def __isEmpty(self, x, y):
        return len(x) == 0 and len(y) == 0

    def __savePlottingData(self, x, y, base_folder, filename):
        path = f"{base_folder}/plotting_data/{filename}.json"
        data = {"x": list(x), "y": list(y)}
        json_object = json.dumps(data, indent=4)
        with open(path, "w") as outfile:
            outfile.write(json_object)

    def __drawRssEvolutionGraphs(self, save, figure_width, figure_height, folder):
        for algorithm in Configs.getAlgorithms():
            algorithm_folder = f"{folder}/rss-evolutions/{algorithm.lower().replace(' ', '-')}"
            algorithm_runned = False

            for correct_observations in Configs.getParameter("correct_obs"):
                fig, ax = plt.subplots()
                fig = plt.figure(figsize=(figure_width, figure_height))
                xlabel = "Nb. of patterns"
                y_label = "RSS"
                title = f"Elbow analysis for co={correct_observations}"

                filename = self.__attribute.value.lower().replace(" ", "-")
                filename = f"{filename}-for-co-{correct_observations}.png"
                path = f"{algorithm_folder}/{filename}"

                u = 0.0
                self.__plotting_data.setAlgorithm(algorithm)
                self.__plotting_data.setAttribute(self.__attribute)
                self.__plotting_data.setU(u)
                self.__plotting_data.setCorrectObservations(correct_observations)
                x, y = self.__plotting_data.getXY()

                if self.__isEmpty(x, y):  # algorithm not runned
                    continue

                if list(y)[0] == 0:
                    continue

                y = list(y)[0]
                x = [index + 1 for index, value in enumerate(y)]

                if self.__isEmpty(x, y):  # algorithm not runned
                    continue

                plotting_data_filename = f"synthetic-{self.__dimension}d-rssevolution#" \
                                            f"{algorithm.lower().replace(' ','')}-co{correct_observations}"
                self.__savePlottingData(x, y, folder, plotting_data_filename)

                algorithm_runned = True

                x_ticks = [tick for tick in range(0, max(x) + 1, 10)]
                x_ticks[0] = 1

                single_pattern = None
                elbow_point = Configs.getParameter("n_patterns")
                custom_scatter = []
                if len(y) >= elbow_point:  # scatter on the elbow point
                    elbow_point = (elbow_point, y[elbow_point - 1])
                    custom_scatter.append(elbow_point)

                elif len(y) == 1:  # scatter on the single pattern
                    single_pattern = (1, y[0])
                    custom_scatter.append(single_pattern)

                self.__configureCurve(x, y, algorithm, u, xlabel, y_label, title, "u", scatter=False,
                                        invert_x=False, custom_x_ticks=x_ticks, custom_scatter=custom_scatter)

                if algorithm_runned is False:
                    continue

                # self.__saveGraph(False, fig, path)
                self.__saveGraph(save, fig, path)

    def __drawUDependentGraphs(self, save, figure_width, figure_height, folder):
        u = 0.0
        fig, ax = plt.subplots()
        fig = plt.figure(figsize=(figure_width, figure_height))
        xlabel = "Nb. of correct observations"
        y_label = self.__attribute.value
        title = f"{self.__attribute.value}"

        for algorithm in Configs.getAlgorithms():
            self.__plotting_data.setAlgorithm(algorithm)
            self.__plotting_data.setAttribute(self.__attribute)
            self.__plotting_data.setU(u)
            x, y = self.__plotting_data.getXY()

            if self.__isEmpty(x, y):  # algorithm not runned
                continue

            plotting_data_filename = f"synthetic-{self.__dimension}d-{self.__attribute.value.lower().replace(' ','')}-" \
                                        f"#{algorithm.lower().replace(' ','')}-"
            self.__savePlottingData(x, y, folder, plotting_data_filename)

            self.__configureCurve(x, y, algorithm, u, xlabel, y_label, title, "algorithm")

        for curve in self.__curves:
            if self.__attribute == Attribute.PATTERN_NUMBER and curve == "Number of planted patterns":
                pattern_number = Configs.getParameter("n_patterns")
                x = Configs.getParameter("correct_obs")
                y = [pattern_number for i in range(len(x))]

                self.__configureCurve(x, y, curve, u, xlabel, y_label, title, "algorithm")

        filename = self.__attribute.value.lower().replace(" ", "-")
        filename = f"{filename}-for-u-{u}.png"
        path = f"{folder}/{filename}"

        self.__saveGraph(save, fig, path)

    def __saveGraph(self, save, fig, path):
        if save:
            plt.savefig(path)
            plt.close(fig)
        else:
            plt.show()

    def drawGraphs(self, folder, save):
        figure_width = 12
        figure_height = 9

        if self.__attribute == Attribute.RSS_EVOLUTION:
            self.__drawRssEvolutionGraphs(save, figure_width, figure_height, folder)
        else:
            self.__drawUDependentGraphs(save, figure_width, figure_height, folder)
