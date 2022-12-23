// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "TrieWithPrediction.h"

TrieWithPrediction::TrieWithPrediction(): hyperplanes()
{
}

TrieWithPrediction::TrieWithPrediction(TrieWithPrediction&& otherTrieWithPrediction): hyperplanes(std::move(otherTrieWithPrediction.hyperplanes))
{
}

TrieWithPrediction::TrieWithPrediction(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd): hyperplanes()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  const vector<unsigned int>::const_iterator nextCardinalityIt = cardinalityIt + 1;
  if (nextCardinalityIt + 1 == cardinalityEnd)
    {
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TubeWithPrediction(*nextCardinalityIt));
	}
      return;
    }
  for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
    {
      hyperplanes.push_back(new TrieWithPrediction(nextCardinalityIt, cardinalityEnd));
    }
}

TrieWithPrediction::TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd): hyperplanes()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  const vector<unsigned int>::const_iterator nextCardinalityIt = cardinalityIt + 1;
  if (nextCardinalityIt + 1 == cardinalityEnd)
    {
      for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
	{
	  hyperplanes.push_back(new TubeWithPrediction(membershipIt, *nextCardinalityIt, unit));
	}
      return;
    }
  for (unsigned int hyperplaneId = 0; hyperplaneId != cardinality; ++hyperplaneId)
    {
      hyperplanes.push_back(new TrieWithPrediction(membershipIt, unit, nextCardinalityIt, cardinalityEnd));
    }
}

TrieWithPrediction::~TrieWithPrediction()
{
  for (AbstractDataWithPrediction* hyperplane : hyperplanes)
    {
      delete hyperplane;
    }
}

TrieWithPrediction& TrieWithPrediction::operator=(TrieWithPrediction&& otherTrieWithPrediction)
{
  hyperplanes = std::move(otherTrieWithPrediction.hyperplanes);
  return *this;
}

void TrieWithPrediction::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
  hyperplanes[*idIt]->setTuple(idIt + 1, membership);
}

void TrieWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->membershipSumOnSlice(nextDimensionIt, sum);
    }
}

int TrieWithPrediction::density(const vector<vector<unsigned int>>& nSet) const
{
  long long sum = 0;
  const vector<vector<unsigned int>>::const_iterator secondDimensionIt = ++nSet.begin();
  for (const unsigned int id : nSet.front())
    {
      int sumOnSlice = 0;
      hyperplanes[id]->membershipSumOnSlice(secondDimensionIt, sumOnSlice);
      sum += sumOnSlice;
    }
  for (const vector<unsigned int>& dimension : nSet)
    {
      sum /= dimension.size();
    }
  return sum;
}

long long TrieWithPrediction::sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>& tuples, const int density) const
{
  long long sum = 0;
  sumNegativeDensityMinus2Memberships(tuples.begin(), density, sum);
  return sum;
}

void TrieWithPrediction::sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->sumNegativeDensityMinus2Memberships(nextDimensionIt, density, sum);
    }
}

long long TrieWithPrediction::addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density)
{
  long long nullSubmodelQuadraticError = 0;
  addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(tuples.begin(), density, nullSubmodelQuadraticError);
  return nullSubmodelQuadraticError;
}

void TrieWithPrediction::addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(nextDimensionIt, density, nullSubmodelQuadraticError);
    }
}

long long TrieWithPrediction::addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density)
{
  long long nullSubmodelQuadraticError = 0;
  addPatternToModelAndGetQuadraticErrorOfNullSubmodel(tuples.begin(), density, nullSubmodelQuadraticError);
  return nullSubmodelQuadraticError;
}

void TrieWithPrediction::addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->addPatternToModelAndGetQuadraticErrorOfNullSubmodel(nextDimensionIt, density, nullSubmodelQuadraticError);
    }
}

void TrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>& tuples, const int density)
{
  addFirstPatternToModel(tuples.begin(), density);
}

void TrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->addFirstPatternToModel(nextDimensionIt, density);
    }
}

void TrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>& tuples, const int density)
{
  addPatternToModel(tuples.begin(), density);
}

void TrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->addPatternToModel(nextDimensionIt, density);
    }
}

long long TrieWithPrediction::addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>& tuples, const int density)
{
  long long quadraticErrorVariation = 0;
  addPatternToModelAndGetQuadraticErrorVariation(tuples.begin(), density, quadraticErrorVariation);
  return quadraticErrorVariation;
}

void TrieWithPrediction::addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& quadraticErrorVariation)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->addPatternToModelAndGetQuadraticErrorVariation(nextDimensionIt, density, quadraticErrorVariation);
    }
}

long long TrieWithPrediction::quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>& tuples, const int density) const
{
  long long nullSubmodelQuadraticError = 0;
  quadraticErrorOfNullSubmodel(tuples.begin(), density, nullSubmodelQuadraticError);
  return nullSubmodelQuadraticError;
}

void TrieWithPrediction::quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->quadraticErrorOfNullSubmodel(nextDimensionIt, density, nullSubmodelQuadraticError);
    }
}

pair<long long, long long> TrieWithPrediction::lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>& tuples, const int density) const
{
  pair<long long, long long> lowerBoundAndDelta(0, 0);
  lowerBoundAndQuadraticErrorVariation(tuples.begin(), density, lowerBoundAndDelta.first, lowerBoundAndDelta.second);
  return lowerBoundAndDelta;
}

void TrieWithPrediction::lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& lowerBound, long long& delta) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->lowerBoundAndQuadraticErrorVariation(nextDimensionIt, density, lowerBound, delta);
    }
}

TrieWithPrediction TrieWithPrediction::subtensor(const vector<vector<unsigned int>>& tuples, const int density) const
{
  TrieWithPrediction subtrie;
  const vector<vector<unsigned int>>::const_iterator dimensionIt = tuples.begin();
  subtrie.hyperplanes.reserve(dimensionIt->size());
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      subtrie.hyperplanes.push_back(hyperplanes[id]->subtensor(nextDimensionIt, density));
    }
  return subtrie;
}

TrieWithPrediction* TrieWithPrediction::subtensor(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) const
{
  TrieWithPrediction* subtrie = new TrieWithPrediction();
  subtrie->hyperplanes.reserve(dimensionIt->size());
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      subtrie->hyperplanes.push_back(hyperplanes[id]->subtensor(nextDimensionIt, density));
    }
  return subtrie;
}

long long TrieWithPrediction::quadraticErrorVariation(const vector<vector<unsigned int>> tuples, const int density) const
{
  long long delta = 0;
  quadraticErrorVariation(tuples.begin(), density, delta);
  return delta;
}

void TrieWithPrediction::quadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& delta) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  for (const unsigned int id : *dimensionIt)
    {
      hyperplanes[id]->quadraticErrorVariation(nextDimensionIt, density, delta);
    }
}
