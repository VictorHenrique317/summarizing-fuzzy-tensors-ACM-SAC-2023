// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "Pattern.h"

const AbstractRoughTensor* Pattern::roughTensor;
Trie Pattern::tensor;
vector<Pattern> Pattern::patterns;
unordered_set<vector<vector<unsigned int>>, nSetHash<unsigned int>> Pattern::visitedPatterns;
bool Pattern::isIntermediaryPatternsOutput;

Pattern::Pattern(Pattern&& otherPattern): nSet(std::move(otherPattern.nSet)), membershipSum(otherPattern.membershipSum), dimensionIdOfNextElement(otherPattern.dimensionIdOfNextElement), nextElement(otherPattern.nextElement), membershipSumOnNextElement(otherPattern.membershipSumOnNextElement), rssDiff(otherPattern.rssDiff)
{
}

Pattern::Pattern(vector<vector<unsigned int>>& nSetParam): nSet(std::move(nSetParam)), membershipSum(), dimensionIdOfNextElement(), nextElement(), membershipSumOnNextElement(), rssDiff()
{
  if (Trie::is01)
    {
      membershipSum = SparseCrispTube::getDefaultMembership();
      for (const vector<unsigned int>& dimension : nSet)
	{
	  membershipSum *= dimension.size();
	}
      membershipSum += tensor.nbOfPresentTuples(nSet) * AbstractRoughTensor::getUnit();
      return;
    }
  membershipSum = tensor.membershipSum(nSet);
}

Pattern& Pattern::operator=(Pattern&& otherPattern)
{
  nSet = std::move(otherPattern.nSet);
  membershipSum = otherPattern.membershipSum;
  dimensionIdOfNextElement = otherPattern.dimensionIdOfNextElement;
  nextElement = otherPattern.nextElement;
  membershipSumOnNextElement = otherPattern.membershipSumOnNextElement;
  rssDiff = otherPattern.rssDiff;
  return *this;
}

ostream& operator<<(ostream& out, const Pattern& pattern)
{
  Pattern::roughTensor->printPattern(pattern.nSet, static_cast<float>(pattern.membershipSum) / pattern.area(), out);
  return out;
}

unsigned int Pattern::area() const
{
  unsigned int area = 1;
  for (const vector<unsigned int>& dimension : nSet)
    {
      area *= dimension.size();
    }
  return area;
}

bool Pattern::subPatternOf(const Pattern& otherPattern)
{
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (vector<vector<unsigned int>>::const_iterator otherDimensionIt = otherPattern.nSet.begin(); dimensionIt != dimensionEnd && includes(otherDimensionIt->begin(), otherDimensionIt->end(), dimensionIt->begin(), dimensionIt->end()); ++otherDimensionIt)
    {
      ++dimensionIt;
    }
  if (dimensionIt != dimensionEnd)
    {
      return false;
    }
#ifdef DEBUG_GROW
  cout << "Erasing " << *this << ", sub-pattern of " << otherPattern << '\n';
#endif
  outputOrMoveAsCandidateVariable(nSet, membershipSum);
  return true;
}

bool Pattern::subPatternOf(const vector<Pattern>::const_iterator begin, const vector<Pattern>::const_iterator end)
{
  vector<Pattern>::const_iterator patternIt = begin;
  for (; patternIt != end && !subPatternOf(*patternIt); ++patternIt)
    {
    }
  return patternIt != end;
}

InclusionResult Pattern::subOrSuperPatternOf(Pattern& otherPattern)
{
  vector<vector<unsigned int>>::const_iterator otherDimensionIt = otherPattern.nSet.begin();
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (; dimensionIt != dimensionEnd && dimensionIt->size() == otherDimensionIt->size(); ++dimensionIt)
    {
      ++otherDimensionIt;
    }
  if (dimensionIt == dimensionEnd)
    {
      // Same sizes but pattern cannot be equal to otherPattern because that was tested earlier (alreadyFound)
      return incomparable;
    }
  if (dimensionIt->size() < otherDimensionIt->size())
    {
      if (subPatternOf(otherPattern))
	{
	  return sub;
	}
      return incomparable;
    }
  if (otherPattern.subPatternOf(*this))
    {
      return super;
    }
  return incomparable;
}

bool Pattern::findNextElement(const vector<vector<int>>& extensionSums)
{
  const vector<vector<int>>::const_iterator extensionSumEnd = extensionSums.end();
  long long bestAdjustedNextMembershipSum;
  vector<vector<int>>::const_iterator extensionSumsInNextElementDimensionIt = extensionSums.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  unsigned int sizeOfNextElementDimension = dimensionIt->size();
  vector<int>::const_iterator extensionIt = Trie::maxExtension(*extensionSumsInNextElementDimensionIt);
  if (Trie::is01)
    {
      // Advance to the next dimension where some element is absent (*extensionIt != numeric_limits<int>::min(), otherwise defining membershipSumOnNextElement would underflow)
      for (; *extensionIt == numeric_limits<int>::min(); ++dimensionIt)
	{
	  // *dimensionIt contains all elements
	  if (++extensionSumsInNextElementDimensionIt == extensionSumEnd)
	    {
	      break;
	    }
	  extensionIt = Trie::maxExtension(*extensionSumsInNextElementDimensionIt);
	}
      if (*extensionIt == numeric_limits<int>::min())
	{
	  // *this is the whole tensor
	  membershipSumOnNextElement = 0;
	  return true;
	}
      long long onlyNoise = SparseCrispTube::getDefaultMembership();
      for (const vector<unsigned int>& dimension : nSet)
	{
	  onlyNoise *= dimension.size();
	}
      membershipSumOnNextElement = onlyNoise / sizeOfNextElementDimension + *extensionIt * AbstractRoughTensor::getUnit();
      bestAdjustedNextMembershipSum = (membershipSum + membershipSumOnNextElement) * sizeOfNextElementDimension;
      ++sizeOfNextElementDimension;
      bestAdjustedNextMembershipSum = bestAdjustedNextMembershipSum / sizeOfNextElementDimension + bestAdjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
      for (vector<vector<int>>::const_iterator extensionSumsInDimensionIt = extensionSumsInNextElementDimensionIt; ++extensionSumsInDimensionIt != extensionSumEnd; )
	{
	  vector<int>::const_iterator extensionInDimensionIt = Trie::maxExtension(*extensionSumsInDimensionIt);
	  // Advance to the next dimension where some element is absent (*extensionInDimensionIt != numeric_limits<int>::min(), otherwise defining membershipSumOnExtension would underflow)
	  for (++dimensionIt; *extensionInDimensionIt == numeric_limits<int>::min(); ++dimensionIt)
	    {
	      // *dimensionIt contains all elements
	      if (++extensionSumsInDimensionIt == extensionSumEnd)
		{
		  break;
		}
	      extensionInDimensionIt = Trie::maxExtension(*extensionSumsInDimensionIt);
	    }
	  if (*extensionInDimensionIt == numeric_limits<int>::min())
	    {
	      break;
	    }
	  sizeOfNextElementDimension = dimensionIt->size();
	  const int membershipSumOnExtension = onlyNoise / sizeOfNextElementDimension + *extensionInDimensionIt * AbstractRoughTensor::getUnit();
	  long long adjustedNextMembershipSum = (membershipSum + membershipSumOnExtension) * sizeOfNextElementDimension;
	  ++sizeOfNextElementDimension;
	  adjustedNextMembershipSum = adjustedNextMembershipSum / sizeOfNextElementDimension + adjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
	  if (adjustedNextMembershipSum > bestAdjustedNextMembershipSum)
	    {
	      bestAdjustedNextMembershipSum = adjustedNextMembershipSum;
	      membershipSumOnNextElement = membershipSumOnExtension;
	      extensionSumsInNextElementDimensionIt = extensionSumsInDimensionIt;
	      extensionIt = extensionInDimensionIt;
	    }
	}
    }
  else
    {
      bestAdjustedNextMembershipSum = (membershipSum + *extensionIt) * sizeOfNextElementDimension;
      ++sizeOfNextElementDimension;
      bestAdjustedNextMembershipSum = bestAdjustedNextMembershipSum / sizeOfNextElementDimension + bestAdjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
      for (vector<vector<int>>::const_iterator extensionSumsInDimensionIt = extensionSumsInNextElementDimensionIt; ++extensionSumsInDimensionIt != extensionSumEnd; )
	{
	  sizeOfNextElementDimension = (++dimensionIt)->size();
	  const vector<int>::const_iterator extensionInDimensionIt = Trie::maxExtension(*extensionSumsInDimensionIt);
	  long long adjustedNextMembershipSum = (membershipSum + *extensionInDimensionIt) * sizeOfNextElementDimension;
	  ++sizeOfNextElementDimension;
	  adjustedNextMembershipSum = adjustedNextMembershipSum / sizeOfNextElementDimension + adjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
	  if (adjustedNextMembershipSum > bestAdjustedNextMembershipSum)
	    {
	      bestAdjustedNextMembershipSum = adjustedNextMembershipSum;
	      extensionSumsInNextElementDimensionIt = extensionSumsInDimensionIt;
	      extensionIt = extensionInDimensionIt;
	    }
	}
      membershipSumOnNextElement = *extensionIt;
    }
  dimensionIdOfNextElement = extensionSumsInNextElementDimensionIt - extensionSums.begin();
  nextElement = extensionIt - extensionSumsInNextElementDimensionIt->begin();
#ifdef DEBUG_GROW
  if (bestAdjustedNextMembershipSum <= membershipSum)
    {
      const unsigned int sizeOfNextElementDimension = nSet[dimensionIdOfNextElement].size();
      const long long patternArea = area();
      if (membershipSumOnNextElement <= 0)
	{
	  cout << "Any extension of " << *this << " is negative\n";
	  return true;
	}
      long long newRssDiff = membershipSum - bestAdjustedNextMembershipSum;
      newRssDiff *= newRssDiff / patternArea;
      cout << "Next element for " << *this << " is ";
      AbstractRoughTensor::printElement(dimensionIdOfNextElement, nextElement, cout);
      cout << "; its shifted density " << static_cast<float>(membershipSumOnNextElement) * sizeOfNextElementDimension / patternArea / AbstractRoughTensor::getUnit() << " lowers the current one, " << static_cast<float>(membershipSum) / patternArea / AbstractRoughTensor::getUnit() << "; RSS difference = " << patternArea << " * (" << static_cast<float>(membershipSum) / patternArea / AbstractRoughTensor::getUnit() << " - " << static_cast<float>(bestAdjustedNextMembershipSum) / patternArea / AbstractRoughTensor::getUnit() << ")² = " << static_cast<double>(newRssDiff) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit();
    }
#endif
  return bestAdjustedNextMembershipSum <= membershipSum;
}

bool Pattern::extend(const vector<vector<int>>::iterator extensionSumsBegin)
{
  vector<unsigned int>& dimension = nSet[dimensionIdOfNextElement];
  dimension.insert(lower_bound(dimension.begin(), dimension.end(), nextElement), nextElement);
  if (visitedPatterns.find(nSet) == visitedPatterns.end())
    {
      membershipSum += membershipSumOnNextElement;
#ifdef DEBUG_GROW
      cout << *this << " obtained after extension with ";
      AbstractRoughTensor::printElement(dimensionIdOfNextElement, nextElement, cout);
      cout << " and shifted density " << static_cast<float>(membershipSumOnNextElement) * nSet[dimensionIdOfNextElement].size() / area() / AbstractRoughTensor::getUnit() << '\n';
#endif
      visitedPatterns.emplace(nSet);
      const vector<unsigned int> extendedDimension(std::move(dimension));
      dimension = {nextElement};
      const vector<vector<int>>::iterator extensionSumsInDimensionIt = extensionSumsBegin + dimensionIdOfNextElement;
      (*extensionSumsInDimensionIt)[nextElement] = numeric_limits<int>::min();
      tensor.updateSumsOnExtensionsAfterExtension(nSet.begin(), extensionSumsBegin, extensionSumsInDimensionIt);
      dimension = std::move(extendedDimension);
      return false;
    }
#ifdef DEBUG_GROW
  cout << *this << " would have been obtained after extension with ";
  AbstractRoughTensor::printElement(dimensionIdOfNextElement, nextElement, cout);
  cout << " and shifted density " << static_cast<float>(membershipSumOnNextElement) * nSet[dimensionIdOfNextElement].size() / area() / AbstractRoughTensor::getUnit() << " but was discarded for having already been built\n";
#endif
  return true;
}

MaximizationResult Pattern::maximizeDensityAndFindNextElement()
{
  vector<vector<int>> extensionSums = tensor.sumsOnExtensions(nSet.begin(), AbstractRoughTensor::getCardinalities());
  for (MaximizationResult result = alreadyMaximal; ; result = maximized)
    {
      if (findNextElement(extensionSums))
	{
	  if (membershipSum <= 0)
	    {
	      return alreadyFoundOrNegative; // negative density
	    }
	  if (membershipSumOnNextElement <= 0)
	    {
	      rssDiff = numeric_limits<unsigned long long>::max();
	      return result;
	    }
#ifdef DEBUG_GROW
	  cout << '\n';
#endif
	  const unsigned int sizeOfNextElementDimension = nSet[dimensionIdOfNextElement].size();
	  rssDiff = membershipSum - (membershipSum + membershipSumOnNextElement) * sizeOfNextElementDimension / (sizeOfNextElementDimension + 1);
	  rssDiff *= rssDiff / area();
	  return result;
	}
      if (extend(extensionSums.begin()))
	{
	  return alreadyFoundOrNegative; // already found
	}
    }
}

ExtensionResult Pattern::extendAndFindNextElement()
{
#ifdef DEBUG_GROW
  cout << "Extending " << *this << " with ";
  AbstractRoughTensor::printElement(dimensionIdOfNextElement, nextElement, cout);
  cout << " and shifted density " << static_cast<float>(membershipSumOnNextElement) * nSet[dimensionIdOfNextElement].size() / area() / AbstractRoughTensor::getUnit() << ": shifted density lowers from " << static_cast<float>(membershipSum) / area() / AbstractRoughTensor::getUnit() << " to " << static_cast<float>(membershipSum + membershipSumOnNextElement) * nSet[dimensionIdOfNextElement].size() / (nSet[dimensionIdOfNextElement].size() + 1) / area() / AbstractRoughTensor::getUnit() << '\n';
#endif
  vector<pair<vector<vector<unsigned int>>, long long>> intermediaryPatterns(1, {nSet, membershipSum});
  nSet[dimensionIdOfNextElement].insert(lower_bound(nSet[dimensionIdOfNextElement].begin(), nSet[dimensionIdOfNextElement].end(), nextElement), nextElement);
  if (visitedPatterns.find(nSet) == visitedPatterns.end())
    {
      membershipSum += membershipSumOnNextElement;
      visitedPatterns.emplace(nSet);
      vector<vector<int>> extensionSums = tensor.sumsOnExtensions(nSet.begin(), AbstractRoughTensor::getCardinalities());
      for (; ; )
	{
	  if (findNextElement(extensionSums))
	    {
	      // Extension is lowering the density
	      if (membershipSumOnNextElement <= 0)
		{
		  // Negative extension
		  rssDiff = numeric_limits<unsigned long long>::max();
		  outputIntermediaryPatterns(intermediaryPatterns);
		  return extended;
		}
	      const unsigned int sizeOfNextElementDimension = nSet[dimensionIdOfNextElement].size();
	      const unsigned int patternArea = area();
	      unsigned long long newRssDiff = membershipSum - (membershipSum + membershipSumOnNextElement) * sizeOfNextElementDimension / (sizeOfNextElementDimension + 1);
	      newRssDiff *= newRssDiff / patternArea;
	      if (newRssDiff > rssDiff)
		{
#ifdef DEBUG_GROW
		  cout << '\n';
#endif
		  rssDiff = newRssDiff;
		  outputIntermediaryPatterns(intermediaryPatterns);
		  return extended;
		}
	      newRssDiff = membershipSum - membershipSum * sizeOfNextElementDimension / (sizeOfNextElementDimension + 1);
	      newRssDiff *= newRssDiff / patternArea;
	      if (newRssDiff <= rssDiff)
		{
#ifdef DEBUG_GROW
		  cout << ": a decrease from the previous value, " << static_cast<double>(rssDiff) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << " but even a null extension would do -> stop growing!\n";
#endif
		  nSet = std::move(intermediaryPatterns.front().first);
		  membershipSum = intermediaryPatterns.front().second;
		  return infinitelyGrowing;
		}
#ifdef DEBUG_GROW
	      cout << ": a decrease from the previous value, " << static_cast<double>(rssDiff) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << " -> extend!\n";
#endif
	      if (isIntermediaryPatternsOutput)
		{
		  intermediaryPatterns.emplace_back(nSet, membershipSum);
		}
	    }
	  if (extend(extensionSums.begin()))
	    {
	      outputIntermediaryPatterns(intermediaryPatterns);
	      return alreadyFound;
	    }
	}
    }
  // After the initial extension, *this is a pattern that has already been built, output the pattern before the extension (the only element in intermediaryPatterns)
  outputOrMoveAsCandidateVariable(intermediaryPatterns.front().first, intermediaryPatterns.front().second);
  return alreadyFound;
}

void Pattern::output(const vector<vector<unsigned int>>& nSet, const long long membershipSum)
{
  float density = membershipSum;
  for (const vector<unsigned int>& dimension : nSet)
    {
      density /= dimension.size();
    }
  roughTensor->output(nSet, density);
}

void Pattern::outputOrMoveAsCandidateVariable(vector<vector<unsigned int>>& nSet, const long long membershipSum)
{
  if (isIntermediaryPatternsOutput)
    {
      if (AbstractRoughTensor::isDirectOutput())
	{
	  output(nSet, membershipSum);
	}
      else
	{
	  AbstractRoughTensor::moveAsCandidateVariable(nSet);
	}
    }
}

void Pattern::grow(const char* patternFileName, const char* patternDimensionSeparator, const char* patternElementSeparator, const AbstractRoughTensor* roughTensorParam, Trie& tensorParam, const unsigned int nbOfGrownPatterns, const bool isInputMaximal, const bool isIntermediaryPatternsOutputParam, const bool isVerbose)
{
#ifdef DETAILED_TIME
  steady_clock::time_point startingPoint = steady_clock::now();
#endif
  roughTensor = roughTensorParam;
  isIntermediaryPatternsOutput = isIntermediaryPatternsOutputParam;
  tensor = std::move(tensorParam);
  vector<Pattern>::iterator selectedPatternEnd;
  PatternFileReader patternFileReader(patternFileName, patternDimensionSeparator, patternElementSeparator, AbstractRoughTensor::getIds2Labels());
  if (isInputMaximal)
    {
      // No pairwise inclusion between the input patterns (if the assumption does not hold, some patterns are not erased and grown patterns may not be unique)
      vector<Pattern> alreadyDensityMaximalPatterns;
      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
	{
	  Pattern pattern(nSet);
	  const MaximizationResult result = pattern.maximizeDensityAndFindNextElement();
	  if (result != alreadyFoundOrNegative)
	    {
	      if (result == maximized)
		{
		  insertAndEraseSubPatterns(pattern);
		}
	      else
		{
		  alreadyDensityMaximalPatterns.emplace_back(std::move(pattern));
		}
	      if (isVerbose)
		{
		  cout << "\rMaximizing pattern densities by growing: " << patterns.size() << " maximized + " << alreadyDensityMaximalPatterns.size() << " already maximal." << flush;
		}
	    }
	}
      patterns.reserve(patterns.size() + alreadyDensityMaximalPatterns.size());
      selectedPatternEnd = patterns.end();
      patterns.insert(selectedPatternEnd, std::make_move_iterator(alreadyDensityMaximalPatterns.begin()), std::make_move_iterator(alreadyDensityMaximalPatterns.end()));
      if (patterns.empty())
	{
	  throw UsageException(("No pattern in " + string(patternFileName) + '!').c_str());
	}
      if (isVerbose)
	{
	  cout << '\n';
	}
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
      cout << "Initial density maximization time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
      startingPoint = steady_clock::now();
#endif
      if (isVerbose)
	{
	  cout << "Discarding sub-patterns of density-maximized patterns ... " << flush;
	}
      if (selectedPatternEnd == patterns.begin())
	{
	  // No pattern had its density increased (a case eraseSubPatternsAndSetNextPatternsToGrow does not treat)
	  vector<Pattern>::iterator minRssDiffBegin = selectedPatternEnd;
	  vector<Pattern>::iterator minRssDiffEnd = selectedPatternEnd + 1;
	  const vector<Pattern>::iterator patternEnd = patterns.end();
	  for (vector<Pattern>::iterator patternIt = minRssDiffEnd; patternIt != patternEnd; ++patternIt)
	    {
	      updateMinRssDiff(patternIt, minRssDiffBegin, minRssDiffEnd);
	    }
	  do
	    {
	      iter_swap(--minRssDiffEnd, selectedPatternEnd++);
	    }
	  while (minRssDiffEnd != minRssDiffBegin);
	}
      else
	{
	  // Some pattern had its density increased
	  selectedPatternEnd = eraseSubPatternsAndSetNextPatternsToGrow(selectedPatternEnd);
	}
      if (isVerbose)
	{
	  cout << "\rDiscarding sub-patterns of density-maximized patterns: " << patterns.size() << " patterns remaining.\n";
	}
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
      cout << "Initial sub-pattern discarding time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
      startingPoint = steady_clock::now();
#endif
    }
  else
    {
      // Pairwise inclusion (but not equality!) between the input patterns is possible, according to the user
      for (vector<vector<unsigned int>> nSet = patternFileReader.next(); !nSet.empty(); nSet = patternFileReader.next())
	{
	  Pattern pattern(nSet);
	  const MaximizationResult result = pattern.maximizeDensityAndFindNextElement();
	  if (result != alreadyFoundOrNegative)
	    {
	      insertAndEraseSubPatterns(pattern);
	      if (isVerbose)
		{
		  cout << "\rMaximizing pattern densities by growing: " << patterns.size() << " maximal." << flush;
		}
	    }
	}
      if (patterns.empty())
	{
	  throw UsageException(("No pattern in " + string(patternFileName) + '!').c_str());
	}
      if (isVerbose)
	{
	  cout << '\n';
	}
      // set first patterns to grow
      selectedPatternEnd = patterns.begin();
      vector<Pattern>::iterator minRssDiffBegin = selectedPatternEnd;
      const vector<Pattern>::iterator patternEnd = patterns.end();
      vector<Pattern>::iterator minRssDiffEnd = selectedPatternEnd + 1;
      for (vector<Pattern>::iterator patternIt = minRssDiffEnd; patternIt != patternEnd; ++patternIt)
	{
	  updateMinRssDiff(patternIt, minRssDiffBegin, minRssDiffEnd);
	}
      do
	{
	  iter_swap(--minRssDiffEnd, selectedPatternEnd++);
	}
      while (minRssDiffEnd != minRssDiffBegin);
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "\t0";
#else
      cout << "Initial density maximization time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\nInitial sub-pattern discarding time: 0s\n";
  startingPoint = steady_clock::now();
#endif
#endif
    }
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
#if defined NUMERIC_PRECISION || defined DETAILED_TIME
  cout << '\t';
#endif
  cout << patterns.size();
#else
  cout << "Nb of density-maximized patterns: " << patterns.size() << '\n';
#endif
#endif
  if (nbOfGrownPatterns < patterns.size())
    {
      if (isVerbose)
	{
	  if (nbOfGrownPatterns == 0)
	    {
	      cout << "Further growing patterns and discarding sub-patterns ... " << flush;
	    }
	  else
	    {
	      CompletionMessage<unsigned int>::set("Further growing patterns and discarding sub-patterns", patterns.size() - nbOfGrownPatterns);
	    }
	}
      bool isPatternsFinitelyGrowing = true;
      for (; nbOfGrownPatterns < patterns.size() && isPatternsFinitelyGrowing && patterns.front().rssDiff != numeric_limits<unsigned long long>::max(); selectedPatternEnd = eraseSubPatternsAndSetNextPatternsToGrow(selectedPatternEnd))
	{
#ifdef DEBUG_GROW
	  cout << "Extending " << selectedPatternEnd - patterns.begin() << " pattern(s) leading to a " << static_cast<float>(patterns.front().rssDiff) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << " quadratic error variation\n";
#endif
	  visitedPatterns = {};
	  if (isVerbose && nbOfGrownPatterns != 0)
	    {
	      CompletionMessage<unsigned int>::print(patterns.size() - nbOfGrownPatterns);
	    }
	  vector<Pattern>::iterator patternEnd = patterns.end();
	  for (vector<Pattern>::iterator selectedPatternIt = patterns.begin(); selectedPatternIt != selectedPatternEnd; )
	    {
	      const ExtensionResult result = selectedPatternIt->extendAndFindNextElement();
	      if (result == extended)
		{
		  vector<Pattern>::iterator previouslyGrownPatternIt = patterns.begin();
		  if (previouslyGrownPatternIt == selectedPatternIt)
		    {
		      // No previously selected pattern
		      ++selectedPatternIt;
		    }
		  else
		    {
		      InclusionResult inclusionResult;
		      do
			{
			  inclusionResult = selectedPatternIt->subOrSuperPatternOf(*previouslyGrownPatternIt);
			} while (inclusionResult == incomparable && ++previouslyGrownPatternIt != selectedPatternIt);
		      if (inclusionResult == incomparable)
			{
			  ++selectedPatternIt;
			}
		      else
			{
			  if (inclusionResult == sub)
			    {
			      // Remove *selectedPatternIt because it is not maximal
			      overwriteWithSubsequentPattern(selectedPatternIt, --selectedPatternEnd, --patternEnd);
			    }
			  else
			    {
			      // Remove any previously selected pattern that is a sub-pattern of *selectedPatternIt
			      overwriteWithSubsequentPattern(previouslyGrownPatternIt, selectedPatternIt, --selectedPatternEnd);
			      if (selectedPatternEnd != --patternEnd)
				{
				  *selectedPatternEnd = std::move(*patternEnd);
				}
			      for (vector<Pattern>::iterator otherPreviouslyGrownPatternIt = previouslyGrownPatternIt + 1; otherPreviouslyGrownPatternIt != selectedPatternIt; )
				{
				  if (otherPreviouslyGrownPatternIt->subPatternOf(*previouslyGrownPatternIt))
				    {
				      overwriteWithSubsequentPattern(otherPreviouslyGrownPatternIt, --selectedPatternIt, --selectedPatternEnd);
				      if (selectedPatternEnd != --patternEnd)
					{
					  *selectedPatternEnd = std::move(*patternEnd);
					}
				    }
				  else
				    {
				      ++otherPreviouslyGrownPatternIt;
				    }
				}
			    }
			}
		    }
		}
	      else
		{
		  if (result == alreadyFound)
		    {
		      overwriteWithSubsequentPattern(selectedPatternIt, --selectedPatternEnd, --patternEnd);
		    }
		  else
		    {
		      // Because *selectedPatternIt has not changed, it cannot be a super-pattern of any previously selected pattern
		      // iter_swap(selectedPatternIt, --selectedPatternEnd), to consider *selectedPatternIt as unselected would work if eraseSubPatternsAndSetNextPatternsToGrow would consider the case selectedPatternEnd == patterns.begin(); doing so avoid testing whether all patterns after selectedPatternIt (grown or not) are sub-patterns of *selectedPatternIt, what is impossible since *selectedPatternIt has not changed... but result == infinitelyGrowing does not usually happen (i.e., when nbOfGrownPatterns is as high as possible): adding the test looks worthless
		      if (selectedPatternIt->subPatternOf(patterns.begin(), selectedPatternIt))
			{
			  overwriteWithSubsequentPattern(selectedPatternIt, --selectedPatternEnd, --patternEnd);
			}
		      else
			{
			  ++selectedPatternIt;
			}
		      isPatternsFinitelyGrowing = false;
		    }
		}
	    }
	  patterns.erase(patternEnd, patterns.end());
	}
      if (patterns.size() > nbOfGrownPatterns && nbOfGrownPatterns != 0)
	{
	  throw UsageException(("Cannot get fewer than " + lexical_cast<string>(patterns.size()) + " patterns!").c_str());
	}
      if (isVerbose)
	{
	  cout << "\rFurther growing patterns and discarding sub-patterns: " << patterns.size() << " patterns remaining.\n";
	}
    }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
  cout << "Further growing time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
#endif
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
  cout << '\t' << patterns.size();
#else
  cout << "Nb of grown patterns: " << patterns.size() << '\n';
#endif
#endif
}

void Pattern::outputPatterns()
{
  for (const Pattern& pattern : patterns)
    {
      output(pattern.nSet, pattern.membershipSum);
    }
}

unsigned int Pattern::setPatternsAsCandidateVariables()
{
  const unsigned int nbOfPatterns = patterns.size();
  AbstractRoughTensor::reserveAdditionalCandidateVariables(nbOfPatterns);
  for (Pattern& pattern : patterns)
    {
      AbstractRoughTensor::moveAsCandidateVariable(pattern.nSet);
    }
  patterns.clear();
  patterns.shrink_to_fit();
  tensor.clearAndFree();
  return nbOfPatterns;
}

vector<pair<vector<vector<unsigned int>>, long long>> Pattern::getPatterns()
{
  vector<pair<vector<vector<unsigned int>>, long long>> patternsToAgglomerate;
  patternsToAgglomerate.reserve(patterns.size());
  for (Pattern& pattern : patterns)
    {
      patternsToAgglomerate.emplace_back(std::move(pattern.nSet), pattern.membershipSum);
    }
  patterns.clear();
  patterns.shrink_to_fit();
  return patternsToAgglomerate;
}

Trie& Pattern::getTensor()
{
  return tensor;
}

void Pattern::insertAndEraseSubPatterns(Pattern& pattern)
{
  InclusionResult inclusionResult = incomparable;
  vector<Pattern>::iterator grownMaximalPatternEnd = patterns.end();
  vector<Pattern>::iterator grownPatternIt = patterns.begin();
  for (; grownPatternIt != grownMaximalPatternEnd; ++grownPatternIt)
    {
      inclusionResult = pattern.subOrSuperPatternOf(*grownPatternIt);
      if (inclusionResult != incomparable)
	{
	  break;
	}
    }
  if (inclusionResult == incomparable)
    {
      patterns.emplace_back(std::move(pattern));
      return;
    }
  if (inclusionResult == super)
    {
      *grownPatternIt = std::move(pattern);
      for (vector<Pattern>::iterator otherGrownPatternIt = grownPatternIt + 1; otherGrownPatternIt != grownMaximalPatternEnd; )
	{
	  if (otherGrownPatternIt->subPatternOf(*grownPatternIt))
	    {
	      *otherGrownPatternIt = std::move(*--grownMaximalPatternEnd);
	    }
	  else
	    {
	      ++otherGrownPatternIt;
	    }
	}
      patterns.erase(grownMaximalPatternEnd, patterns.end());
    }
}

void Pattern::overwriteWithSubsequentPattern(const vector<Pattern>::iterator patternToOverwriteIt, const vector<Pattern>::iterator subsequentPatternIt, const vector<Pattern>::iterator end)
{
  *patternToOverwriteIt = std::move(*subsequentPatternIt);
  if (subsequentPatternIt != end)
    {
      *subsequentPatternIt = std::move(*end);
    }
}

vector<Pattern>::iterator Pattern::eraseSubPatternsAndSetNextPatternsToGrow(const vector<Pattern>::iterator grownPatternEnd)
{
  // Grown patterns that are sub-patterns of other grown patterns have been discarded earlier
  vector<Pattern>::iterator patternBegin = patterns.begin();
  vector<Pattern>::iterator patternEnd = patterns.end();
  // Searching the first non-grown pattern that is not a sub-pattern of any grow pattern
  while (patternEnd != grownPatternEnd && grownPatternEnd->subPatternOf(patternBegin, grownPatternEnd))
    {
      *grownPatternEnd = std::move(*--patternEnd);
    }
  vector<Pattern>::iterator minRssDiffBegin;
  vector<Pattern>::iterator minRssDiffEnd;
  if (patternEnd == grownPatternEnd)
    {
      // Only grown patterns remain or there are only non-grown patterns: searching those with the minimal rssDiff
      minRssDiffBegin = patternBegin;
      minRssDiffEnd = patternBegin + 1;
      for (vector<Pattern>::iterator patternIt = minRssDiffEnd; patternIt != patternEnd; ++patternIt)
	{
	  updateMinRssDiff(patternIt, minRssDiffBegin, minRssDiffEnd);
	}
    }
  else
    {
      // Some non-grown patterns remain: searching those with the minimal rssDiff
      minRssDiffBegin = grownPatternEnd;
      minRssDiffEnd = grownPatternEnd + 1;
      for (vector<Pattern>::iterator patternIt = minRssDiffEnd; patternIt != patternEnd; )
	{
	  if (patternIt->subPatternOf(patternBegin, grownPatternEnd))
	    {
	      // *patternIt is a sub-pattern of some grown pattern
	      *patternIt = std::move(*--patternEnd);
	    }
	  else
	    {
	      // *patternIt is not a sub-pattern of some grown pattern
	      updateMinRssDiff(patternIt++, minRssDiffBegin, minRssDiffEnd);
	    }
	}
      // Searching the grown patterns with the minimal rssDiff
      vector<Pattern>::iterator minRssDiffBeginAmongGrown = patternBegin;
      vector<Pattern>::iterator minRssDiffEndAmongGrown = patternBegin + 1;
      for (vector<Pattern>::iterator grownPatternIt = minRssDiffEndAmongGrown; grownPatternIt != grownPatternEnd; ++grownPatternIt)
	{
	  updateMinRssDiff(grownPatternIt, minRssDiffBeginAmongGrown, minRssDiffEndAmongGrown);
	}
      if (minRssDiffBeginAmongGrown->rssDiff <= minRssDiffBegin->rssDiff)
	{
	  if (minRssDiffBeginAmongGrown->rssDiff == minRssDiffBegin->rssDiff)
	    {
	      do
		{
		  iter_swap(--minRssDiffEnd, minRssDiffEndAmongGrown++);
		} while (minRssDiffEnd != minRssDiffBegin);
	    }
	  minRssDiffBegin = minRssDiffBeginAmongGrown;
	  minRssDiffEnd = minRssDiffEndAmongGrown;
	}
    }
  patterns.erase(patternEnd, patterns.end());
  do
    {
      iter_swap(--minRssDiffEnd, patternBegin++);
    }
  while (minRssDiffEnd != minRssDiffBegin);
  return patternBegin;
}

void Pattern::updateMinRssDiff(const vector<Pattern>::iterator patternIt, vector<Pattern>::iterator& minRssDiffBegin, vector<Pattern>::iterator& minRssDiffEnd)
{
  if (patternIt->rssDiff <= minRssDiffBegin->rssDiff)
    {
      if (patternIt->rssDiff == minRssDiffBegin->rssDiff)
	{
	  iter_swap(patternIt, minRssDiffEnd);
	}
      else
	{
	  minRssDiffBegin = patternIt;
	  minRssDiffEnd = patternIt;
	}
      ++minRssDiffEnd;
    }
}

void Pattern::outputIntermediaryPatterns(vector<pair<vector<vector<unsigned int>>, long long>>& intermediaryPatterns)
{
  if (isIntermediaryPatternsOutput)
    {
      if (AbstractRoughTensor::isDirectOutput())
	{
	  for (const pair<vector<vector<unsigned int>>, long long>& intermediaryPattern : intermediaryPatterns)
	    {
	      output(intermediaryPattern.first, intermediaryPattern.second);
	    }
	}
      else
	{
	  for (pair<vector<vector<unsigned int>>, long long>& intermediaryPattern : intermediaryPatterns)
	    {
	      AbstractRoughTensor::moveAsCandidateVariable(intermediaryPattern.first);
	    }
	}
    }
}
