// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "AbstractData.h"

AbstractData::~AbstractData()
{
}

bool AbstractData::isFullSparseTube() const
{
  return false;
}

void AbstractData::setTuple(const vector<unsigned int>::const_iterator idIt)
{
}

void AbstractData::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
}

void AbstractData::sortTubes()
{
}

void AbstractData::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const
{
  // *this is a crisp tube
  // Never called
}

void AbstractData::updateSumsOnExtensionsAfterExtension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt, const vector<vector<int>>::iterator sumsInExtendedDimensionIt) const
{
  // *this is a tube and sumsIt == sumsInExtendedDimensionIt necessarily: nothing to do
}

int AbstractData::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is a crisp tube
  // Never called
  return 0;
}

void AbstractData::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is a crisp tube
  // Never called
}

int AbstractData::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is a fuzzy tube and *dimensionIt is necessarily the increased dimension
  return increaseSumsOnPatternAndHyperplanes(dimensionIt, sumsIt);
}
