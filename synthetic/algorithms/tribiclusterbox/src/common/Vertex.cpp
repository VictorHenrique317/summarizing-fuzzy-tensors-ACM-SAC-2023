// Copyright 2019-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "Vertex.h"

bool subPattern(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>& otherNSet)
{
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (vector<vector<unsigned int>>::const_iterator otherDimensionIt = otherNSet.begin(); dimensionIt != dimensionEnd && includes(otherDimensionIt->begin(), otherDimensionIt->end(), dimensionIt->begin(), dimensionIt->end()); ++otherDimensionIt)
    {
      ++dimensionIt;
    }
  return dimensionIt == dimensionEnd;
}

bool emptyIntersection(const vector<unsigned int>& dimension, vector<unsigned int>::const_iterator otherDimensionElementIt)
{
  const vector<unsigned int>::const_iterator elementEnd = dimension.end();
  vector<unsigned int>::const_iterator elementIt = dimension.begin();
  do
    {
      for (; *otherDimensionElementIt < *elementIt; ++otherDimensionElementIt)
	{
	}
      if (*otherDimensionElementIt == *elementIt)
	{
	  return false;
	}
    }
  while (++elementIt != elementEnd);
  return true;
}

vector<Vertex*> Vertex::model;

Vertex::Vertex(Vertex&& otherVertex): id(otherVertex.id), nSet(std::move(otherVertex.nSet)), density(otherVertex.density), quadraticErrorOfSubmodel(otherVertex.quadraticErrorOfSubmodel)
{
}

Vertex::Vertex(vector<vector<unsigned int>>& nSetParam, const int densityParam, const long long quadraticErrorOfSubmodelParam): id(model.size()), nSet(std::move(nSetParam)), density(densityParam), quadraticErrorOfSubmodel(quadraticErrorOfSubmodelParam)
{
}

Vertex::~Vertex()
{
}

bool Vertex::sink() const
{
  return true;
}

const vector<vector<unsigned int>>& Vertex::getNSet() const
{
  return nSet;
}

int Vertex::getDensity() const
{
  return density;
}

unsigned int Vertex::getArea() const
{
  unsigned int area = 1;
  for (const vector<unsigned int>& dimension : nSet)
    {
      area *= dimension.size();
    }
  return area;
}

long long Vertex::getG(const int backgroundDensity) const
{
  return static_cast<long long>(getArea()) * (density - backgroundDensity) * (density - backgroundDensity);
}

void Vertex::visit(dynamic_bitset<>& descendants) const
{
}

bool Vertex::overlapsWith(const vector<vector<unsigned int>>& otherNSet) const
{
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (const vector<unsigned int>& otherDimension : otherNSet)
    {
      if (*--dimensionIt->end() < *--otherDimension.end())
	{
	  if (emptyIntersection(*dimensionIt, otherDimension.begin()))
	    {
	      return false;
	    }
	}
      else
	{
	  if (emptyIntersection(otherDimension, dimensionIt->begin()))
	    {
	      return false;
	    }
	}
      ++dimensionIt;
    }
  return true;
}

bool Vertex::subPatternOf(const vector<vector<unsigned int>>& otherNSet) const
{
  return subPattern(nSet, otherNSet);
}

bool Vertex::superPatternOf(const vector<vector<unsigned int>>& otherNSet) const
{
  return subPattern(otherNSet, nSet);
}

InclusionResult Vertex::subOrSuperPatternOf(const vector<vector<unsigned int>>& otherNSet) const
{
  vector<vector<unsigned int>>::const_iterator otherDimensionIt = otherNSet.begin();
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (; dimensionIt != dimensionEnd && dimensionIt->size() == otherDimensionIt->size(); ++dimensionIt)
    {
      ++otherDimensionIt;
    }
  if (dimensionIt == dimensionEnd)
    {
      // Same sizes but nSet cannot be equal to otherNSet
      return incomparable;
    }
  if (dimensionIt->size() < otherDimensionIt->size())
    {
      if (subPatternOf(otherNSet))
	{
	  return sub;
	}
      return incomparable;
    }
  if (superPatternOf(otherNSet))
    {
      return super;
    }
  return incomparable;
}

long long Vertex::getQuadraticErrorDecreaseOfNullSubmodel(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity) const
{
  unsigned int candidateArea = 1;
  for (const vector<unsigned int>& candidateDimension : candidateNSet)
    {
      candidateArea *= candidateDimension.size();
    }
  return static_cast<long long>(candidateArea) * (candidateDensity - density) * (candidateDensity - density);
}

Confirmation Vertex::confirm(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity, dynamic_bitset<>& visitedVertices) const
{
  const unsigned int area = getArea();
  if (-getQuadraticErrorDecreaseOfNullSubmodel(candidateNSet, candidateDensity) < quadraticErrorOfSubmodel * (exp(2 * (1. - area) / (area - 2) / (area - 3)) - 1))
    {
      return Confirmation(id);
    }
  return Confirmation();
}
