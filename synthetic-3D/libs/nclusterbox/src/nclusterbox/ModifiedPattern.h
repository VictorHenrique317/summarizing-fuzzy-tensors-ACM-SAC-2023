// Copyright 2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MODIFIED_PATTERN_H_
#define MODIFIED_PATTERN_H_

#include "../common/AbstractRoughTensor.h"

#ifdef NCLUSTERBOX_REMEMBERS
#include <unordered_set>
#include "../utilities/nSetHash.h"
#endif

enum NextStep { insert, erase, stop };

class ModifiedPattern
{
 public:
  static void setContext(const AbstractRoughTensor* roughTensor, Trie& tensor, const bool isIntermediaryPatternsOutput);
  static void clearAndFree();

  static void modify(vector<vector<unsigned int>>& nSet);
  static void grow(vector<vector<unsigned int>>& nSet);

 private:
  static vector<vector<unsigned int>> nSet;
  static unsigned long long area;
  static long long membershipSum;
  static vector<vector<int>> sumsOnHyperplanes;

  static NextStep nextStep;
  static double bestG;
  static vector<vector<unsigned int>>::iterator bestDimensionIt;
  static vector<int>::const_iterator bestSumIt;

  static const AbstractRoughTensor* roughTensor;
  static bool isIntermediaryPatternsOutput;
  static Trie tensor;

#ifdef NCLUSTERBOX_REMEMBERS
  static unordered_set<vector<vector<unsigned int>>, nSetHash<unsigned int>> visitedPatterns;
#endif

  static void init(vector<vector<unsigned int>>& nSet);
  static const vector<vector<unsigned int>> getFirstNonInitialAndSubsequentInitial();
  static bool doStep(); // returns whether to go on
  static void considerAsIntermediary();
  static void considerAsFinal();

#ifdef ASSERT
  static void assertAreaAndSums();
#endif
};

#endif	/* MODIFIED_PATTERN_H_ */
