// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_CRISP_TUBE_H_
#define DENSE_CRISP_TUBE_H_

#include <limits>
#include <boost/dynamic_bitset.hpp>

#include "AbstractData.h"

using namespace boost;

class DenseCrispTube final : public AbstractData
{
 public:
  DenseCrispTube(vector<double>::const_iterator& membershipIt);
  DenseCrispTube(const vector<unsigned int>& sparseTube);

  void setTuple(const vector<unsigned int>::const_iterator idIt);

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& nbOfPresentTuples) const;
  void sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator numbersOfPresentTuplesIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const;

  static void setSize(const unsigned int size);

 private:
  dynamic_bitset<> tube;		/* indicates the presence of the shifted tuple */
  /* TODO: test whether storing is faster than computing tube.find_first() every time it is needed */

  static unsigned int size;
};

#endif /*DENSE_CRISP_TUBE_H_*/
