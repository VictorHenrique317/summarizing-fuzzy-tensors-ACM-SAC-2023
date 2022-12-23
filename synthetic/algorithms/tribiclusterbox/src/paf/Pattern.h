// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PATTERN_H_
#define PATTERN_H_

#include <unordered_set>

#include "../utilities/CompletionMessage.h"
#include "../utilities/nSetHash.h"
#include "../utilities/InclusionResult.h"
#include "../common/PatternFileReader.h"

enum MaximizationResult { alreadyMaximal, maximized, alreadyFoundOrNegative };
enum ExtensionResult { extended, alreadyFound, infinitelyGrowing };

class Pattern
{
 public:
  Pattern(Pattern&& otherPattern);
  Pattern(vector<vector<unsigned int>>& nSet);

  Pattern& operator=(Pattern&& otherPattern);

  static void grow(const char* patternFileName, const char* patternDimensionSeparator, const char* patternElementSeparator, const AbstractRoughTensor* roughTensor, Trie& tensor, const unsigned int nbOfGrownPatterns, const bool isInputMaximal, const bool isIntermediaryPatternsOutput, const bool isVerbose);
  static void outputPatterns();
  static unsigned int setPatternsAsCandidateVariables();
  static vector<pair<vector<vector<unsigned int>>, long long>> getPatterns();
  static Trie& getTensor();

 private:
  vector<vector<unsigned int>> nSet;
  long long membershipSum;
  unsigned int dimensionIdOfNextElement;
  unsigned int nextElement;
  int membershipSumOnNextElement;
  unsigned long long rssDiff;

  static const AbstractRoughTensor* roughTensor;
  static Trie tensor;
  static vector<Pattern> patterns;
  static unordered_set<vector<vector<unsigned int>>, nSetHash<unsigned int>> visitedPatterns;
  static bool isIntermediaryPatternsOutput;

  friend ostream& operator<<(ostream& out, const Pattern& pattern);

  unsigned int area() const;
  bool subPatternOf(const Pattern& otherPattern); /* and output or move as candidate variable if true */
  bool subPatternOf(const vector<Pattern>::const_iterator begin, const vector<Pattern>::const_iterator end);
  InclusionResult subOrSuperPatternOf(Pattern& otherPattern); /* and output or move as candidate variable the sub-pattern (*this if sub, otherPattern if super) */
  bool findNextElement(const vector<vector<int>>& extensionSums); /* sets dimensionIdOfNextElement, nextElement, membershipSumOnNextElement, and returns whether the density decreases */
  bool extend(const vector<vector<int>>::iterator extensionSumsBegin); /* returns whether the extended patterns has already been found */
  MaximizationResult maximizeDensityAndFindNextElement();
  ExtensionResult extendAndFindNextElement();

  static void insertAndEraseSubPatterns(Pattern& pattern);
  static void overwriteWithSubsequentPattern(const vector<Pattern>::iterator patternToOverwriteIt, const vector<Pattern>::iterator subsequentPatternIt, const vector<Pattern>::iterator end);
  static vector<Pattern>::iterator eraseSubPatternsAndSetNextPatternsToGrow(const vector<Pattern>::iterator grownPatternEnd); /* erases, from grownPatternsEnd, every pattern that is a sub-pattern of some pattern before grownPatternsEnd, searches the pattern(s) having the smallest rssDiff and setting them first (before the returned iterator) */
  static void updateMinRssDiff(const vector<Pattern>::iterator patternIt, vector<Pattern>::iterator& minRssDiffBegin, vector<Pattern>::iterator& minRssDiffEnd);
  static void outputIntermediaryPatterns(vector<pair<vector<vector<unsigned int>>, long long>>& intermediaryPatterns);
  static void output(const vector<vector<unsigned int>>& nSet, const long long membershipSum);
  static void outputOrMoveAsCandidateVariable(vector<vector<unsigned int>>& nSet, const long long membershipSum);
};

#endif	/* PATTERN_H_ */
