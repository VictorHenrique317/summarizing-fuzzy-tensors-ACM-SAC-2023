// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_ROUGH_TENSOR_H_
#define DENSE_ROUGH_TENSOR_H_

#include "AbstractRoughTensor.h"
#include "ConstantShift.h"
#include "ExpectationShift.h"

class DenseRoughTensor final : public AbstractRoughTensor
{
 public:
  DenseRoughTensor(const DenseRoughTensor& otherDenseRoughTensor) = delete;
  DenseRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isVerbose);
  DenseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double constantShift);
  ~DenseRoughTensor();

  DenseRoughTensor& operator=(const DenseRoughTensor& otherDenseRoughTensor) const = delete;

  Trie getTensor() const;
  void setNoSelection();
  TrieWithPrediction projectTensor(const unsigned int nbOfPatternsHavingAllElements);

  double getAverageShift(const vector<vector<unsigned int>>& nSet) const;

 private:
  AbstractShift* shift;
  vector<double> memberships; /* non-empty if only if patterns are to be selected */
  /* PERF: a specific class for a 0/1 tensor where memberships are stored in a dynamic_bitset */

  void init(vector<FuzzyTuple>& fuzzyTuples);
  double updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships);
  double updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships);
  void updateNullModelQuadraticErrorAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships);
  void updateNullModelQuadraticErrorAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships);
};

#endif /*DENSE_ROUGH_TENSOR_H_*/
