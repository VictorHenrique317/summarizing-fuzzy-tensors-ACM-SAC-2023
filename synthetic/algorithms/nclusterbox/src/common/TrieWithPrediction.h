// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TRIE_WITH_PREDICTION_H_
#define TRIE_WITH_PREDICTION_H_

#include "TubeWithPrediction.h"

class TrieWithPrediction final : public AbstractDataWithPrediction
{
 public:
  TrieWithPrediction();
  TrieWithPrediction(TrieWithPrediction&& otherTrieWithPrediction);
  TrieWithPrediction(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);
  TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd);

  ~TrieWithPrediction();

  TrieWithPrediction& operator=(TrieWithPrediction&& otherTrieWithPrediction);

  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);

  int density(const vector<vector<unsigned int>>& nSet) const;
  void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const;
  long long sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>& tuples, const int density) const;
  void sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& sum) const;
  long long addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density);
  void addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError);
  long long addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density);
  void addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError);
  void addFirstPatternToModel(const vector<vector<unsigned int>>& tuples, const int density);
  void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density);
  void addPatternToModel(const vector<vector<unsigned int>>& tuples, const int density);
  void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density);
  long long addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>& tuples, const int density);
  void addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& quadraticErrorVariation);
  long long quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density) const;
  void quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) const;
  pair<long long, long long> lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>& tuples, const int density) const;
  void lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& lowerBound, long long& delta) const;
  TrieWithPrediction subtensor(const vector<vector<unsigned int>>& tuples, const int density) const;
  TrieWithPrediction* subtensor(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) const;
  long long quadraticErrorVariation(const vector<vector<unsigned int>> tuples, const int density) const;
  void quadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& delta) const;

 private:
  vector<AbstractDataWithPrediction*> hyperplanes;
};

#endif /*TRIE_WITH_PREDICTION_H_*/
