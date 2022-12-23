// Copyright 2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "ModifiedPattern.h"

vector<vector<unsigned int>> ModifiedPattern::nSet;
unsigned long long ModifiedPattern::area;
long long ModifiedPattern::membershipSum;
vector<vector<int>> ModifiedPattern::sumsOnHyperplanes;

NextStep ModifiedPattern::nextStep;
double ModifiedPattern::bestG;
vector<vector<unsigned int>>::iterator ModifiedPattern::bestDimensionIt;
vector<int>::const_iterator ModifiedPattern::bestSumIt;

const AbstractRoughTensor* ModifiedPattern::roughTensor;
bool ModifiedPattern::isIntermediaryPatternsOutput;
Trie ModifiedPattern::tensor;

#ifdef NCLUSTERBOX_REMEMBERS
unordered_set<vector<vector<unsigned int>>, nSetHash<unsigned int>> ModifiedPattern::visitedPatterns;
#endif

void ModifiedPattern::setContext(const AbstractRoughTensor* roughTensorParam, Trie& tensorParam, const bool isIntermediaryPatternsOutputParam)
{
  roughTensor = roughTensorParam;
  isIntermediaryPatternsOutput = isIntermediaryPatternsOutputParam;
  tensor = std::move(tensorParam);
  const vector<unsigned int>& cardinalities = AbstractRoughTensor::getCardinalities();
  sumsOnHyperplanes.reserve(cardinalities.size());
  for (const unsigned int cardinality : cardinalities)
    {
      sumsOnHyperplanes.emplace_back(cardinality);
    }
}

void ModifiedPattern::clearAndFree()
{
  nSet.clear();
  nSet.shrink_to_fit();
  sumsOnHyperplanes.clear();
  sumsOnHyperplanes.shrink_to_fit();
#ifdef NCLUSTERBOX_REMEMBERS
  visitedPatterns.clear();
#endif
}

void ModifiedPattern::init(vector<vector<unsigned int>>& nSetParam)
{
  nSet = std::move(nSetParam);
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  area = dimensionIt->size();
  for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
    {
      area *= dimensionIt->size();
    }
  if (Trie::is01)
    {
      membershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
    }
  else
    {
      membershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes);
    }
  bestG = abs(static_cast<double>(membershipSum)) * membershipSum / area;
#ifdef DEBUG_NCLUSTERBOX
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
  cout << " gives g = " << static_cast<float>(bestG) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
#ifdef NCLUSTERBOX_REMEMBERS
  visitedPatterns.emplace(nSet); // if input patterns can be subpatterns of each other (or even equal), it would be good to test whether the insertion occurred and, if not, continue with the next input pattern
#endif
}

const vector<vector<unsigned int>> ModifiedPattern::getFirstNonInitialAndSubsequentInitial()
{
  vector<vector<unsigned int>> firstNonInitialAndSubsequentInitial;
  firstNonInitialAndSubsequentInitial.reserve(nSet.size());
  for (const vector<unsigned int>& dimension : nSet)
    {
      const unsigned int sizeOfDimension = dimension.size();
      unsigned int elementId = 0;
      while (dimension[elementId] == elementId && ++elementId != sizeOfDimension)
	{
	}
      firstNonInitialAndSubsequentInitial.emplace_back();
      firstNonInitialAndSubsequentInitial.back().reserve(sizeOfDimension - elementId + 1);
      firstNonInitialAndSubsequentInitial.back().push_back(elementId);
      firstNonInitialAndSubsequentInitial.back().insert(firstNonInitialAndSubsequentInitial.back().end(), dimension.begin() + elementId, dimension.end());
    }
  return firstNonInitialAndSubsequentInitial;
}

bool ModifiedPattern::doStep()
{
  if (nextStep == stop)
    {
      considerAsFinal();
      return false;
    }
  area /= bestDimensionIt->size();
#ifdef NCLUSTERBOX_UPDATES_SUMS
  vector<unsigned int> singleElement {static_cast<unsigned int>(bestSumIt - sumsOnHyperplanes[bestDimensionIt - nSet.begin()].begin())};
#else
  const unsigned int element = static_cast<unsigned int>(bestSumIt - sumsOnHyperplanes[bestDimensionIt - nSet.begin()].begin());
#endif
  if (nextStep == insert)
    {
#ifdef DEBUG_NCLUSTERBOX
      cout << "    Adding slice for ";
#ifdef NCLUSTERBOX_UPDATES_SUMS
      AbstractRoughTensor::printElement(bestDimensionIt - nSet.begin(), singleElement.front(), cout);
#else
      AbstractRoughTensor::printElement(bestDimensionIt - nSet.begin(), element, cout);
#endif
      cout << " gives ";
#endif
#ifdef NCLUSTERBOX_UPDATES_SUMS
      bestDimensionIt->insert(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), singleElement.front()), singleElement.front());
#else
      bestDimensionIt->insert(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element), element);
#endif
#ifdef NCLUSTERBOX_REMEMBERS
      if (!visitedPatterns.emplace(nSet).second)
	{
#ifdef DEBUG_NCLUSTERBOX
	  roughTensor->printPattern(nSet, static_cast<float>(membershipSum + *bestSumIt) / (area * bestDimensionIt->size()), cout);
	  cout << ", which has already been reached: abort\n";
#endif
	  return false;
	}
#endif
#ifdef NCLUSTERBOX_UPDATES_SUMS
      singleElement.swap(*bestDimensionIt);
      if (Trie::is01)
	{
	  tensor.increaseSumsOnHyperplanes(nSet.begin(), bestDimensionIt - nSet.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
	}
      else
	{
	  tensor.increaseSumsOnHyperplanes(nSet.begin(), bestDimensionIt - nSet.begin(), sumsOnHyperplanes);
	}
      membershipSum += *bestSumIt;
#endif
    }
  else
    {
#ifdef DEBUG_NCLUSTERBOX
      cout << "    Removing slice for ";
#ifdef NCLUSTERBOX_UPDATES_SUMS
      AbstractRoughTensor::printElement(bestDimensionIt - nSet.begin(), singleElement.front(), cout);
#else
      AbstractRoughTensor::printElement(bestDimensionIt - nSet.begin(), element, cout);
#endif
      cout << " gives ";
#endif
#ifdef NCLUSTERBOX_UPDATES_SUMS
      bestDimensionIt->erase(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), singleElement.front()));
#else
      bestDimensionIt->erase(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element));
#endif
#ifdef NCLUSTERBOX_REMEMBERS
      if (!visitedPatterns.emplace(nSet).second)
	{
#ifdef DEBUG_NCLUSTERBOX
	  roughTensor->printPattern(nSet, static_cast<float>(membershipSum - *bestSumIt) / (area * bestDimensionIt->size()), cout);
	  cout << ", which has already been reached: abort\n";
#endif
	  return false;
	}
#endif
#ifdef NCLUSTERBOX_UPDATES_SUMS
      singleElement.swap(*bestDimensionIt);
      if (Trie::is01)
	{
	  tensor.increaseSumsOnHyperplanes(nSet.begin(), bestDimensionIt - nSet.begin(), sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit()); // negating the last two arguments for a decrease
	}
      else
	{
	  tensor.decreaseSumsOnHyperplanes(nSet.begin(), bestDimensionIt - nSet.begin(), sumsOnHyperplanes);
	}
      membershipSum -= *bestSumIt;
#endif
    }
#ifdef NCLUSTERBOX_UPDATES_SUMS
  area *= singleElement.size();
  singleElement.swap(*bestDimensionIt);
#else
  area *= bestDimensionIt->size();
  if (Trie::is01)
    {
      membershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
    }
  else
    {
      membershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes);
    }
#endif
#ifdef DEBUG_NCLUSTERBOX
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
  cout << " and g = " << static_cast<float>(bestG) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
  return true;
}

void ModifiedPattern::considerAsIntermediary()
{
  if (isIntermediaryPatternsOutput)
    {
      if (AbstractRoughTensor::isDirectOutput())
	{
	  roughTensor->output(nSet, static_cast<float>(membershipSum) / area);
	  return;
	}
      AbstractRoughTensor::copyAsCandidateVariable(nSet);
    }
}

void ModifiedPattern::considerAsFinal()
{
#ifdef DEBUG_NCLUSTERBOX
  cout << "    g's local maximum reached\n";
#endif
  if (!isIntermediaryPatternsOutput)
    {
      if (AbstractRoughTensor::isDirectOutput())
	{
	  roughTensor->output(nSet, static_cast<float>(membershipSum) / area);
	  return;
	}
      AbstractRoughTensor::moveAsCandidateVariable(nSet);
    }
}

void ModifiedPattern::modify(vector<vector<unsigned int>>& nSetParam)
{
  init(nSetParam);
  do
    {
      // Decide modification step
      considerAsIntermediary();
      nextStep = stop;
      vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
      const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
      for (vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin(); dimensionIt != dimensionEnd; ++dimensionIt)
	{
	  if (sumsInDimensionIt->size() == dimensionIt->size())
	    {
	      // Every element in the dimension of the tensor is present
	      if (dimensionIt->size() != 1)
		{
		  // Any element can be erased from *dimensionIt
		  const vector<int>::const_iterator bestDecreasingSumInDimensionIt = min_element(sumsInDimensionIt->begin(), sumsInDimensionIt->end()); // in case of equality, prefer removing the globally sparsest slice
		  double g = membershipSum - *bestDecreasingSumInDimensionIt;
		  g *= abs(g) / (area / dimensionIt->size() * (dimensionIt->size() - 1));
		  if (g > bestG)
		    {
		      bestG = g;
		      bestDimensionIt = dimensionIt;
		      bestSumIt = bestDecreasingSumInDimensionIt;
		      nextStep = erase;
		    }
		}
	      ++sumsInDimensionIt;
	      continue;
	    }
	  // Some element absent from *dimensionIt can be added
	  vector<int>::const_iterator bestIncreasingSumInDimensionIt;
	  vector<int>::const_iterator sumIt;
	  if (dimensionIt->size() == 1)
	    {
	      // No element can be erased from *dimensionIt
	      if (dimensionIt->front())
		{
		  sumIt = sumsInDimensionIt->begin() + dimensionIt->front();
		  bestIncreasingSumInDimensionIt = max_element(sumsInDimensionIt->begin(), sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
		  ++sumIt;
		}
	      else
		{
		  sumIt = ++(sumsInDimensionIt->begin());
		  bestIncreasingSumInDimensionIt = sumIt++;
		}
	    }
	  else
	    {
	      // Some element can be erased from *dimensionIt
	      vector<int>::const_iterator bestDecreasingSumInDimensionIt;
	      const vector<int>::const_iterator sumBegin = sumsInDimensionIt->begin();
	      vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
	      if (*presentElementIdIt)
		{
		  // Initializing bestDecreasingSumInDimensionIt with the sum relating to the first present element; bestIncreasingSumInDimensionIt with the greatest sum before
		  sumIt = sumBegin + *presentElementIdIt++;
		  bestIncreasingSumInDimensionIt = max_element(sumBegin, sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
		  bestDecreasingSumInDimensionIt = sumIt++;
		}
	      else
		{
		  // Initializing bestIncreasingSumInDimensionIt with the sum relating to the first absent element; bestDecreasingSumInDimensionIt with the lowest sum before
		  bestDecreasingSumInDimensionIt = sumBegin;
		  sumIt = sumBegin;
		  unsigned int elementId = 1;
		  for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd && *presentElementIdIt == elementId; ++elementId)
		    {
		      if (*++sumIt < *bestDecreasingSumInDimensionIt) // in case of equality, prefer removing the globally sparsest slice
			{
			  bestDecreasingSumInDimensionIt = sumIt;
			}
		    }
		  bestIncreasingSumInDimensionIt = ++sumIt++;
		}
	      // Compute bestDecreasingSumInDimensionIt and bestIncreasingSumInDimensionIt considering the sums until the one relating to the last present element
	      for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); presentElementIdIt != presentElementIdEnd; ++presentElementIdIt)
		{
		  for (const vector<int>::const_iterator end = sumBegin + *presentElementIdIt; sumIt != end; ++sumIt)
		    {
		      if (*sumIt >= *bestIncreasingSumInDimensionIt) // in case of equality, prefer adding the globally densest slice
			{
			  bestIncreasingSumInDimensionIt = sumIt;
			}
		    }
		  if (*sumIt < *bestDecreasingSumInDimensionIt) // in case of equality, prefer removing the globally sparsest slice
		    {
		      bestDecreasingSumInDimensionIt = sumIt;
		    }
		  ++sumIt;
		}
	      double g = membershipSum - *bestDecreasingSumInDimensionIt;
	      g *= abs(g) / (area / dimensionIt->size() * (dimensionIt->size() - 1));
	      if (g > bestG)
		{
		  bestG = g;
		  bestDimensionIt = dimensionIt;
		  bestSumIt = bestDecreasingSumInDimensionIt;
		  nextStep = erase;
		}
	    }
	  // Elements after the last present one
	  for (const vector<int>::const_iterator sumEnd = sumsInDimensionIt->end(); sumIt != sumEnd; ++sumIt)
	    {
	      if (*sumIt >= *bestIncreasingSumInDimensionIt) // in case of equality, prefer adding the globally densest slice
		{
		  bestIncreasingSumInDimensionIt = sumIt;
		}
	    }
	  double g = membershipSum + *bestIncreasingSumInDimensionIt;
	  g *= abs(g) / (area / dimensionIt->size() * (dimensionIt->size() + 1));
	  if (g > bestG)
	    {
	      bestG = g;
	      bestDimensionIt = dimensionIt;
	      bestSumIt = bestIncreasingSumInDimensionIt;
	      nextStep = insert;
	    }
	  ++sumsInDimensionIt;
	}
#ifdef ASSERT
      assertAreaAndSums();
#endif
    }
  while (doStep());
}

void ModifiedPattern::grow(vector<vector<unsigned int>>& nSetParam)
{
  init(nSetParam);
  const vector<vector<unsigned int>> firstNonInitialAndSubsequentInitial = getFirstNonInitialAndSubsequentInitial();
  do
    {
      // Decide growing step
      considerAsIntermediary();
      nextStep = stop;
      vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
      vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
      const vector<vector<unsigned int>>::const_iterator firstNonInitialAndSubsequentInitialEnd = firstNonInitialAndSubsequentInitial.end();
      for (vector<vector<unsigned int>>::const_iterator firstNonInitialAndSubsequentInitialIt = firstNonInitialAndSubsequentInitial.begin(); firstNonInitialAndSubsequentInitialIt != firstNonInitialAndSubsequentInitialEnd; ++firstNonInitialAndSubsequentInitialIt)
	{
	  unsigned int elementId = firstNonInitialAndSubsequentInitialIt->front();
	  vector<int>::const_iterator sumIt = sumsInDimensionIt->begin() + elementId;
	  if (sumIt != sumsInDimensionIt->end())
	    {
	      // Some element can be added or removed
	      int bestIncreasingSum;
	      vector<int>::const_iterator bestIncreasingSumInDimensionIt;
	      vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin() + elementId;
	      if (presentElementIdIt == dimensionIt->end())
		{
		  // No element can be removed
		  bestIncreasingSumInDimensionIt = sumIt;
		  bestIncreasingSum = *sumIt;
		}
	      else
		{
		  int bestDecreasingSum;
		  vector<int>::const_iterator bestDecreasingSumInDimensionIt;
		  if (*presentElementIdIt == elementId)
		    {
		      // First non initial element is present
		      ++presentElementIdIt;
		      bestDecreasingSumInDimensionIt = sumIt;
		      bestDecreasingSum = *sumIt;
		      bestIncreasingSumInDimensionIt = sumsInDimensionIt->end();
		      bestIncreasingSum = numeric_limits<int>::min();
		    }
		  else
		    {
		      // First non initial element is absent
		      bestDecreasingSumInDimensionIt = sumsInDimensionIt->end();
		      bestDecreasingSum = numeric_limits<int>::max();
		      bestIncreasingSumInDimensionIt = sumIt;
		      bestIncreasingSum = *sumIt;
		    }
		  // Elements until the last last initial one
		  vector<unsigned int>::const_iterator end = firstNonInitialAndSubsequentInitialIt->end();
		  for (vector<unsigned int>::const_iterator initialElementIt = firstNonInitialAndSubsequentInitialIt->begin(); ++initialElementIt != end; ++presentElementIdIt)
		    {
		      while (++elementId != *initialElementIt)
			{
			  if (elementId == *presentElementIdIt)
			    {
			      ++presentElementIdIt;
			      if (*++sumIt < bestDecreasingSum) // in case of equality, prefer removing the globally sparsest slice
				{
				  bestDecreasingSum = *sumIt;
				  bestDecreasingSumInDimensionIt = sumIt;
				}
			      continue;
			    }
			  if (*++sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
			    {
			      bestIncreasingSum = *sumIt;
			      bestIncreasingSumInDimensionIt = sumIt;
			    }
			}
		      ++sumIt;
		    }
		  // Elements after the last initial one
		  for (end = dimensionIt->end(); presentElementIdIt != end; ++presentElementIdIt)
		    {
		      while (++elementId != *presentElementIdIt)
			{
			  if (*++sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
			    {
			      bestIncreasingSum = *sumIt;
			      bestIncreasingSumInDimensionIt = sumIt;
			    }
			}
		      if (*++sumIt < bestDecreasingSum) // in case of equality, prefer removing the globally sparsest slice
			{
			  bestDecreasingSum = *sumIt;
			  bestDecreasingSumInDimensionIt = sumIt;
			}
		    }
		  if (bestDecreasingSum != numeric_limits<int>::max())
		    {
		      double g = membershipSum - bestDecreasingSum;
		      g *= abs(g) / (area / dimensionIt->size() * (dimensionIt->size() - 1));
		      if (g > bestG)
			{
			  bestG = g;
			  bestDimensionIt = dimensionIt;
			  bestSumIt = bestDecreasingSumInDimensionIt;
			  nextStep = erase;
			}
		    }
		}
	      // Elements after the last present one
	      for (const vector<int>::const_iterator sumEnd = sumsInDimensionIt->end(); ++sumIt != sumEnd; )
		{
		  if (*sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
		    {
		      bestIncreasingSum = *sumIt;
		      bestIncreasingSumInDimensionIt = sumIt;
		    }
		}
	      if (bestIncreasingSum != numeric_limits<int>::min())
		{
		  double g = membershipSum + bestIncreasingSum;
		  g *= abs(g) / (area / dimensionIt->size() * (dimensionIt->size() + 1));
		  if (g > bestG)
		    {
		      bestG = g;
		      bestDimensionIt = dimensionIt;
		      bestSumIt = bestIncreasingSumInDimensionIt;
		      nextStep = insert;
		    }
		}
	    }
	  ++sumsInDimensionIt;
	  ++dimensionIt;
	}
#ifdef ASSERT
      assertAreaAndSums();
#endif
    }
  while (doStep());
}

#ifdef ASSERT
void ModifiedPattern::assertAreaAndSums()
{
  {
    // Asserting area
    vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
    unsigned int actualArea = dimensionIt->size();
    for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
      {
	actualArea *= dimensionIt->size();
      }
    if (actualArea != area)
      {
	cerr << "area is " << actualArea << " and not " << area << ", as computed!\n";
      }
  }
  vector<vector<int>> actualSumsOnHyperplanes;
  {
    // Asserting membershipSum
    long long actualMembershipSum;
    const vector<unsigned int>& cardinalities = AbstractRoughTensor::getCardinalities();
    actualSumsOnHyperplanes.reserve(cardinalities.size());
    for (const unsigned int cardinality : cardinalities)
      {
	actualSumsOnHyperplanes.emplace_back(cardinality);
      }
    if (Trie::is01)
      {
	actualMembershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), actualSumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
      }
    else
      {
	actualMembershipSum = tensor.sumsOnPatternAndHyperplanes(nSet.begin(), actualSumsOnHyperplanes);
      }
    if (actualMembershipSum != membershipSum)
      {
	cerr << "membership sum is " << static_cast<double>(actualMembershipSum) / AbstractRoughTensor::getUnit() << " and not " << static_cast<double>(membershipSum) / AbstractRoughTensor::getUnit() << ", as computed!\n";
      }
  }
  // Asserting sumsOnHyperplanes
  vector<vector<int>>::const_iterator actualSumsInDimensionIt = actualSumsOnHyperplanes.begin();
  for (const vector<int>& sumsInDimension : sumsOnHyperplanes)
    {
      for (pair<vector<int>::const_iterator, vector<int>::const_iterator> mismatchingSumIts = mismatch(sumsInDimension.begin(), sumsInDimension.end(), actualSumsInDimensionIt->begin()); mismatchingSumIts.first != sumsInDimension.end(); mismatchingSumIts = mismatch(++mismatchingSumIts.first, sumsInDimension.end(), ++mismatchingSumIts.second))
	{
	  cerr << "sum on ";
	  AbstractRoughTensor::printElement(actualSumsInDimensionIt - actualSumsOnHyperplanes.begin(), mismatchingSumIts.first - sumsInDimension.begin(), cerr);
	  cerr << " is " << static_cast<double>(*mismatchingSumIts.second) / AbstractRoughTensor::getUnit() << " and not " << static_cast<double>(*mismatchingSumIts.first) / AbstractRoughTensor::getUnit() << ", as computed!\n";
	}
      ++actualSumsInDimensionIt;
    }
}
#endif
