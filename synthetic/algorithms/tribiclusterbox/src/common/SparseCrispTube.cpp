// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseCrispTube.h"

long long SparseCrispTube::defaultMembership;
unsigned int SparseCrispTube::sizeLimit;

SparseCrispTube::SparseCrispTube(): tube()
{
}

bool SparseCrispTube::isFullSparseTube() const
{
  return tube.size() == sizeLimit;
}

void SparseCrispTube::setTuple(const vector<unsigned int>::const_iterator idIt)
{
  tube.push_back(*idIt);
}

DenseCrispTube* SparseCrispTube::getDenseRepresentation() const
{
  return new DenseCrispTube(tube);
}

void SparseCrispTube::sortTubes()
{
  tube.shrink_to_fit();
  sort(tube.begin(), tube.end());
}

void SparseCrispTube::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& nbOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = dimensionIt->begin(); idIt != idEnd; ++idIt)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt);
      if (tubeBegin == tubeEnd)
	{
	  return;
	}
      if (*tubeBegin == *idIt)
	{
	  ++nbOfPresentTuples;
	}
    }
}

void SparseCrispTube::sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator numbersOfPresentTuplesIt) const
{
  for (const unsigned int elementId : tube)
    {
      int& currentNumberOfPresentTuples = (*numbersOfPresentTuplesIt)[elementId];
      if (currentNumberOfPresentTuples != numeric_limits<int>::min())
	{
	  ++currentNumberOfPresentTuples;
	}
    }
}

int SparseCrispTube::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator numbersOfPresentTuplesIt) const
{
  if (tube.empty())
    {
      return 0;
    }
  int sumOnPattern = 0;
  if (dimensionIt->back() < tube.back())
    {
      vector<unsigned int>::const_iterator idIt = tube.begin();
      for (const unsigned int presentElementId : *dimensionIt)
	{
	  for (; *idIt < presentElementId; ++idIt)
	    {
	      ++(*numbersOfPresentTuplesIt)[*idIt];
	    }
	  if (*idIt == presentElementId)
	    {
	      ++idIt;
	      ++(*numbersOfPresentTuplesIt)[presentElementId];
	      ++sumOnPattern;
	    }
	}
      for (const vector<unsigned int>::const_iterator idEnd = tube.end(); idIt != idEnd; ++idIt)
	{
	  ++(*numbersOfPresentTuplesIt)[*idIt];
	}
      return sumOnPattern;
    }
  const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  for (const unsigned int id : tube)
    {
      ++(*numbersOfPresentTuplesIt)[id];
      presentElementIdIt = lower_bound(presentElementIdIt, presentElementIdEnd, id);
      if (*presentElementIdIt == id)
	{
	  ++sumOnPattern;
	  ++presentElementIdIt;
	}
    }
  return sumOnPattern;
}

void SparseCrispTube::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // n == 2 and first dimension increased
  for (const unsigned int elementId : tube)
    {
      ++(*sumsIt)[elementId];
    }
}

int SparseCrispTube::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // necessarily the increased dimension
  if (binary_search(tube.begin(), tube.end(), dimensionIt->front()))
    {
      return 1;
    }
  return 0;
}

long long SparseCrispTube::getDefaultMembership()
{
  return defaultMembership;
}

void SparseCrispTube::setDefaultMembership(const int defaultMembershipParam)
{
  defaultMembership = defaultMembershipParam;
}

void SparseCrispTube::setDefaultMembershipAndSizeLimit(const int defaultMembershipParam, const unsigned int sizeLimitParam)
{
  defaultMembership = defaultMembershipParam;
  sizeLimit = sizeLimitParam;
}
