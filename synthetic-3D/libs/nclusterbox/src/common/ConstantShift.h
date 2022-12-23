// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CONSTANT_SHIFT_H_
#define CONSTANT_SHIFT_H_

#include "AbstractShift.h"

class ConstantShift final : public AbstractShift
{
 public:
  ConstantShift(const double constant);

  double getShift(const vector<unsigned int>& tuple) const;
  double getAverageShift(const vector<vector<unsigned int>>& nSet) const;

  void setNewDimensionOrderAndNewIds(const vector<unsigned int>& old2NewDimensionOrder, const vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships);
  void setNewIds(const vector<vector<unsigned int>>& newIds2OldIds);

 private:
  const double constant;
};

#endif /*CONSTANT_SHIFT_H_*/
