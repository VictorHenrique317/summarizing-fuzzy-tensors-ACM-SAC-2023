// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TUBE_WITH_PREDICTION_H_
#define TUBE_WITH_PREDICTION_H_

#include "AbstractDataWithPrediction.h"

class TubeWithPrediction final : public AbstractDataWithPrediction
{
 public:
  TubeWithPrediction();
  TubeWithPrediction(const unsigned int size);
  TubeWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int size, const unsigned int unit);

  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);

  void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, int& sum) const;
  void sumNegativeDensityMinus2Memberships(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& sum) const;
  void addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError);
  void addPatternToModelAndGetQuadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError);
  void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density);
  void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density);
  void addPatternToModelAndGetQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& quadraticErrorVariation);
  void quadraticErrorOfNullSubmodel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& nullSubmodelQuadraticError) const;
  void lowerBoundAndQuadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& lowerBound, long long& delta) const;
  TubeWithPrediction* subtensor(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density) const;
  void quadraticErrorVariation(const vector<vector<unsigned int>>::const_iterator dimensionIt, const int density, long long& delta) const;

  static void setDefaultMembership(const int defaultMembership);

 private:
  vector<pair<int, int>> tube;	/* estimated and real membership; "estimated = -1" means no candidate pattern covers the tuple; otherwise, the greatest density of a selected pattern covering the tuple */

  static int defaultMembership;
};

#endif /*TUBE_WITH_PREDICTION_H_*/
