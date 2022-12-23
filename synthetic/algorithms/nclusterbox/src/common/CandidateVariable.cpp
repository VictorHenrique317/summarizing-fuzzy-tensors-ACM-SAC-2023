// Copyright 2018,2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "CandidateVariable.h"

CandidateVariable::CandidateVariable(const CandidateVariable& otherCandidateVariable): nSet(otherCandidateVariable.nSet), density(otherCandidateVariable.density)
{
}

CandidateVariable::CandidateVariable(CandidateVariable&& otherCandidateVariable): nSet(std::move(otherCandidateVariable.nSet)), density(otherCandidateVariable.density)
{
}

CandidateVariable::CandidateVariable(vector<vector<unsigned int>>& nSetParam, const int densityParam): nSet(std::move(nSetParam)), density(densityParam)
{
}

long long CandidateVariable::getG() const
{
  unsigned int area = 1;
  for (const vector<unsigned int>& dimension : nSet)
    {
      area *= dimension.size();
    }
  return static_cast<long long>(area) * density * density;
}
