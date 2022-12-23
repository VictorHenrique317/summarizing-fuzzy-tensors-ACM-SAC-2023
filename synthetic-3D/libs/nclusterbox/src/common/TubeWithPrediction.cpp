// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "TubeWithPrediction.h"

int TubeWithPrediction::defaultMembership;

TubeWithPrediction::TubeWithPrediction(): tube()
{
}

TubeWithPrediction::TubeWithPrediction(const unsigned int size): tube(size, pair<int, int>(0, defaultMembership))
{
}

TubeWithPrediction::TubeWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int size, const unsigned int unit): tube()
{
  tube.reserve(size);
  for (const vector<double>::const_iterator tubeEnd = membershipIt + size; membershipIt != tubeEnd; ++membershipIt)
    {
      tube.emplace_back(0, unit * *membershipIt);
    }
}

void TubeWithPrediction::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
  tube[*idIt].second = membership;
}

void TubeWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const
{
  for (const unsigned int id : *dimensionIt)
    {
      sum += tube[id].second;
    }
}

void TubeWithPrediction::sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& sum) const
{
  for (const unsigned int id : *dimensionIt)
    {
      const int term = density - 2 * tube[id].second;
      if (term < 0)
	{
	  sum += term;
	}
    }
}

void TubeWithPrediction::addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError)
{
  for (const unsigned int id : *dimensionIt)
    {
      pair<int, int>& estimatedAndRealMembership = tube[id];
      estimatedAndRealMembership.first = density;
      nullSubmodelQuadraticError += static_cast<long long>(density - estimatedAndRealMembership.second) * (density - estimatedAndRealMembership.second);
    }
}

void TubeWithPrediction::addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError)
{
  for (const unsigned int id : *dimensionIt)
    {
      pair<int, int>& estimatedAndRealMembership = tube[id];
      if (estimatedAndRealMembership.first < density)
	{
	  estimatedAndRealMembership.first = density;
	}
      nullSubmodelQuadraticError += static_cast<long long>(density - estimatedAndRealMembership.second) * (density - estimatedAndRealMembership.second);
    }
}

void TubeWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density)
{
  for (const unsigned int id : *dimensionIt)
    {
      tube[id].first = density;
    }
}

void TubeWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density)
{
  for (const unsigned int id : *dimensionIt)
    {
      int& estimatedMembership = tube[id].first;
      if (estimatedMembership < density)
	{
	  estimatedMembership = density;
	}
    }
}

void TubeWithPrediction::addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& quadraticErrorVariation)
{
  for (const unsigned int id : *dimensionIt)
    {
      pair<int, int>& estimatedAndRealMembership = tube[id];
      if (estimatedAndRealMembership.first < density)
	{
	  quadraticErrorVariation += static_cast<long long>(density - estimatedAndRealMembership.second) * (density - estimatedAndRealMembership.second) - static_cast<long long>(estimatedAndRealMembership.first - estimatedAndRealMembership.second) * (estimatedAndRealMembership.first - estimatedAndRealMembership.second);
	  estimatedAndRealMembership.first = density;
	}
    }
}

void TubeWithPrediction::quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) const
{
  for (const unsigned int id : *dimensionIt)
    {
      const long long residual = density - tube[id].second;
      nullSubmodelQuadraticError += residual * residual;
    }
}

void TubeWithPrediction::lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& lowerBound, long long& delta) const
{
  for (const unsigned int id : *dimensionIt)
    {
      const pair<int, int>& estimatedAndRealMembership = tube[id];
      if (estimatedAndRealMembership.first < density)
	{
	  const long long term = static_cast<long long>(density - estimatedAndRealMembership.second) * (density - estimatedAndRealMembership.second) - static_cast<long long>(estimatedAndRealMembership.first - estimatedAndRealMembership.second) * (estimatedAndRealMembership.first - estimatedAndRealMembership.second);
	  if (term < 0)
	    {
	      lowerBound += term;
	    }
	  delta += term;
	}
    }
}

TubeWithPrediction* TubeWithPrediction::subtensor(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) const
{
  TubeWithPrediction* subtube = new TubeWithPrediction();
  subtube->tube.reserve(dimensionIt->size());
  for (const unsigned int id : *dimensionIt)
    {
      subtube->tube.emplace_back(density, tube[id].second);
    }
  return subtube;
}

void TubeWithPrediction::quadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& delta) const
{
  for (const unsigned int id : *dimensionIt)
    {
      const pair<int, int>& estimatedAndRealMembership = tube[id];
      if (estimatedAndRealMembership.first < density)
	{
	  delta += static_cast<long long>(density - estimatedAndRealMembership.second) * (density - estimatedAndRealMembership.second) - static_cast<long long>(estimatedAndRealMembership.first - estimatedAndRealMembership.second) * (estimatedAndRealMembership.first - estimatedAndRealMembership.second);
	}
    }
}

void TubeWithPrediction::setDefaultMembership(const int defaultMembershipParam)
{
  defaultMembership = defaultMembershipParam;
}
