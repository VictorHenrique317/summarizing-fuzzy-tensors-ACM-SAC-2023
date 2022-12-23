// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FUZZY_TUPLE_H_
#define FUZZY_TUPLE_H_

#include <vector>
#include <limits>

using namespace std;

class FuzzyTuple
{
 public:
  FuzzyTuple(const FuzzyTuple& otherFuzzyTuple) = default;
  FuzzyTuple(vector<unsigned int>& tuple, const double membership);
  FuzzyTuple(const vector<vector<unsigned int>::const_iterator>& tupleIts, const double membership);

  FuzzyTuple& operator=(FuzzyTuple&& otherFuzzyTuple);

  bool operator==(const FuzzyTuple& otherFuzzyTuple) const;
  bool operator<(const FuzzyTuple& otherFuzzyTuple) const;

  const vector<unsigned int>& getTuple() const;
  double getMembership() const;
  double getMembershipSquared() const;
  unsigned int getElementId(const unsigned int dimensionId) const;
  unsigned int& getElementId(const unsigned int dimensionId);
  void reorder(const vector<unsigned int>& oldOrder2NewOrder);
  bool setNewIds(const vector<vector<unsigned int>>& oldIds2NewIds);
  void shiftMembership(const double shift);

 private:
  vector<unsigned int> tuple;
  double membership;
};

#endif /*FUZZY_TUPLE_H_*/
