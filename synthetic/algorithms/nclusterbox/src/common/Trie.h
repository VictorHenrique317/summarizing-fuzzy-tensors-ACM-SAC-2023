// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TRIE_H_
#define TRIE_H_

#include "SparseCrispTube.h"
#include "SparseFuzzyTube.h"

class Trie final : public AbstractData
{
 public:
  static bool is01;

  Trie();
  Trie(Trie&& otherTrie);
  Trie(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);
  Trie(vector<double>::const_iterator& membershipIt, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);
  Trie(vector<double>::const_iterator& membershipIt, const int unit, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);

  ~Trie();

  Trie& operator=(Trie&& otherTrie);

  void setTuple(const vector<unsigned int>::const_iterator idIt);
  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);
  void sortTubes();
  void clearAndFree();

  long long membershipSum(const vector<vector<unsigned int>>& nSet) const; /* assumes the overall sum fits in an int */
  long long nbOfPresentTuples(const vector<vector<unsigned int>>& nSet) const; /* assumes the sum on every slice fits in an int */
  vector<vector<int>> sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>& cardinalities) const;

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  void sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void updateSumsOnExtensionsAfterExtension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt, const vector<vector<int>>::iterator sumsInExtendedDimensionIt) const; /* the *dimensionIt relating to *sumsInExtendedDimensionIt must contain the extension (one single element) rather than the actual dimension of the n-set */
  long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const;
  long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes, const unsigned long long area, const int unit) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionBegin, const unsigned int increasedDimensionId, vector<vector<int>>& sums) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionBegin, const unsigned int decreasedDimensionId, vector<vector<int>>& sums) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionBegin, const unsigned int increasedDimensionId, vector<vector<int>>& sums, const int area, const int unit) const;

  static vector<int>::const_iterator maxExtension(const vector<int>& extensionsInDimension); /* extensionsInDimension must not only contain numeric_limits<unsigned int>::max(), i.e., the pattern must not contain every element of that dimension */

 private:
  vector<AbstractData*> hyperplanes;
};

#endif /*TRIE_H_*/
