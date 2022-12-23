// Copyright 2018-2020 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_DATA_WITH_PREDICTION_H_
#define ABSTRACT_DATA_WITH_PREDICTION_H_

#include <vector>

using namespace std;

class AbstractDataWithPrediction
{
 public:
  virtual ~AbstractDataWithPrediction();

  virtual void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership) = 0;

  virtual void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const = 0;
  virtual void sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& sum) const = 0;
  virtual void addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) = 0;
  virtual void addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) = 0;
  virtual void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) = 0;
  virtual void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) = 0;
  virtual void addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& quadraticErrorVariation) = 0;
  virtual void quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) const = 0;
  virtual void lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& lowerBound, long long& delta) const = 0;
  virtual AbstractDataWithPrediction* subtensor(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) const = 0;
  virtual void quadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& delta) const = 0;
};

#endif /*ABSTRACT_DATA_WITH_PREDICTION_H_*/
