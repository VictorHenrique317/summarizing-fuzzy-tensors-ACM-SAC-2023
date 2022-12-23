// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CANDIDATE_NODE_H_
#define CANDIDATE_NODE_H_

#include <vector>

using namespace std;

class CandidateNode
{
 public:
  CandidateNode(const unsigned int child1Id, const unsigned int child2Id, const long long quadraticErrorVariation);
  CandidateNode(const CandidateNode& otherCandidateNode);

  CandidateNode& operator=(const CandidateNode& otherCandidateNode);

  vector<unsigned int> getChildrenIds() const;
  long long getQuadraticErrorVariation() const;
  bool valid(const vector<unsigned int>& belowFrontierIds) const;

 private:
  unsigned int child1Id;
  unsigned int child2Id;
  long long quadraticErrorVariation;
};

#endif	/* CANDIDATE_NODE_H_ */
