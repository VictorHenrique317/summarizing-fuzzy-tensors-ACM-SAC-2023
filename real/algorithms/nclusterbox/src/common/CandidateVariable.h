// Copyright 2018,2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CANDIDATE_VARIABLE_H_
#define CANDIDATE_VARIABLE_H_

#include <vector>

using namespace std;

class CandidateVariable
{
 public:
  vector<vector<unsigned int>> nSet;
  const int density;

  CandidateVariable(const CandidateVariable& otherCandidateVariable);
  CandidateVariable(CandidateVariable&& otherCandidateVariable);
  CandidateVariable(vector<vector<unsigned int>>& nSet, const int density);

  long long getG() const;
};

#endif	/* CANDIDATE_VARIABLE_H_ */
