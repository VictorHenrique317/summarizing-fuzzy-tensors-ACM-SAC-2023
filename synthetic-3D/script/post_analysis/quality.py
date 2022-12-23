from base.file_system import FileSystem
from models.pattern import Pattern
from base.configs import Configs
import itertools

class Quality():

    @staticmethod
    def __findMostSimilarFoundPattern(planted_pattern, found_patterns):
        # returns the most similar found pattern to a given planted one
        most_similar_pattern = None
        higher_jaccard = -1

        for found_pattern in found_patterns:
            jaccard_index = found_pattern.jaccardIndex(planted_pattern)
            if jaccard_index >= higher_jaccard:
                higher_jaccard = jaccard_index
                most_similar_pattern = found_pattern
        return most_similar_pattern

    @staticmethod
    def findMostSimilarFoundPattern(planted_pattern, found_patterns):
        return Quality.__findMostSimilarFoundPattern(planted_pattern, found_patterns)

    @staticmethod
    def __multiplePatternUnion(patterns):
        # dimension = len(Configs.getParameter("dataset_size"))
        # union = Pattern("", dimension)
        # for pattern in patterns:
        #     union = union.union(pattern)
        # return union
        
        union_tuplas = set()
        for pattern in patterns:
            union_tuplas = pattern.union(union_tuplas)

        return union_tuplas
        

    # @staticmethod
    # def __multiplePatternUnionArea(patterns):
    #     return Quality.__multiplePatternUnion(patterns).area()

    @staticmethod
    def calculate(experiment, planted_patterns, truncate_number=None):
        try:
            empty_test = experiment.getPatterns()
            next(empty_test)
        except StopIteration: # no patterns found by the algorithm
            return 0 # zero quality

        all_p_intersection_argmax = []
        for planted_pattern in planted_patterns:
            if truncate_number is None:
                most_similar_found = Quality.__findMostSimilarFoundPattern(planted_pattern, experiment.getPatterns())
            else:
                found_patterns = [pattern for index, pattern in enumerate(experiment.getPatterns()) if index < truncate_number]
                most_similar_found = Quality.__findMostSimilarFoundPattern(planted_pattern, found_patterns)
                # print("================================")
                # print(f"Planted pattern: {planted_pattern.getPatternString()[:9]}")
                # print(f"Most similar pattern: {most_similar_found.getPatternString()[:9]}, density = {most_similar_found.getDensity()}, jaccard = {most_similar_found.jaccardIndex(planted_pattern)}")


            p_intersection_argmax = most_similar_found.intersection(planted_pattern.getIndices())
            # print(f"Intersection: {len(p_intersection_argmax)}")
            all_p_intersection_argmax.append(p_intersection_argmax)

        numerator = len(Quality.__multiplePatternUnion(all_p_intersection_argmax))

        planted_patterns_union = Quality.__multiplePatternUnion(planted_patterns)

        if truncate_number is None:
            found_patterns_union = Quality.__multiplePatternUnion(experiment.getPatterns())
        else:
            found_patterns = (pattern for index, pattern in enumerate(experiment.getPatterns()) if index < truncate_number)
            found_patterns_union = Quality.__multiplePatternUnion(found_patterns)

        # denominator = planted_patterns_union.unionArea(found_patterns_union)
        denominator = len(planted_patterns_union.union(found_patterns_union))
        return numerator / denominator
        

        

