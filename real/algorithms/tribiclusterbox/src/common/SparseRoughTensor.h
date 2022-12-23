// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef SPARSE_ROUGH_TENSOR_H_
#define SPARSE_ROUGH_TENSOR_H_

#include "AbstractRoughTensor.h"

class SparseRoughTensor final : public AbstractRoughTensor
{
 public:
  SparseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double shift);

  Trie getTensor() const;
  void setNoSelection();
  TrieWithPrediction projectTensor(const unsigned int nbOfPatternsHavingAllElements);

  double getAverageShift(const vector<vector<unsigned int>>& nSet) const;

 private:
  vector<FuzzyTuple> fuzzyTuples; /* non-empty if only if patterns are to be selected */
  /* PERF: a specific class for a 0/1 tensor where memberships are not stored */
  const double shift;
};

#endif /*SPARSE_ROUGH_TENSOR_H_*/
