// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseRoughTensor.h"

SparseRoughTensor::SparseRoughTensor(vector<FuzzyTuple>& fuzzyTuplesParam, const double shiftParam): fuzzyTuples(std::move(fuzzyTuplesParam)), shift(shiftParam)
{
}

Trie SparseRoughTensor::getTensor() const
{
  Trie tensor(cardinalities.begin(), cardinalities.end());
  if (Trie::is01)
    {
      for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
	{
	  tensor.setTuple(fuzzyTuple.getTuple().begin());
	}
      tensor.sortTubes();
      return tensor;
    }
  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
    {
      tensor.setTuple(fuzzyTuple.getTuple().begin(), unit * fuzzyTuple.getMembership());
    }
  tensor.sortTubes();
  return tensor;
}

void SparseRoughTensor::setNoSelection()
{
  fuzzyTuples.clear();
  fuzzyTuples.shrink_to_fit();
}

TrieWithPrediction SparseRoughTensor::projectTensor(const unsigned int nbOfPatternsHavingAllElements)
{
  // Update cardinalities, ids2Labels, candidateVariables, and fuzzyTuples
  const vector<vector<unsigned int>> oldIds2NewIds = projectMetadata(nbOfPatternsHavingAllElements, true);
  vector<FuzzyTuple>::iterator end = fuzzyTuples.end();
  for (vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin(); fuzzyTupleIt != end; )
    {
      if (fuzzyTupleIt->setNewIds(oldIds2NewIds))
	{
	  ++fuzzyTupleIt;
	}
      else
	{
	  *fuzzyTupleIt = std::move(*--end);
	}
    }
  fuzzyTuples.erase(end, fuzzyTuples.end());
  // Compute negative/positive memberships of elements in first dimension and the quadratic error of the null model
  double totalShiftOnElementInFirstDimension = shift;
  const vector<unsigned int>::const_iterator cardinalityEnd = cardinalities.end();
  for (vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin(); ++cardinalityIt != cardinalityEnd; )
    {
      totalShiftOnElementInFirstDimension *= *cardinalityIt;
    }
  double quadraticError = totalShiftOnElementInFirstDimension * cardinalities.front() * shift;
  const double squaredShift = shift * shift;
  vector<double> elementPositiveMemberships(cardinalities.front());
  vector<double> elementNegativeMemberships(cardinalities.front(), totalShiftOnElementInFirstDimension);
  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
    {
      const unsigned int elementId = fuzzyTuple.getElementId(0);
      const double membership = fuzzyTuple.getMembership();
      if (membership > 0)
	{
	  elementPositiveMemberships[elementId] += membership;
	  elementNegativeMemberships[elementId] -= shift;
	}
      else
	{
	  elementNegativeMemberships[elementId] -= membership + shift;
	}
      quadraticError += membership * membership - squaredShift;
    }
  // Compute unit
  setUnitForProjectedTensor(quadraticError, elementNegativeMemberships, elementPositiveMemberships);
  // Construct TrieWithPrediction
  TubeWithPrediction::setDefaultMembership(unit * -shift);
  TrieWithPrediction tensor(cardinalities.begin(), cardinalities.end());
  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
    {
      tensor.setTuple(fuzzyTuple.getTuple().begin(), unit * fuzzyTuple.getMembership());
    }
  setNoSelection();
  return tensor;
}

double SparseRoughTensor::getAverageShift(const vector<vector<unsigned int>>& nSet) const
{
  return shift;
}
