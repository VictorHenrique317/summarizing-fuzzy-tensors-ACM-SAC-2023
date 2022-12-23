// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "ExpectationShift.h"

ExpectationShift::ExpectationShift(const vector<FuzzyTuple>& fuzzyTuples, const vector<vector<string>>& ids2Labels): elementAvgs()
{
  unsigned long long area = 1;
  elementAvgs.reserve(ids2Labels.size());
  for (const vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      elementAvgs.push_back(vector<double>(ids2LabelsInDimension.size()));
      area *= ids2LabelsInDimension.size();
    }
  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
    {
      vector<vector<double>>::iterator elementAvgsIt = elementAvgs.begin();
      for (const unsigned int element : fuzzyTuple.getTuple())
	{
	  (*elementAvgsIt)[element] += fuzzyTuple.getMembership();
	  ++elementAvgsIt;
	}
    }
  vector<vector<double>>::iterator elementAvgsInDimensionIt = elementAvgs.begin();
  for (const vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      const unsigned int areaOfElementInDimension = area / ids2LabelsInDimension.size();
      for (double& elementAvg : *elementAvgsInDimensionIt)
	{
	  elementAvg /= areaOfElementInDimension;
	}
      ++elementAvgsInDimensionIt;
    }
}

double ExpectationShift::getShift(const vector<unsigned int>& tuple) const
{
  vector<vector<double>>::const_iterator elementAvgsInDimensionIt = elementAvgs.begin();
  vector<unsigned int>::const_iterator elementIt = tuple.begin();
  double max = (*elementAvgsInDimensionIt)[*elementIt];
  const vector<unsigned int>::const_iterator end = tuple.end();
  while (++elementIt != end)
    {
      const double elementAvg = (*++elementAvgsInDimensionIt)[*elementIt];
      if (elementAvg > max)
	{
	  max = elementAvg;
	}
    }
  return max;
}

double ExpectationShift::getAverageShift(const vector<vector<unsigned int>>& nSet) const
{
  unsigned int area = 1;
  double sum = 0;
  vector<vector<double>>::const_iterator elementAvgsInDimensionIt = elementAvgs.begin();
  for (const vector<unsigned int>& nSetDimension : nSet)
    {
      for (const unsigned int id : nSetDimension)
	{
	  sum += (*elementAvgsInDimensionIt)[id];
	}
      ++elementAvgsInDimensionIt;
      area *= nSetDimension.size();
    }
  return sum / area;
}

void ExpectationShift::setNewDimensionOrderAndNewIds(const vector<unsigned int>& old2NewDimensionOrder, const vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships)
{
  vector<vector<double>> reorderedElementsAvg(elementAvgs.size());
  vector<vector<pair<double, unsigned int>>>::const_iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
  vector<vector<double>>::iterator elementAvgsInDimensionIt = elementAvgs.begin();
  for (const unsigned int newDimensionId : old2NewDimensionOrder)
    {
      vector<double> reorderedElementsAvgsInDimension;
      reorderedElementsAvgsInDimension.reserve(elementAvgsInDimensionIt->size());
      for (const pair<double, unsigned int>& elementMembershipAndOldId : *elementPositiveMembershipsInDimensionIt)
	{
	  reorderedElementsAvgsInDimension.push_back((*elementAvgsInDimensionIt)[elementMembershipAndOldId.second]);
	}
      reorderedElementsAvg[newDimensionId] = std::move(*elementAvgsInDimensionIt++);
      ++elementPositiveMembershipsInDimensionIt;
    }
  elementAvgs = std::move(reorderedElementsAvg);
}

void ExpectationShift::setNewIds(const vector<vector<unsigned int>>& newIds2OldIds)
{
  vector<vector<double>>::iterator elementAvgsInDimensionIt = elementAvgs.begin();
  for (const vector<unsigned int>& newIds2OldIdsInDimension : newIds2OldIds)
    {
      vector<double> reorderedElementsAvgsInDimension;
      reorderedElementsAvgsInDimension.reserve(newIds2OldIdsInDimension.size());
      for (const unsigned int oldId : newIds2OldIdsInDimension)
	{
	  reorderedElementsAvgsInDimension.push_back((*elementAvgsInDimensionIt)[oldId]);
	}
      *elementAvgsInDimensionIt++ = std::move(reorderedElementsAvgsInDimension);
    }
}
