// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseFuzzyTube.h"

int SparseFuzzyTube::defaultMembership;
unsigned int SparseFuzzyTube::sizeLimit;

SparseFuzzyTube::SparseFuzzyTube(): tube()
{
}

bool SparseFuzzyTube::isFullSparseTube() const
{
  return tube.size() == sizeLimit;
}

void SparseFuzzyTube::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
  tube.emplace_back(*idIt, membership);
}

DenseFuzzyTube* SparseFuzzyTube::getDenseRepresentation() const
{
  return new DenseFuzzyTube(tube, defaultMembership);
}

void SparseFuzzyTube::sortTubes()
{
  tube.shrink_to_fit();
  sort(tube.begin(), tube.end(), [](const pair<unsigned int, int>& entry1, const pair<unsigned int, int>& entry2) {return entry1.first < entry2.first;});
}

void SparseFuzzyTube::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const
{
  unsigned int nbOfDefaultMemberships = 0;
  const vector<pair<unsigned int, int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator tubeBegin = tube.begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = dimensionIt->begin(); idIt != idEnd; ++idIt)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt, [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
      if (tubeBegin == tubeEnd)
	{
	  sum += static_cast<int>(idEnd - idIt + nbOfDefaultMemberships) * defaultMembership;
	  return;
	}
      if (tubeBegin->first == *idIt)
	{
	  sum += tubeBegin->second;
	}
      else
	{
	  ++nbOfDefaultMemberships;
	}
    }
  sum += static_cast<int>(nbOfDefaultMemberships) * defaultMembership;
}

void SparseFuzzyTube::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const
{
  unsigned int nbOfDefaultMemberships = 0;
  const vector<pair<unsigned int, int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator tubeBegin = tube.begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = dimensionIt->begin(); idIt != idEnd; ++idIt)
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt, [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
      if (tubeBegin == tubeEnd)
	{
	  sum -= static_cast<int>(idEnd - idIt + nbOfDefaultMemberships) * defaultMembership;
	  return;
	}
      if (tubeBegin->first == *idIt)
	{
	  sum -= tubeBegin->second;
	}
      else
	{
	  ++nbOfDefaultMemberships;
	}
    }
  sum -= static_cast<int>(nbOfDefaultMemberships) * defaultMembership;
}

void SparseFuzzyTube::sumsOnExtensions(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  unsigned int elementId = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  for (const pair<unsigned int, int>& entry : tube)
    {
      while (entry.first != elementId++)
	{
	  if (*sumIt != numeric_limits<int>::min())
	    {
	      *sumIt += defaultMembership;
	    }
	  ++sumIt;
	}
      if (*sumIt != numeric_limits<int>::min())
	{
	  *sumIt += entry.second;
	}
      ++sumIt;
    }
  for (const vector<int>::iterator sumEnd = sumsIt->end(); sumIt != sumEnd; ++sumIt)
    {
      if (*sumIt != numeric_limits<int>::min())
	{
	  *sumIt += defaultMembership;
	}
    }
}

int SparseFuzzyTube::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  if (tube.empty())
    {
      addDefaultMembership(sumsIt->begin(), sumsIt->end());
      return static_cast<int>(dimensionIt->size()) * defaultMembership;
    }
  int sumOnPattern = 0;
  const vector<int>::iterator sumBegin = sumsIt->begin();
  vector<int>::iterator sumIt = sumBegin;
  if (tube.back().first < dimensionIt->back())
    {
      vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
      for (const pair<unsigned int, int>& entry : tube)
	{
	  const unsigned int id = entry.first;
	  for (; *presentElementIdIt < id; ++presentElementIdIt)
	    {
	      addDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	      *sumIt++ += defaultMembership;
	      sumOnPattern += defaultMembership;
	    }
	  addDefaultMembership(sumIt, sumBegin + id);
	  *sumIt++ += entry.second;
	  if (*presentElementIdIt == id)
	    {
	      sumOnPattern += entry.second;
	      ++presentElementIdIt;
	    }
	}
      sumOnPattern += static_cast<int>(dimensionIt->end() - presentElementIdIt) * defaultMembership;
    }
  else
    {
      vector<pair<unsigned int, int>>::const_iterator entryIt = tube.begin();
      for (const unsigned int presentElementId : *dimensionIt)
	{
	  for (; entryIt->first < presentElementId; ++entryIt)
	    {
	      addDefaultMembership(sumIt, sumBegin + entryIt->first);
	      *sumIt++ += entryIt->second;
	    }
	  addDefaultMembership(sumIt, sumBegin + presentElementId);
	  if (entryIt->first == presentElementId)
	    {
	      *sumIt++ += entryIt->second;
	      sumOnPattern += entryIt->second;
	      ++entryIt;
	    }
	  else
	    {
	      *sumIt++ += defaultMembership;
	      sumOnPattern += defaultMembership;
	    }
	}
      for (const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end(); entryIt != entryEnd; ++entryIt)
	{
	  addDefaultMembership(sumIt, sumBegin + entryIt->first);
	  *sumIt++ += entryIt->second;
	}
    }
  addDefaultMembership(sumIt, sumsIt->end());
  return sumOnPattern;
}

int SparseFuzzyTube::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  if (tube.empty())
    {
      subtractDefaultMembership(sumsIt->begin(), sumsIt->end());
      return static_cast<int>(dimensionIt->size()) * defaultMembership;
    }
  int sumOnPattern = 0;
  const vector<int>::iterator sumBegin = sumsIt->begin();
  vector<int>::iterator sumIt = sumBegin;
  if (tube.back().first < dimensionIt->back())
    {
      vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
      for (const pair<unsigned int, int>& entry : tube)
	{
	  const unsigned int id = entry.first;
	  for (; *presentElementIdIt < id; ++presentElementIdIt)
	    {
	      subtractDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	      *sumIt++ -= defaultMembership;
	      sumOnPattern += defaultMembership;
	    }
	  subtractDefaultMembership(sumIt, sumBegin + id);
	  *sumIt++ -= entry.second;
	  if (*presentElementIdIt == id)
	    {
	      sumOnPattern += entry.second;
	      ++presentElementIdIt;
	    }
	}
      sumOnPattern += static_cast<int>(dimensionIt->end() - presentElementIdIt) * defaultMembership;
    }
  else
    {
      vector<pair<unsigned int, int>>::const_iterator entryIt = tube.begin();
      for (const unsigned int presentElementId : *dimensionIt)
	{
	  for (; entryIt->first < presentElementId; ++entryIt)
	    {
	      subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
	      *sumIt++ -= entryIt->second;
	    }
	  subtractDefaultMembership(sumIt, sumBegin + presentElementId);
	  if (entryIt->first == presentElementId)
	    {
	      *sumIt++ -= entryIt->second;
	      sumOnPattern += entryIt->second;
	      ++entryIt;
	    }
	  else
	    {
	      *sumIt++ -= defaultMembership;
	      sumOnPattern += defaultMembership;
	    }
	}
      for (const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end(); entryIt != entryEnd; ++entryIt)
	{
	  subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
	  *sumIt++ -= entryIt->second;
	}
    }
  subtractDefaultMembership(sumIt, sumsIt->end());
  return sumOnPattern;
}

void SparseFuzzyTube::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // n == 2 and first dimension increased
  const vector<int>::iterator sumBegin = sumsIt->begin();
  vector<int>::iterator sumIt = sumBegin;
  for (const pair<unsigned int, int>& entry : tube)
    {
      addDefaultMembership(sumIt, sumBegin + entry.first);
      *sumIt++ += entry.second;
    }
  addDefaultMembership(sumIt, sumsIt->end());
}

void SparseFuzzyTube::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // n == 2 and first dimension decreased
  const vector<int>::iterator sumBegin = sumsIt->begin();
  vector<int>::iterator sumIt = sumBegin;
  for (const pair<unsigned int, int>& entry : tube)
    {
      subtractDefaultMembership(sumIt, sumBegin + entry.first);
      *sumIt++ -= entry.second;
    }
  subtractDefaultMembership(sumIt, sumsIt->end());
}

int SparseFuzzyTube::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // necessarily the increased dimension
  const vector<pair<unsigned int, int>>::const_iterator it = lower_bound(tube.begin(), tube.end(), dimensionIt->front(), [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
  if (it == tube.end() || it->first != dimensionIt->front())
    {
      return defaultMembership;
    }
  return it->second;
}

void SparseFuzzyTube::setDefaultMembershipAndSizeLimit(const int defaultMembershipParam, const unsigned int sizeLimitParam)
{
  defaultMembership = defaultMembershipParam;
  sizeLimit = sizeLimitParam;
}

void SparseFuzzyTube::addDefaultMembership(vector<int>::iterator it, const vector<int>::iterator end)
{
  for (; it != end; ++it)
    {
      *it += defaultMembership;
    }
}

void SparseFuzzyTube::subtractDefaultMembership(vector<int>::iterator it, const vector<int>::iterator end)
{
  for (; it != end; ++it)
    {
      *it -= defaultMembership;
    }
}
