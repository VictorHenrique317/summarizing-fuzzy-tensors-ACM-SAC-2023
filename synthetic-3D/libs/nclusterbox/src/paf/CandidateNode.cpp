// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "CandidateNode.h"

CandidateNode::CandidateNode(const unsigned int child1IdParam, const unsigned int child2IdParam, const long long quadraticErrorVariationParam): child1Id(child1IdParam), child2Id(child2IdParam), quadraticErrorVariation(quadraticErrorVariationParam)
{
}

CandidateNode::CandidateNode(const CandidateNode& otherCandidateNode): child1Id(otherCandidateNode.child1Id), child2Id(otherCandidateNode.child2Id), quadraticErrorVariation(otherCandidateNode.quadraticErrorVariation)
{
}

CandidateNode& CandidateNode::operator=(const CandidateNode& otherCandidateNode)
{
  child1Id = otherCandidateNode.child1Id;
  child2Id = otherCandidateNode.child2Id;
  quadraticErrorVariation = otherCandidateNode.quadraticErrorVariation;
  return *this;
}

vector<unsigned int> CandidateNode::getChildrenIds() const
{
  return {child1Id, child2Id};
}

long long CandidateNode::getQuadraticErrorVariation() const
{
  return quadraticErrorVariation;
}

bool CandidateNode::valid(const vector<unsigned int>& belowFrontierIds) const
{
  for (const unsigned int id : belowFrontierIds)
    {
      if (id == child1Id || id == child2Id)
	{
	  return false;
	}
    }
  return true;
}
