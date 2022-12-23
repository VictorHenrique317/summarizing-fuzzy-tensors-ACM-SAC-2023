// Copyright 2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ORDER_PATTERNS_H_
#define ORDER_PATTERNS_H_

#include <map>
#include <iomanip>

#include "AbstractRoughTensor.h"
#include "CandidateVariable.h"

class OrderPatterns
{
 public:
  static void order(const unsigned int nbOfCandidateVariablesHavingAllElements, AbstractRoughTensor* roughTensor, const bool isVerbose, const bool isRSSPrinted);

 private:
  static bool emptyIntersection(const vector<unsigned int>& dimension, vector<unsigned int>::const_iterator otherDimensionElementIt);
  static bool overlapsWith(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>& otherNSet);
};

#endif	/* ORDER_PATTERNS_H_ */
