// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_TUBE_H_
#define DENSE_TUBE_H_

#include <limits>

#include "AbstractData.h"

class DenseFuzzyTube final : public AbstractData
{
 public:
  DenseFuzzyTube(vector<double>::const_iterator& membershipIt, const int unit);
  DenseFuzzyTube(const vector<pair<unsigned int, int>>& sparseTube, const int defaultMembership);

  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const;
  void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const;
  void sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;

  static void setSize(const unsigned int size);

 private:
  vector<int> tube;		/* every value is membership * unit */

  static unsigned int size;
};

#endif /*DENSE_TUBE_H_*/
