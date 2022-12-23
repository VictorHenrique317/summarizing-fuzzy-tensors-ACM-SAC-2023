// Copyright 2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "OrderPatterns.h"

bool OrderPatterns::emptyIntersection(const vector<unsigned int>& dimension, vector<unsigned int>::const_iterator otherDimensionElementIt)
{
  const vector<unsigned int>::const_iterator elementEnd = dimension.end();
  vector<unsigned int>::const_iterator elementIt = dimension.begin();
  do
    {
      for (; *otherDimensionElementIt < *elementIt; ++otherDimensionElementIt)
	{
	}
      if (*otherDimensionElementIt == *elementIt)
	{
	  return false;
	}
    }
  while (++elementIt != elementEnd);
  return true;
}

bool OrderPatterns::overlapsWith(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>& otherNSet)
{
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (const vector<unsigned int>& otherDimension : otherNSet)
    {
      if (*--dimensionIt->end() < *--otherDimension.end())
	{
	  if (emptyIntersection(*dimensionIt, otherDimension.begin()))
	    {
	      return false;
	    }
	}
      else
	{
	  if (emptyIntersection(otherDimension, dimensionIt->begin()))
	    {
	      return false;
	    }
	}
      ++dimensionIt;
    }
  return true;
}

void OrderPatterns::order(const unsigned int nbOfCandidateVariablesHavingAllElements, AbstractRoughTensor* roughTensor, const bool isVerbose, const bool isRSSPrinted)
{
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
  cout << '\t' << AbstractRoughTensor::getCandidateVariables().size();
#else
  cout << "Nb of patterns candidates for selection: " << AbstractRoughTensor::getCandidateVariables().size() << '\n';
#endif
#endif
  // Set tensor
#ifdef DETAILED_TIME
  steady_clock::time_point startingPoint = steady_clock::now();
#endif
  if (isVerbose)
    {
      cout << "Reducing fuzzy tensor to elements in patterns ... " << flush;
    }
  TrieWithPrediction tensor = roughTensor->projectTensor(nbOfCandidateVariablesHavingAllElements);
  if (isVerbose)
    {
      cout << "\rReducing fuzzy tensor to elements in patterns: done.\n";
    }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
  cout << "Tensor reduction time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
  startingPoint = steady_clock::now();
#endif
#endif
  if (isVerbose)
    {
      cout << "Selecting patterns ... " << flush;
    }
  double quadraticError = AbstractRoughTensor::getNullModelQuadraticError();
  // Construct the candidates
  multimap<long long, CandidateVariable> candidates; /* ordered by lower bound of the quadratic error variation if selected */
  vector<vector<vector<unsigned int>>>& raw = AbstractRoughTensor::getCandidateVariables();
  for (vector<vector<unsigned int>>& pattern : raw)
    {
      const int density = tensor.density(pattern);
      if (density > 0)
	{
	  const long long lowerBound = density * tensor.sumNegativeDensityMinus2Memberships(pattern, density);
	  candidates.emplace(lowerBound, CandidateVariable(pattern, density));
	}
    }
  raw.clear();
  raw.shrink_to_fit();
#ifdef DEBUG_SELECT
  cout << "Quadratic error of null model: " << quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
  // Find the first pattern to select
  long long minQuadraticErrorVariation = 0;
  multimap<long long, CandidateVariable>::iterator bestCandidateIt = candidates.end();
  const multimap<long long, CandidateVariable>::iterator candidateEnd = bestCandidateIt;
  for (multimap<long long, CandidateVariable>::iterator candidateIt = candidates.begin(); candidateIt != candidateEnd && candidateIt->first < minQuadraticErrorVariation; ++candidateIt)
    {
      const long long minusG = -candidateIt->second.getG();
      if (minusG < minQuadraticErrorVariation)
	{
	  minQuadraticErrorVariation = minusG;
	  bestCandidateIt = candidateIt;
	}
    }
  if (bestCandidateIt == candidateEnd)
    {
      if (isVerbose)
	{
	  cout << "\rSelecting patterns: no pattern selected.\n";
	}
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
      cout << "\t0";
#else
      cout << "Nb of selected patterns: 0\n";
#endif
#endif
    }
  else
    {
#ifdef DEBUG_SELECT
      roughTensor->printPattern(bestCandidateIt->second.nSet, bestCandidateIt->second.density, cout);
      cout << " decreases the quadratic error, " << quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << ", the most, by " << static_cast<double>(-minQuadraticErrorVariation) * 100 / quadraticError << "%\n";
#endif
      if (isVerbose)
	{
	  cout << "\rSelecting patterns: 1 pattern selected." << flush;
	}
      quadraticError += minQuadraticErrorVariation;
      if (isRSSPrinted)
	{
	  roughTensor->output(bestCandidateIt->second.nSet, bestCandidateIt->second.density, quadraticError);
	}
      else
	{
	  roughTensor->output(bestCandidateIt->second.nSet, bestCandidateIt->second.density);
	}
      tensor.addFirstPatternToModel(bestCandidateIt->second.nSet, bestCandidateIt->second.density);
      candidates.erase(bestCandidateIt);
      minQuadraticErrorVariation = 0;
      unsigned int nbOfSelectedPatterns = 1;
      multimap<long long, pair<long long, CandidateVariable>> updated; /* ordered by quadratic error variation if selected */
      for (; ; )
	{
	  // Find the next pattern to select
	  while (!candidates.empty() && candidates.begin()->first < minQuadraticErrorVariation)
	    {
	      // It is faster to seize the similar computation of the quadratic error variation to compute as well the new lower bound here (and to store it in updated) rather than when reinserting in candidates
	      const pair<long long, long long> lowerBoundAndQuadraticErrorVariation = tensor.lowerBoundAndQuadraticErrorVariation(candidates.begin()->second.nSet, candidates.begin()->second.density);
	      if (lowerBoundAndQuadraticErrorVariation.second < minQuadraticErrorVariation)
	      	{
	      	  minQuadraticErrorVariation = lowerBoundAndQuadraticErrorVariation.second;
	      	  updated.emplace_hint(updated.begin(), lowerBoundAndQuadraticErrorVariation.second, make_pair(lowerBoundAndQuadraticErrorVariation.first, std::move(candidates.begin()->second)));
	      	}
	      else
	      	{
	      	  updated.emplace(lowerBoundAndQuadraticErrorVariation.second, make_pair(lowerBoundAndQuadraticErrorVariation.first, std::move(candidates.begin()->second)));
	      	}
	      candidates.erase(candidates.begin());
	    }
	  if (updated.empty() || updated.begin()->first >= 0)
	    {
	      break;
	    }
	  CandidateVariable& selected = updated.begin()->second.second;
#ifdef DEBUG_SELECT
	  roughTensor->printPattern(selected.nSet, selected.density, cout);
	  cout << " decreases the quadratic error, " << quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << ", the most, by " << static_cast<double>(-minQuadraticErrorVariation) * 100 / quadraticError << "%\n";
#endif
	  quadraticError += minQuadraticErrorVariation;
	  if (isRSSPrinted)
	    {
	      roughTensor->output(selected.nSet, selected.density, quadraticError);
	    }
	  else
	    {
	      roughTensor->output(selected.nSet, selected.density);
	    }
	  if (isVerbose)
	    {
	      cout << "\rSelecting patterns: " << ++nbOfSelectedPatterns << " patterns selected." << flush;
	    }
#ifdef NB_OF_PATTERNS
	  else
	    {
	      ++nbOfSelectedPatterns;
	    }
#endif
	  tensor.addPatternToModel(selected.nSet, selected.density);
	  const vector<vector<unsigned int>> nSet = std::move(selected.nSet);
	  const multimap<long long, pair<long long, CandidateVariable>>::iterator end = updated.end();
	  for (multimap<long long, pair<long long, CandidateVariable>>::iterator patternIt = updated.erase(updated.begin()); patternIt != end; )
	    {
	      // Slower but still correct if the test below is considered always true
	      if (overlapsWith(nSet, patternIt->second.second.nSet))
		{
		  candidates.emplace(std::move(patternIt->second));
		  patternIt = updated.erase(patternIt);
		}
	      else
		{
		  ++patternIt;
		}
	    }
	  if (updated.empty() || updated.begin()->first > 0)
	    {
	      minQuadraticErrorVariation = 0;
	    }
	  else
	    {
	      minQuadraticErrorVariation = updated.begin()->first;
	    }
	}
      if (isVerbose)
	{
	  cout << '\n';
	}
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
      cout << '\t' << nbOfSelectedPatterns;
#else
      cout << "Nb of selected patterns: " << nbOfSelectedPatterns << '\n';
#endif
#endif
    }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
  cout << "Selection time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
#endif
}
