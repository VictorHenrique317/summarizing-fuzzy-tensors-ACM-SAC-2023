// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseRoughTensor.h"

DenseRoughTensor::DenseRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isVerbose): shift(), memberships()
{
  vector<FuzzyTuple> fuzzyTuples = getFuzzyTuples(tensorFileName, inputDimensionSeparator, inputElementSeparator, isVerbose);
  Trie::is01 = false;
  shift = new ExpectationShift(fuzzyTuples, ids2Labels);
  init(fuzzyTuples);
}

DenseRoughTensor::DenseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double constantShift): shift(new ConstantShift(constantShift)), memberships()
{
  init(fuzzyTuples);
  if (Trie::is01)
    {
      SparseCrispTube::setDefaultMembership(unit * -constantShift);
    }
}

DenseRoughTensor::~DenseRoughTensor()
{
  delete shift;
}

void DenseRoughTensor::init(vector<FuzzyTuple>& fuzzyTuples)
{
  nullModelQuadraticError = 0;
  // Initialize tuple and positive/negative memberships of the elements
  vector<double> shiftedMemberships;
  unsigned long long nbOfTuples = 1;
  vector<unsigned int> tuple;
  tuple.reserve(ids2Labels.size());
  vector<vector<pair<double, unsigned int>>> elementPositiveMemberships;
  elementPositiveMemberships.reserve(ids2Labels.size());
  double minElementNegativeMembership;
  if (Trie::is01)
    {
      // Consider a slice with every membership null to define minElementNegativeMembership and, consequently, unit, so that SparseCrispTube::getDefaultMembership() * area (with area the area of a slice) fits in an int (e.g., in Trie::increaseSumsOnHyperplanes)
      unsigned int minCardinality = numeric_limits<unsigned int>::max();
      for (const vector<string>& labelsInDimension : ids2Labels)
	{
	  const unsigned int nbOfElements = labelsInDimension.size();
	  if (nbOfElements < minCardinality)
	    {
	      minCardinality = nbOfElements;
	    }
	  nbOfTuples *= nbOfElements;
	  tuple.push_back(nbOfElements - 1);
	  vector<pair<double, unsigned int>> elementPositiveMembershipsInDimension;
	  elementPositiveMembershipsInDimension.reserve(nbOfElements);
	  for (unsigned int id = 0; id != nbOfElements; ++id)
	    {
	      elementPositiveMembershipsInDimension.emplace_back(0, id);
	    }
	  elementPositiveMemberships.emplace_back(std::move(elementPositiveMembershipsInDimension));
	}
      shiftedMemberships.resize(nbOfTuples);
      vector<double>::reverse_iterator shiftedMembershipIt = shiftedMemberships.rbegin(); // filled backwards, so that in lexicographic order of the tuples
      const double constantShift = -shift->getShift(tuple); // necessary constant (otherwise !is01)
      minElementNegativeMembership = constantShift * (nbOfTuples / minCardinality);
      const vector<FuzzyTuple>::const_iterator end = --fuzzyTuples.end();
      vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
      for (; fuzzyTupleIt != end; ++fuzzyTupleIt)
	{
	  // Comparing in the reverse order because the last ids are more likely to be different
	  while (!equal(tuple.rbegin(), tuple.rend(), fuzzyTupleIt->getTuple().rbegin()))
	    {
	      *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, constantShift, elementPositiveMemberships);
	    }
	  *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, fuzzyTupleIt->getMembership() + constantShift, elementPositiveMemberships);
	}
      // fuzzyTupleIt->getTuple() is necessarily a vector of zero (first fuzzy tuple that was read): no more tuple
      // Comparing in the reverse order because the last ids are more likely to be different
      while (!equal(tuple.rbegin(), tuple.rend(), fuzzyTupleIt->getTuple().rbegin()))
	{
	  *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, constantShift, elementPositiveMemberships);
	}
      *shiftedMembershipIt = fuzzyTupleIt->getMembership() + constantShift;
      updateNullModelQuadraticErrorAndElementMemberships(tuple, *shiftedMembershipIt, elementPositiveMemberships);
    }
  else
    {
      vector<vector<double>> elementNegativeMemberships;
      elementNegativeMemberships.reserve(ids2Labels.size());
      for (const vector<string>& labelsInDimension : ids2Labels)
	{
	  const unsigned int nbOfElements = labelsInDimension.size();
	  nbOfTuples *= nbOfElements;
	  tuple.push_back(nbOfElements - 1);
	  vector<pair<double, unsigned int>> elementPositiveMembershipsInDimension;
	  elementPositiveMembershipsInDimension.reserve(nbOfElements);
	  for (unsigned int id = 0; id != nbOfElements; ++id)
	    {
	      elementPositiveMembershipsInDimension.emplace_back(0, id);
	    }
	  elementPositiveMemberships.emplace_back(std::move(elementPositiveMembershipsInDimension));
	  elementNegativeMemberships.emplace_back(nbOfElements);
	}
      shiftedMemberships.resize(nbOfTuples);
      vector<double>::reverse_iterator shiftedMembershipIt = shiftedMemberships.rbegin(); // filled backwards, so that in lexicographic order of the tuples
      const vector<FuzzyTuple>::const_iterator end = --fuzzyTuples.end();
      vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
      for (; fuzzyTupleIt != end; ++fuzzyTupleIt)
	{
	  // Comparing in the reverse order because the last ids are more likely to be different
	  while (!equal(tuple.rbegin(), tuple.rend(), fuzzyTupleIt->getTuple().rbegin()))
	    {
	      *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, -shift->getShift(tuple), elementPositiveMemberships, elementNegativeMemberships);
	    }
	  *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, fuzzyTupleIt->getMembership() - shift->getShift(tuple), elementPositiveMemberships, elementNegativeMemberships);
	}
      // fuzzyTupleIt->getTuple() is necessarily a vector of zero (first fuzzy tuple that was read): no more tuple
      // Comparing in the reverse order because the last ids are more likely to be different
      while (!equal(tuple.rbegin(), tuple.rend(), fuzzyTupleIt->getTuple().rbegin()))
	{
	  *shiftedMembershipIt++ = updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(tuple, -shift->getShift(tuple), elementPositiveMemberships, elementNegativeMemberships);
	}
      *shiftedMembershipIt = fuzzyTupleIt->getMembership() - shift->getShift(tuple);
      updateNullModelQuadraticErrorAndElementMemberships(tuple, *shiftedMembershipIt, elementPositiveMemberships, elementNegativeMemberships);
      vector<vector<double>>::const_iterator elementNegativeMembershipsIt = elementNegativeMemberships.begin();
      minElementNegativeMembership = *min_element(elementNegativeMembershipsIt->begin(), elementNegativeMembershipsIt->end());
      for (const vector<vector<double>>::const_iterator elementNegativeMembershipsEnd = elementNegativeMemberships.end(); elementNegativeMembershipsIt != elementNegativeMembershipsEnd; ++elementNegativeMembershipsIt)
	{
	  const double minElementNegativeMembershipInDimension = *min_element(elementNegativeMembershipsIt->begin(), elementNegativeMembershipsIt->end());
	  if (minElementNegativeMembershipInDimension < minElementNegativeMembership)
	    {
	      minElementNegativeMembership = minElementNegativeMembershipInDimension;
	    }
	}
    }
  fuzzyTuples.clear();
  fuzzyTuples.shrink_to_fit();
  // Compute new ids, in increasing order of element membership, set unit, cardinalities and external2InternalDimensionOrder
  setMetadata(elementPositiveMemberships, -minElementNegativeMembership);
  // Inform the shift of the new dimension order
  shift->setNewDimensionOrderAndNewIds(external2InternalDimensionOrder, elementPositiveMemberships);
  // Compute the offsets to access shiftedMemberships and init tuple to the last one
  vector<unsigned int>::iterator tupleIt = tuple.begin();
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  vector<vector<string>>::const_reverse_iterator labelsInDimensionIt = ids2Labels.rbegin();
  unsigned int offset = 1;
  vector<unsigned int> offsets(ids2Labels.size());
  const vector<unsigned int>::reverse_iterator rend = offsets.rend();
  for (vector<unsigned int>::reverse_iterator offsetIt = offsets.rbegin(); offsetIt != rend; ++offsetIt)
    {
      *offsetIt = offset;
      offset *= labelsInDimensionIt->size();
      ++labelsInDimensionIt;
      *tupleIt++ = *cardinalityIt++ - 1;
    }
  // Compute memberships, reodering of shiftedMemberships according to external2InternalDimensionOrder
  memberships.resize(nbOfTuples);
  for (vector<double>::reverse_iterator membershipIt = memberships.rbegin(); ; ++membershipIt) // filled backwards, so that in lexicographic order of the tuples
    {
      unsigned long long membershipIndex = 0;
      vector<unsigned int>::const_iterator offsetIt = offsets.begin();
      vector<vector<pair<double, unsigned int>>>::const_iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
      for (const unsigned int internalDimensionId : external2InternalDimensionOrder)
	{
	  membershipIndex += (*elementPositiveMembershipsInDimensionIt)[tuple[internalDimensionId]].second * *offsetIt;
	  ++offsetIt;
	  ++elementPositiveMembershipsInDimensionIt;
	}
      *membershipIt = shiftedMemberships[membershipIndex];
      if (--nbOfTuples == 0)
	{
	  break;
	}
      vector<unsigned int>::reverse_iterator elementIt = tuple.rbegin();
      for (vector<unsigned int>::const_reverse_iterator cardinalityIt = cardinalities.rbegin(); *elementIt == 0; ++cardinalityIt)
	{
	  *elementIt++ = *cardinalityIt - 1;
	}
      --*elementIt;
    }
}

double DenseRoughTensor::updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships)
{
  updateNullModelQuadraticErrorAndElementMemberships(tuple, shiftedMembership, elementPositiveMemberships);
  // Advance tuple, big-endian-like
  vector<unsigned int>::reverse_iterator elementIt = tuple.rbegin();
  for (vector<vector<string>>::const_reverse_iterator labelsInDimensionReverseIt = ids2Labels.rbegin(); *elementIt == 0; ++labelsInDimensionReverseIt)
    {
      *elementIt++ = labelsInDimensionReverseIt->size() - 1;
    }
  --*elementIt;
  return shiftedMembership;
}

double DenseRoughTensor::updateNullModelQuadraticErrorAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships)
{
  updateNullModelQuadraticErrorAndElementMemberships(tuple, shiftedMembership, elementPositiveMemberships, elementNegativeMemberships);
  // Advance tuple, big-endian-like
  vector<unsigned int>::reverse_iterator elementIt = tuple.rbegin();
  for (vector<vector<string>>::const_reverse_iterator labelsInDimensionReverseIt = ids2Labels.rbegin(); *elementIt == 0; ++labelsInDimensionReverseIt)
    {
      *elementIt++ = labelsInDimensionReverseIt->size() - 1;
    }
  --*elementIt;
  return shiftedMembership;
}

void DenseRoughTensor::updateNullModelQuadraticErrorAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships)
{
  nullModelQuadraticError += shiftedMembership * shiftedMembership; // the quadratic error of the null model on the tuples read so far
  if (shiftedMembership > 0)
    {
      vector<vector<pair<double, unsigned int>>>::iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
      for (const unsigned int id : tuple)
	{
	  (*elementPositiveMembershipsInDimensionIt)[id].first += shiftedMembership;
	  ++elementPositiveMembershipsInDimensionIt;
	}
    }
}

void DenseRoughTensor::updateNullModelQuadraticErrorAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships)
{
  nullModelQuadraticError += shiftedMembership * shiftedMembership; // the quadratic error of the null model on the tuples read so far
  if (shiftedMembership < 0)
    {
      vector<vector<double>>::iterator elementNegativeMembershipsInDimensionIt = elementNegativeMemberships.begin();
      for (const unsigned int id : tuple)
	{
	  (*elementNegativeMembershipsInDimensionIt)[id] += shiftedMembership;
	  ++elementNegativeMembershipsInDimensionIt;
	}
      return;
    }
  vector<vector<pair<double, unsigned int>>>::iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
  for (const unsigned int id : tuple)
    {
      (*elementPositiveMembershipsInDimensionIt)[id].first += shiftedMembership;
      ++elementPositiveMembershipsInDimensionIt;
    }
}

Trie DenseRoughTensor::getTensor() const
{
  vector<double>::const_iterator membershipIt = memberships.begin();
  if (Trie::is01)
    {
      DenseCrispTube::setSize(cardinalities.back());
      return Trie(membershipIt, cardinalities.begin(), cardinalities.end());
    }
  DenseFuzzyTube::setSize(cardinalities.back());
  return Trie(membershipIt, unit, cardinalities.begin(), cardinalities.end());
}

void DenseRoughTensor::setNoSelection()
{
  memberships.clear();
  memberships.shrink_to_fit();
}

TrieWithPrediction DenseRoughTensor::projectTensor(const unsigned int nbOfPatternsHavingAllElements)
{
  // Compute the offsets to access memberships from the non-updated cardinalities
  unsigned int offset = 1;
  vector<unsigned int>::const_reverse_iterator cardinalityIt = cardinalities.rbegin();
  vector<unsigned int> offsets(cardinalities.size() - 1);
  const vector<unsigned int>::reverse_iterator rend = offsets.rend();
  for (vector<unsigned int>::reverse_iterator offsetIt = offsets.rbegin(); offsetIt != rend; ++offsetIt)
    {
      offset *= *cardinalityIt++;
      *offsetIt = offset;
    }
  // Update cardinalities, ids2Labels and candidateVariables
  const vector<vector<unsigned int>> newIds2OldIds = projectMetadata(nbOfPatternsHavingAllElements, false);
  // Inform the shift of the new ids
  shift->setNewIds(newIds2OldIds);
  // Compute last tuple and nb of tuples, according to new cardinalities
  vector<unsigned int> tuple;
  tuple.reserve(cardinalities.size());
  unsigned long long nbOfSelectedTuples = 1;
  for (const unsigned int cardinality : cardinalities)
    {
      nbOfSelectedTuples *= cardinality;
      tuple.push_back(cardinality - 1);
    }
  // Select memberships in reduced tensor, compute negative/positive memberships of elements in first dimension and the quadratic error of the null model
  double quadraticError = 0;
  vector<double> elementNegativeMemberships(cardinalities.front());
  vector<double> elementPositiveMemberships(cardinalities.front());
  vector<double> selectedMemberships(nbOfSelectedTuples);
  for (vector<double>::reverse_iterator selectedMembershipIt = selectedMemberships.rbegin(); ; ++selectedMembershipIt) // filled backwards, so that in lexicographic order of the tuples
    {
      unsigned long long membershipIndex = 0;
      vector<vector<unsigned int>>::const_iterator newIds2OldIdsIt = newIds2OldIds.begin();
      vector<unsigned int>::const_iterator idIt = tuple.begin();
      for (const unsigned int offset : offsets)
	{
	  membershipIndex += offset * (*newIds2OldIdsIt)[*idIt];
	  ++newIds2OldIdsIt;
	  ++idIt;
	}
      const double membership = memberships[membershipIndex + (*newIds2OldIdsIt)[*idIt]];
      *selectedMembershipIt = membership;
      if (membership < 0)
	{
	  elementNegativeMemberships[tuple.front()] -= membership;
	}
      else
	{
	  elementPositiveMemberships[tuple.front()] += membership;
	}
      quadraticError += membership * membership;
      if (--nbOfSelectedTuples == 0)
	{
	  break;
	}
      vector<unsigned int>::reverse_iterator elementIt = tuple.rbegin();
      for (vector<unsigned int>::const_reverse_iterator cardinalityIt = cardinalities.rbegin(); *elementIt == 0; ++cardinalityIt)
	{
	  *elementIt++ = *cardinalityIt - 1;
	}
      --*elementIt;
    }
  setNoSelection();
  // Compute unit
  setUnitForProjectedTensor(quadraticError, elementNegativeMemberships, elementPositiveMemberships);
  // Construct TrieWithPrediction
  vector<double>::const_iterator selectedMembershipIt = selectedMemberships.begin();
  return TrieWithPrediction(selectedMembershipIt, unit, cardinalities.begin(), cardinalities.end());
}

double DenseRoughTensor::getAverageShift(const vector<vector<unsigned int>>& nSet) const
{
  return shift->getAverageShift(nSet);
}
