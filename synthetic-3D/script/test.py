from time import sleep
import json
from base.file_system import FileSystem
from models.attribute import Attribute

from base.configs import Configs
from base.pattern_sorter import PatternSorter
from models.random_dataset import RandomDataset
from models.experiment import Experiment
from post_analysis.quality import Quality
from models.pattern import Pattern
from utils.commands import Commands

Configs.readConfigFile("configs/configs.json")

iteration = 1
co = 16
u = 0.0
algorithm = "tubeinputnclusterbox"
configuration_name = Configs.getParameter("configuration_name")

fuzzy_tensor_path = f"../iterations/{configuration_name}/{iteration}/tensors/numnoise/dataset-co{co}.fuzzy_tensor"
experiment_path = f"../iterations/{configuration_name}/{iteration}/output/co{co}-u{u}/experiments/{algorithm}.experiment"
experiment = Experiment(experiment_path, iteration, co, u, 2)
dataset = RandomDataset(fuzzy_tensor_path)
# experiment_clusters = FileSystem.getExperimentClusters(Configs.getParameter("configuration_name"))

#
#
# data = dict()
# counter = 0
# patterns = experiment.getPatterns()
# considerated_patterns = []
#
# for pattern in patterns:
#     counter += 1
#     considerated_patterns.append(pattern)
#     x = counter
#     y = RssEvolution.calculateModelRss(dataset, considerated_patterns)
#
#     data[x] = y
#
# print(data)


################################
# sorting_blacklist = ['nclusterbox', 'slownclusterbox', 'tubeinputnclusterbox', 'tubeinputslownclusterbox']
#
# for cluster in experiment_clusters:
#     cluster.loadExperiments()
#     for experiment in cluster.getExperiments():
#         # log = experiment.getLog()
#         #
#         # rss_evolution = log.getAttributeValue(Attribute.RSS_EVOLUTION)
#         # rss_evolution = [item[0] for item in rss_evolution]
#         # log.writeAttribute(Attribute.RSS_EVOLUTION.value, rss_evolution)
#
#         if experiment.getAlgorithm() in sorting_blacklist:
#             continue
#
#         print("================")
#         print(experiment.getPath())
#         experiment.sortPatterns(experiment.getDataset())

#####################################
# sorted_patterns = PatternSorter.sort(dataset, experiment, 205)
# sorted_patterns = PatternSorter.sort(dataset, experiment, 10)

# print(experiment.getPath())
# print(sorted_patterns)


####################################################
# cluster_nb = len(experiment_clusters)
# counter = 0
# for cluster in experiment_clusters:
#     counter += 1
#
#     print(f"{counter} of {cluster_nb} done...")
#     cluster.loadExperiments()
#     for experiment in cluster.getExperiments():
#         if experiment.getAlgorithm().lower() != "getf":
#             continue
#
#         log = experiment.getLog()
#         pattern_nb = len([pattern for pattern in experiment.getPatterns()])
#
#         lines = []
#         new_lines = []
#         with open(log.path, 'r') as log_file:
#             lines = [line for line in log_file]
#
#         for line in lines:
#             if "Nb of patterns" in line:
#                 line = f"Nb of patterns: {pattern_nb}\n"
#
#             new_lines.append(line)
#
#         Commands.deleteFile(log.path)
#         with open(log.path, 'w') as log_file:
#             for line in new_lines:
#                 log_file.write(line)
######################################################

found_patterns = [pattern for pattern in experiment.getPatterns()]
planted_patterns = FileSystem.getPlantedPatterns(iteration, configuration_name, dataset)
# mapping = dict()

# for planted_pattern in planted_patterns:
#     most_similar = Quality.findMostSimilarFoundPattern(planted_pattern, found_patterns)
#     most_similar_index = found_patterns.index(most_similar)

#     # mapping[planted_pattern] = (most_similar_index, most_similar) # <======
#     mapping[(most_similar_index, most_similar)] = planted_pattern # <======

# for planted_pattern, index_found_tuple in mapping.items():
#     index = index_found_tuple[0]
#     found_pattern = index_found_tuple[1]
#     jaccard = planted_pattern.jaccardIndex(found_pattern)
#     print(f"Found pattern {found_pattern.getPatternString()[:9]} in line {index+1} has jaccard index:{jaccard}")

# for index_found_tuple, planted_pattern in mapping.items():
#     index = index_found_tuple[0]
#     found_pattern = index_found_tuple[1]
#     jaccard = planted_pattern.jaccardIndex(found_pattern)
#     print(f"Found pattern {found_pattern.getPatternString()[:9]} in line {index+1} has jaccard index:{jaccard}")

print("===============")
# dimension = 2
# union_tuplas = set()
# for pattern in found_patterns[:]:
#     union_tuplas.union(pattern.union(union_tuplas))

# print(len(union_tuplas))

quality = Quality.calculate(experiment, planted_patterns, 10)
print(quality)

# problematic_pattern = found_patterns[0].get()
# equal_tuples = [set() for i in range(2)]

# for pattern in found_patterns[1:]:
#     pattern = pattern.get()
#     # current_tuples = [pattern[0], pattern[1], pattern[2]]

#     for index, one_tuple in enumerate(pattern):
#         problematic_pattern_one_tuple = problematic_pattern[index] # same tuples

#         for dimension in problematic_pattern_one_tuple:
#             if dimension in one_tuple:
#                 equal_tuples[index].add(dimension)

# print("======================")
# print(len(problematic_pattern[0]))
# print(len(equal_tuples[0]))
# print("--------------------------")
# print(len(problematic_pattern[1]))
# print(len(equal_tuples[1]))

# print(len(equal_tuples[0]) * len(equal_tuples[1]))
# print("--------------------------")

# found = Pattern("638,861,507,75,273,275,58,981,316,904,908,412,93,641,287,660,169,237,65,98,543,712,628,416,637,705,70,782,624,131,509,777,607,207,479,353,989,749,767,311,807,686,604,542,271,318,159,650,858,129,492,979,447,559,15,22,826,859,30,445,245,132,362,733,178,724,911,392,66,347,673,802,614,47,662,786,357,923,817,468,572,580,728,567,853,249,966,484,354,116,551,598,210,825,626,758,119,842,14,622,405,827,104,68,133,675,459,427,241,339,839,146,201,303,778,23,336,281,481,690,730,766,377,565,314,220,380,185,89,340,310,321,941,244,26,860,11,556,246,997,813,18,796,863,366,301,820,963,945,644,959,155,805,561,334,436,491,376,387,680,433,375,33,800,37,190,788,232,723,797,435,584,291,231,742,899,302,344,695,236,693,762,496,400,111,581,101,449,715,795,348,349,752,1,829,326,678,872,951,510,821,180,294,174,230,933,272,282,672,134,515,429,145,109,801,676,323,725,182,631,140,183,107,430,173,811,331,465,611,121,773,248,901,378,855,835,157,871,415,501,865,12,748,288,956,421,653,877,409,184,903,691,545,619,214,753,31,698,181,51,371,280,731,532,260,721,494,576,156,737,295,950,601,38,679,603,625,218,352,654,984,570,952,983,898,769,208,381,612,17,651,583,663,471,596,526,228,402,454,115,126,350,267,818,697,560,69,189,537,374,472,149,685,327,585,729,958,659,177,864,921,593,781,411,450,67,780,902,746,19,779,498,719,372,76,255 247,373,750,504,53,625,444,944,811,165,516,981,795,547,101,338,54,719,724,450,354,592,135,976,152,481,794,883,2,789,467,967,257,96,299,202,536,290,451,648,293,193,534,321,382,292,773,549,148,267,961,396,405,323,915,435,69,20,389,522,129,384,177,615,182,799,928,858,355,897,110,762,216,689,556,340,70,495,156,612,972,568,898,102,478,851,105,230,826,106,11,194,404,681,66,371,654,796,115,579,249,960,934,456,562,977,729,283,920,51,533,756,5,50,528,17,344,48,953,111,210,565,593,669,613,749,260,617,899,190,366,764,844,6,577,97,296,497,341,569,523,39,300,150,56,198,83,816,902,841,26,153,125,360,606,653,469,857,649,407,583,231,430,685,91,303,511,32,250,73,823,98,118,428,999,573,738,840,157,696,401,595,356,958,580,959,747,682,690,380,839,736,893,558,948,601,305,974,548,437,319,7,706,916,187,699,645,542,471,665,215,831,892,530,716,560,276,170,178,415,912,661,25,940,588,786,126,181,635,872,86,751,24,572,673,316,636,730,424,754,15,924,537,219,390,345,443,985,945,525,979,753,628,905,458,414,402,288,492,239,477,779,519,501,643,642,670,113,885,169,61,986,64,671,302,78,741,278,46,315,33,609,55,361,324,47,77,72,931,412,206,952,185,312,631,862,36,731,539,822,217,968,220,406,864,74,65,792,880,512,211,123,891,349,128,611,538,505,433,766,154,759,124,869,375,420,803,554,698,100,705,282,29,38 0.411114", 2)
# planted = Pattern("11,14,18,23,30,65,67,89,93,98,107,119,126,129,131,149,157,159,169,178,180,185,201,207,208,214,218,228,241,267,275,287,291,294,310,314,321,353,357,372,381,387,392,427,433,436,447,450,454,459,465,471,479,481,494,507,509,526,559,567,603,622,624,625,626,637,638,641,650,679,680,712,719,733,762,769,780,781,782,796,800,805,818,820,826,842,858,860,861,898,902,904,908,933,945,950,956,959,989,997 2,7,20,29,33,36,38,47,53,55,64,66,69,70,77,97,102,106,110,113,129,135,156,177,182,190,194,206,210,217,230,231,249,257,290,292,296,319,321,323,340,344,371,373,389,402,412,414,437,467,477,478,495,501,504,511,512,516,519,522,530,534,547,548,554,565,569,580,583,588,609,615,631,643,649,669,689,724,741,750,756,759,773,779,786,794,799,803,844,858,897,902,915,931,948,952,960,967,981,986", 2, density=1)

# print(found.jaccardIndex(planted))
