import itertools


class Pattern():
    def __init__(self, pattern, dimension, density=None) -> None:
        self.__dimension = dimension
        self.__density = density
        self.__pattern = self.__formatPattern(pattern)

    def __formatPattern(self, pattern):
        if type(pattern) == list: # already formated
            return pattern

        pattern = pattern.replace("\n", "").strip()
        if len(pattern) == 0: # empty pattern
            return [set() for i in range(self.__dimension)]

        pattern = pattern.split(" ")
        if len(pattern) > self.__dimension: # tem densidade
            if self.__density is None:
                self.__density = float(pattern[-1])
            del pattern[-1] # deletes the density of the pattern found by paf
        else:
            if self.__density is None:
                # self.__density = 1
                raise RuntimeError("Pattern density is None")

        # [['str', 'str'], ['str', 'str']]
        return [list(numbers.split(",")) for numbers in pattern]

    def area(self):
        pattern_area = 1
        for ith_tuple in self.__pattern:
            pattern_area *= len(ith_tuple)
        return pattern_area

    # def intersection(self, pattern): # returns a pattern that is the intersection of both
    #     intersection = []
    #     pattern = pattern.get()
    #     for i in range(self.__dimension):
    #         ith_tuple1 = set(self.__pattern[i]) # {'1', '2'}
    #         ith_tuple2 = set(pattern[i]) # {'1', '3'}

    #         intersection.append(ith_tuple1.intersection(ith_tuple2))
    #     return Pattern(intersection, self.__dimension)

    # def union(self, pattern):
    #     union = []
    #     pattern = pattern.get()        
        
    #     for i in range(self.__dimension):
    #         ith_tuple1 = set(self.__pattern[i]) # {'1', '2'}
    #         ith_tuple2 = set(pattern[i]) # {'1', '3'}

    #         union.append(ith_tuple1.union(ith_tuple2))
    #     return Pattern(union, self.__dimension)

    def intersection(self, tuples):
        tuplas1 = {tuple(tupla) for tupla in self.getIndices()}
        # tuplas2 = {tuple(tupla) for tupla in pattern.getIndices()}
        intersection_tuplas = tuplas1.intersection(tuples)

        # intersection_pattern = [set() for i in range(self.__dimension)]
        # for tupla in intersection_tuplas:
        #     for i, dimension in enumerate(tupla):
        #         intersection_pattern[i].add(dimension)

        # return Pattern(intersection_pattern, self.__dimension)

        return intersection_tuplas

    def union(self, tuples):
        tuplas1 = {tuple(tupla) for tupla in self.getIndices()}
        # tuplas2 = {tuple(tupla) for tupla in pattern.getIndices()}
        union_tuplas = tuplas1.union(tuples)

        # union_pattern = [set() for i in range(self.__dimension)]
        # for tupla in union_tuplas:
        #     for i, dimension in enumerate(tupla):
        #         union_pattern[i].add(dimension)
        
        # return Pattern(union_pattern, self.__dimension)
        return union_tuplas

    # def intersectionArea(self, pattern):
    #     return self.intersection(pattern).area()

    # def unionArea(self, pattern):
    #     return self.union(pattern).area()

    def jaccardIndex(self, pattern):
        # intersection = self.intersectionArea(pattern)
        intersection = len(self.intersection(pattern.getIndices()))
        # union = self.unionArea(pattern)
        union = len(self.union(pattern.getIndices()))
        return intersection / union # 0 <= return <= 1

    def setDensity(self, density):
        self.__density = density

    def get(self):
        return self.__pattern

    def getPatternString(self):
        pattern_string = [",".join(pattern_tuple) for pattern_tuple in self.__pattern]
        pattern_string = " ".join(pattern_string)
        pattern_string = f"{pattern_string} {self.__density}"

        return pattern_string

    def getDimension(self):
        return self.__dimension

    def getDensity(self):
        return self.__density

    def getIndices(self): # Cartesian product
        return [index for index in itertools.product(*self.__pattern)]

    
