// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "FuzzyTuple.h"

FuzzyTuple::FuzzyTuple(vector<unsigned int>& tupleParam, const double membershipParam): tuple(std::move(tupleParam)), membership(membershipParam)
{
}

FuzzyTuple::FuzzyTuple(const vector<vector<unsigned int>::const_iterator>& tupleIts, const double membershipParam): tuple(), membership(membershipParam)
{
  tuple.reserve(tupleIts.size());
  for (const vector<unsigned int>::const_iterator tupleIt : tupleIts)
    {
      tuple.push_back(*tupleIt);
    }
}

FuzzyTuple& FuzzyTuple::operator=(FuzzyTuple&& otherFuzzyTuple)
{
  tuple = std::move(otherFuzzyTuple.tuple);
  membership = otherFuzzyTuple.membership;
  return *this;
}

bool FuzzyTuple::operator==(const FuzzyTuple& otherFuzzyTuple) const
{
  return tuple == otherFuzzyTuple.tuple;
}

bool FuzzyTuple::operator<(const FuzzyTuple& otherFuzzyTuple) const
{
  return tuple > otherFuzzyTuple.tuple;
}

const vector<unsigned int>& FuzzyTuple::getTuple() const
{
  return tuple;
}

double FuzzyTuple::getMembership() const
{
  return membership;
}

double FuzzyTuple::getMembershipSquared() const
{
  return membership * membership;
}

unsigned int FuzzyTuple::getElementId(const unsigned int dimensionId) const
{
  return tuple[dimensionId];
}

unsigned int& FuzzyTuple::getElementId(const unsigned int dimensionId)
{
  return tuple[dimensionId];
}

void FuzzyTuple::reorder(const vector<unsigned int>& oldOrder2NewOrder)
{
  vector<unsigned int> newTuple(tuple.size());
  vector<unsigned int>::const_iterator newDimensionIdIt = oldOrder2NewOrder.begin();
  for (const unsigned int id : tuple)
    {
      newTuple[*newDimensionIdIt] = id;
      ++newDimensionIdIt;
    }
  tuple = std::move(newTuple);
}

bool FuzzyTuple::setNewIds(const vector<vector<unsigned int>>& oldIds2NewIds)
{
  vector<unsigned int>::iterator idIt = tuple.begin();
  for (const vector<unsigned int>& oldIds2NewIdsInDimension : oldIds2NewIds)
    {
      const unsigned int newId = oldIds2NewIdsInDimension[*idIt];
      if (newId == numeric_limits<unsigned int>::max())
	{
	  return false;
	}
      *idIt++ = newId;
    }
  return true;
}

void FuzzyTuple::shiftMembership(const double shift)
{
  membership -= shift;
}
