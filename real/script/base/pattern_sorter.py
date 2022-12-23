from typing import List
from models.pattern import Pattern
from base.retweets_dataset import RetweetsDataset
from post_analysis.rss_evolution import RssEvolution
import sys


class PatternSorter:
    # @staticmethod
    # def sort(dataset: RandomDataset, experiment, nb_sorted_patterns) -> List[Pattern]:
    #     sorted_patterns = []
    #     sorted_patterns_indices = []
    #     nb_sortings = 0
    #
    #     minimum_rss_value = sys.maxsize
    #
    #     print("Sorting patterns...")
    #     while nb_sortings < nb_sorted_patterns:  # sorts all patterns
    #         minimum_rss_pattern = None
    #         minimum_temp_model_rss = sys.maxsize
    #
    #         counter = 0
    #         for pattern in experiment.getPatterns():  # sorting one pattern
    #             counter += 1
    #
    #             if counter % 10 == 0:
    #                 print("@", end='', flush=True)
    #
    #             if counter - 1 in sorted_patterns_indices:  # pattern already selected
    #                 continue
    #
    #             temp_patterns = sorted_patterns + [pattern]
    #             temp_model_rss = RssEvolution.calculateModelRss(dataset, temp_patterns)[0]
    #
    #             if temp_model_rss < minimum_temp_model_rss:
    #                 # for knowing which model has the least rss, don't affect selection
    #                 minimum_temp_model_rss = temp_model_rss
    #
    #             if temp_model_rss <= minimum_rss_value:  # select pattern
    #                 minimum_rss_value = temp_model_rss
    #                 minimum_rss_pattern = pattern
    #                 sorted_patterns_indices.append(counter - 1)
    #                 break
    #
    #         if minimum_rss_pattern is None:  # rss only gets bigger
    #             print(f" => WARNING: RSS increase detected on sorting, minimum_rss_value: {minimum_rss_value}, minimum_temp_model_rss: {minimum_temp_model_rss} ")
    #             break
    #
    #         nb_sortings += 1
    #         sorted_patterns.append(minimum_rss_pattern)  # sorted
    #         print(f" => {nb_sortings} of {nb_sorted_patterns} sortings done...")
    #
    #     return sorted_patterns

    @staticmethod
    def sort(dataset: RetweetsDataset, experiment, nb_sorted_patterns) -> List[Pattern]:
        patterns = tuple([pattern for pattern in experiment.getPatterns()])
        pattern_nb = len(patterns)
        sorted_patterns = []
        # sorted_patterns_indices = []
        nb_sortings = 0

        minimum_rss_value = sys.maxsize

        print("Sorting patterns...")
        while nb_sortings < nb_sorted_patterns:  # sorts all patterns
            if nb_sortings >= pattern_nb:
                break

            minimum_rss_pattern = None  # for decreasing sorting

            minimum_temp_model_rss = sys.maxsize  # for increasing sorting
            minimum_temp_model_pattern = None
            minimum_temp_model_pattern_index = None

            # print("\n")
            # print("######################################")
            counter = 0
            for pattern in patterns:  # sorting one pattern
                # print("=========================================")
                counter += 1

                if pattern in sorted_patterns:
                    # print("Pattern already selected")
                    continue

                if counter % 10 == 0:
                    print("@", end='', flush=True)

                # if counter - 1 in sorted_patterns_indices:  # pattern already selected
                #     print("Pattern already selected")
                #     continue

                temp_patterns = sorted_patterns + [pattern]
                temp_model_rss = RssEvolution.calculateModelRss(dataset, temp_patterns)[0]  # time dump
                # print(f"    Current pattern rss: {temp_model_rss}")
                # print(f"    Minimum rss (current patterns): {minimum_temp_model_rss}")
                # print(f"    Minimum rss (global): {minimum_rss_value}")

                if temp_model_rss <= minimum_temp_model_rss:
                    # for knowing which model has the least rss between the current models
                    # of all current possibilites adding this pattern increases the rss the less

                    minimum_temp_model_rss = temp_model_rss
                    minimum_temp_model_pattern = pattern
                    minimum_temp_model_pattern_index = counter - 1

                if temp_model_rss <= minimum_rss_value:  # select pattern
                    minimum_rss_value = temp_model_rss
                    minimum_rss_pattern = pattern
                    # sorted_patterns_indices.append(counter - 1)
                    # break

            if minimum_rss_pattern is None:  # rss only gets bigger
                # print(f"Pattern {minimum_temp_model_pattern} with rss: {minimum_temp_model_rss} increases rss (selected)")
                sorted_patterns.append(minimum_temp_model_pattern)
                # sorted_patterns_indices.append(minimum_temp_model_pattern_index)

            else:  # rss can get smaller
                # print(f"Pattern {minimum_rss_pattern} with rss: {minimum_rss_value} decreases rss (selected)")
                sorted_patterns.append(minimum_rss_pattern)  # sorted

            nb_sortings += 1
            print(f" => {nb_sortings} of {pattern_nb} sortings done...")

        return sorted_patterns
