// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef SPARSE_CRISP_TUBE_H_
#define SPARSE_CRISP_TUBE_H_

#include <algorithm>

#include "DenseCrispTube.h"

class SparseCrispTube final : public AbstractData
{
 public:
  SparseCrispTube();

  bool isFullSparseTube() const;
  void setTuple(const vector<unsigned int>::const_iterator idIt);
  DenseCrispTube* getDenseRepresentation() const;
  void sortTubes();

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& nbOfPresentTuples) const;
  void sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator numbersOfPresentTuplesIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;

  static long long getDefaultMembership();
  static void setDefaultMembership(const int defaultMembership);
  static void setDefaultMembershipAndSizeLimit(const int defaultMembership, const unsigned int sizeLimit);

 private:
  vector<unsigned int> tube;		/* every value is an id */

  static long long defaultMembership;
  static unsigned int sizeLimit;
};

#endif /*SPARSE_CRISP_TUBE_H_*/
