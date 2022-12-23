// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "DendrogramNode.h"

const AbstractRoughTensor* DendrogramNode::roughTensor;
Trie DendrogramNode::tensor;
unsigned int DendrogramNode::maxId;
vector<DendrogramNode> DendrogramNode::dendrogramFrontier;
vector<CandidateNode> DendrogramNode::candidates;

DendrogramNode::DendrogramNode(const DendrogramNode& otherDendrogramNode): id(otherDendrogramNode.id), nSet(otherDendrogramNode.nSet), area(otherDendrogramNode.area), membershipSum(otherDendrogramNode.membershipSum)
{
}

DendrogramNode::DendrogramNode(DendrogramNode&& otherDendrogramNode): id(otherDendrogramNode.id), nSet(std::move(otherDendrogramNode.nSet)), area(otherDendrogramNode.area), membershipSum(otherDendrogramNode.membershipSum)
{
}

DendrogramNode::DendrogramNode(const vector<vector<unsigned int>>& nSetParam, const long long membershipSumParam): id(maxId++), nSet(nSetParam), area(1), membershipSum(membershipSumParam)
{
  for (const vector<unsigned int>& dimension : nSet)
    {
      area *= dimension.size();
    }
}

DendrogramNode::DendrogramNode(): id(maxId++), nSet(), area(1), membershipSum()
{
  // Move the two children at the end of dendrogramFrontier
  vector<unsigned int> childrenIds = candidates.back().getChildrenIds();
#ifdef DEBUG_AMALGAMATE
  cout << "Inserting pattern " << id << " in the dendrogram, union of patterns " << childrenIds.front() << " and " << childrenIds.back() << " (quadratic error variation = " << static_cast<float>(candidates.back().getQuadraticErrorVariation()) / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << ")\n";
#endif
  candidates.pop_back();
  vector<DendrogramNode>::iterator dendrogramFrontierEnd = dendrogramFrontier.end();
  vector<DendrogramNode>::iterator newDendrogramFrontierEnd = dendrogramFrontierEnd - 2;
  for (vector<DendrogramNode>::iterator nodeInFrontierIt = dendrogramFrontier.begin(); dendrogramFrontierEnd != newDendrogramFrontierEnd; )
    {
      if (nodeInFrontierIt->id == childrenIds.front() || nodeInFrontierIt->id == childrenIds.back())
	{
	  nodeInFrontierIt->id = (--dendrogramFrontierEnd)->id;
	  nodeInFrontierIt->nSet.swap(dendrogramFrontierEnd->nSet);
	  // the two swaps below could be affectations if no direct output
	  swap(nodeInFrontierIt->area, dendrogramFrontierEnd->area);
	  swap(nodeInFrontierIt->membershipSum, dendrogramFrontierEnd->membershipSum);
	}
      else
	{
	  ++nodeInFrontierIt;
	}
    }
  // Compute nSet
  nSet.reserve(dendrogramFrontierEnd->nSet.size());
  vector<vector<unsigned int>>::const_iterator child1NSetIt = dendrogramFrontierEnd->nSet.begin();
  ++dendrogramFrontierEnd;
  for (const vector<unsigned int>& child2NSetDimension : dendrogramFrontierEnd->nSet)
    {
      vector<unsigned int> unionNSetDimension;
      unionNSetDimension.reserve(child2NSetDimension.size() + child1NSetIt->size());
      set_union(child2NSetDimension.begin(), child2NSetDimension.end(), child1NSetIt->begin(), child1NSetIt->end(), back_inserter(unionNSetDimension));
      nSet.emplace_back(std::move(unionNSetDimension));
      ++child1NSetIt;
    }
  if (AbstractRoughTensor::isDirectOutput())
    {
      // Output the two children
      dendrogramFrontierEnd->output();
      (--dendrogramFrontierEnd)->output();
    }
  else
    {
      // Turn the two children into candidate variables
      AbstractRoughTensor::moveAsCandidateVariable(dendrogramFrontierEnd->nSet);
      AbstractRoughTensor::moveAsCandidateVariable((--dendrogramFrontierEnd)->nSet);
    }
  // Compute membershipSum
  if (Trie::is01)
    {
      membershipSum = SparseCrispTube::getDefaultMembership();
      for (const vector<unsigned int>& dimension : nSet)
	{
	  membershipSum *= dimension.size();
	}
      membershipSum += tensor.nbOfPresentTuples(nSet) * AbstractRoughTensor::getUnit();
    }
  else
    {
      membershipSum = tensor.membershipSum(nSet);
    }
  // Maximize density by extension
  for (vector<vector<int>> extensionSums = tensor.sumsOnExtensions(nSet.begin(), AbstractRoughTensor::getCardinalities()); ; )
    {
      const vector<vector<int>>::iterator extensionSumEnd = extensionSums.end();
      vector<vector<int>>::iterator extensionSumsInNextElementDimensionIt;
      long long bestAdjustedNextMembershipSum = membershipSum;
      vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
      vector<int>::const_iterator extensionIt;
      if (Trie::is01)
	{
	  int membershipSumOnNextElement = 0;
	  long long onlyNoise = SparseCrispTube::getDefaultMembership();
	  for (const vector<unsigned int>& dimension : nSet)
	    {
	      onlyNoise *= dimension.size();
	    }
	  for (vector<vector<int>>::iterator extensionSumsInDimensionIt = extensionSums.begin(); extensionSumsInDimensionIt != extensionSumEnd; ++extensionSumsInDimensionIt)
	    {
	      vector<int>::const_iterator extensionInDimensionIt = Trie::maxExtension(*extensionSumsInDimensionIt);
	      // Advance to the next dimension where some element is absent (*extensionInDimensionIt != numeric_limits<int>::min(), otherwise defining membershipSumOnExtension would underflow)
	      for (; *extensionInDimensionIt == numeric_limits<int>::min(); ++dimensionIt)
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
	      int sizeOfNextElementDimension = dimensionIt->size();
	      const int membershipSumOnExtension = onlyNoise / sizeOfNextElementDimension + *extensionInDimensionIt * AbstractRoughTensor::getUnit();
	      long long adjustedNextMembershipSum = (membershipSum + membershipSumOnExtension) * sizeOfNextElementDimension;
	      ++sizeOfNextElementDimension;
	      adjustedNextMembershipSum = adjustedNextMembershipSum / sizeOfNextElementDimension + adjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
	      if (adjustedNextMembershipSum >= bestAdjustedNextMembershipSum)
		{
		  bestAdjustedNextMembershipSum = adjustedNextMembershipSum;
		  membershipSumOnNextElement = membershipSumOnExtension;
		  extensionSumsInNextElementDimensionIt = extensionSumsInDimensionIt;
		  extensionIt = extensionInDimensionIt;
		}
	      ++dimensionIt;
	    }
	  if (membershipSumOnNextElement == 0)
	    {
	      break;
	    }
#ifdef DEBUG_AMALGAMATE
	  printExtendDebug(extensionSumsInNextElementDimensionIt - extensionSums.begin(), extensionIt - extensionSumsInNextElementDimensionIt->begin(), membershipSumOnNextElement);
#endif
	  membershipSum += membershipSumOnNextElement;
	}
      else
	{
	  extensionSumsInNextElementDimensionIt = extensionSumEnd;
	  for (vector<vector<int>>::iterator extensionSumsInDimensionIt = extensionSums.begin(); extensionSumsInDimensionIt != extensionSumEnd; ++extensionSumsInDimensionIt)
	    {
	      const vector<int>::const_iterator extensionInDimensionIt = Trie::maxExtension(*extensionSumsInDimensionIt);
	      int sizeOfNextElementDimension = dimensionIt->size();
	      long long adjustedNextMembershipSum = (membershipSum + *extensionInDimensionIt) * sizeOfNextElementDimension;
	      ++sizeOfNextElementDimension;
	      adjustedNextMembershipSum = adjustedNextMembershipSum / sizeOfNextElementDimension + adjustedNextMembershipSum % sizeOfNextElementDimension; // add the rest of the integer division to avoid that round-off errors make the adjusted membership sum look lesser than the membership sum when they actually are equal (what may often happens for small patterns in 0/1 tensors)
	      if (adjustedNextMembershipSum >= bestAdjustedNextMembershipSum)
		{
		  bestAdjustedNextMembershipSum = adjustedNextMembershipSum;
		  extensionSumsInNextElementDimensionIt = extensionSumsInDimensionIt;
		  extensionIt = extensionInDimensionIt;
		}
	      ++dimensionIt;
	    }
	  if (extensionSumsInNextElementDimensionIt == extensionSumEnd)
	    {
	      break;
	    }
#ifdef DEBUG_AMALGAMATE
	  printExtendDebug(extensionSumsInNextElementDimensionIt - extensionSums.begin(), extensionIt - extensionSumsInNextElementDimensionIt->begin(), *extensionIt);
#endif
	  membershipSum += *extensionIt;
	}
      vector<unsigned int>& dimension = nSet[extensionSumsInNextElementDimensionIt - extensionSums.begin()];
      const vector<unsigned int> dimensionBeforeExtension(std::move(dimension));
      const unsigned int elementId = extensionIt - extensionSumsInNextElementDimensionIt->begin();
      dimension = {elementId};
      tensor.updateSumsOnExtensionsAfterExtension(nSet.begin(), extensionSums.begin(), extensionSumsInNextElementDimensionIt);
      dimension = std::move(dimensionBeforeExtension);
      dimension.insert(lower_bound(dimension.begin(), dimension.end(), elementId), elementId);
      const_cast<int&>(*extensionIt) = numeric_limits<int>::min();
#ifdef DEBUG_AMALGAMATE
      float currentDensity = static_cast<float>(membershipSum) / AbstractRoughTensor::getUnit();
      for (const vector<unsigned int>& dimension : nSet)
	{
	  currentDensity /= dimension.size();
	}
      cout << currentDensity << '\n';
#endif
    }
  // Compute area
  for (const vector<unsigned int>& dimension : nSet)
    {
      area *= dimension.size();
    }
  // Move the end of dendrogramFrontier on the subsets of *this, construct the new candidates and find the one with the smallest quadratic error variation
  const vector<CandidateNode>::iterator newCandidatesBegin = candidates.end();
  vector<CandidateNode>::iterator bestCandidateIt;
  long long minD = numeric_limits<long long>::max();
  for (vector<DendrogramNode>::iterator otherChildIt = dendrogramFrontier.begin(); otherChildIt != newDendrogramFrontierEnd; )
    {
      const long long candidateDensity = unionDensityIfNotSupersetOf(*otherChildIt);
      if (candidateDensity == numeric_limits<long long>::min())
	{
	  childrenIds.push_back(otherChildIt->id);
	  if (AbstractRoughTensor::isDirectOutput())
	    {
	      otherChildIt->output();
	    }
	  else
	    {
	      AbstractRoughTensor::moveAsCandidateVariable(otherChildIt->nSet);
	    }
	  *otherChildIt = std::move(*--newDendrogramFrontierEnd);
	}
      else
	{
	  const long long d = quadraticErrorVariation(*otherChildIt, candidateDensity);
	  candidates.emplace_back(otherChildIt->id, id, d);
	  if (d < minD)
	    {
	      minD = d;
	      bestCandidateIt = --candidates.end();
	    }
	  ++otherChildIt;
	}
    }
#ifdef DEBUG_AMALGAMATE
  cout << "Pattern " << id << ": " << *this << "\n  " << childrenIds.size() << " children:";
  for (const unsigned int id : childrenIds)
    {
      cout << ' ' << id;
    }
  cout << "\n  " << candidates.end() - newCandidatesBegin << " new candidates\n";
#endif
  dendrogramFrontier.erase(newDendrogramFrontierEnd, dendrogramFrontier.end());
  if (!dendrogramFrontier.empty())
    {
      // this is not the root of the dendrogram: find the old candidates that are no longer candidates and the one with the smallest quadratic error, if smaller than the one found among new candidates
      vector<CandidateNode>::iterator oldCandidatesEnd = newCandidatesBegin;
      for (vector<CandidateNode>::iterator oldCandidateIt = candidates.begin(); oldCandidateIt != oldCandidatesEnd; )
	{
	  if (oldCandidateIt->valid(childrenIds))
	    {
	      if (oldCandidateIt->getQuadraticErrorVariation() < minD)
		{
		  minD = oldCandidateIt->getQuadraticErrorVariation();
		  bestCandidateIt = oldCandidateIt;
		}
	      ++oldCandidateIt;
	    }
	  else
	    {
	      *oldCandidateIt = *--oldCandidatesEnd;
	    }
	}
      swap(*bestCandidateIt, candidates.back());
      candidates.erase(oldCandidatesEnd, newCandidatesBegin);
    }
}

DendrogramNode& DendrogramNode::operator=(DendrogramNode&& otherDendrogramNode)
{
  id = otherDendrogramNode.id;
  nSet = std::move(otherDendrogramNode.nSet);
  area = otherDendrogramNode.area;
  membershipSum = otherDendrogramNode.membershipSum;
  return *this;
}

ostream& operator<<(ostream& out, const DendrogramNode& pattern)
{
  DendrogramNode::roughTensor->printPattern(pattern.nSet, static_cast<float>(pattern.membershipSum) / pattern.area, out);
  return out;
}

pair<vector<vector<unsigned int>>, unsigned int> DendrogramNode::unionAndArea(const DendrogramNode& otherDendrogramNode) const
{
  vector<vector<unsigned int>> unionNSet;
  unionNSet.reserve(nSet.size());
  unsigned int unionArea = 1;
  vector<vector<unsigned int>>::const_iterator otherNSetIt = otherDendrogramNode.nSet.begin();
  for (const vector<unsigned int>& nSetDimension : nSet)
    {
      vector<unsigned int> unionNSetDimension;
      unionNSetDimension.reserve(nSetDimension.size() + otherNSetIt->size());
      set_union(nSetDimension.begin(), nSetDimension.end(), otherNSetIt->begin(), otherNSetIt->end(), back_inserter(unionNSetDimension));
      unionArea *= unionNSetDimension.size();
      unionNSet.emplace_back(unionNSetDimension);
      ++otherNSetIt;
    }
  return {unionNSet, unionArea};
}

long long DendrogramNode::unionDensity(const DendrogramNode& otherDendrogramNode) const
{
  const pair<vector<vector<unsigned int>>, unsigned int> candidateAndArea = unionAndArea(otherDendrogramNode);
  if (Trie::is01)
    {
      return SparseCrispTube::getDefaultMembership() + tensor.nbOfPresentTuples(candidateAndArea.first) * AbstractRoughTensor::getUnit() / candidateAndArea.second;
    }
  return tensor.membershipSum(candidateAndArea.first) / candidateAndArea.second;
}

long long DendrogramNode::unionDensityIfNotSupersetOf(const DendrogramNode& otherDendrogramNode) const
{
  const pair<vector<vector<unsigned int>>, unsigned int> candidateAndArea = unionAndArea(otherDendrogramNode);
  if (candidateAndArea.second == area)
    {
      // otherDendrogramNode is a sub-pattern of *this
      return numeric_limits<long long>::min();		// cannot return 0, which is a valid density
    }
  if (Trie::is01)
    {
      return SparseCrispTube::getDefaultMembership() + tensor.nbOfPresentTuples(candidateAndArea.first) * AbstractRoughTensor::getUnit() / candidateAndArea.second;
    }
  return tensor.membershipSum(candidateAndArea.first) / candidateAndArea.second;
}

pair<long long, unsigned int> DendrogramNode::intersectionMembershipSumAndArea(const DendrogramNode& otherDendrogramNode) const
{
  vector<vector<unsigned int>> intersection;
  intersection.reserve(nSet.size());
  unsigned int intersectionArea = 1;
  vector<vector<unsigned int>>::const_iterator otherNSetIt = otherDendrogramNode.nSet.begin();
  for (const vector<unsigned int>& nSetDimension : nSet)
    {
      vector<unsigned int> intersectionDimension;
      intersectionDimension.reserve(nSetDimension.size());
      set_intersection(nSetDimension.begin(), nSetDimension.end(), otherNSetIt->begin(), otherNSetIt->end(), back_inserter(intersectionDimension));
      if (intersectionDimension.empty())
	{
	  return {0, 0};
	}
      intersectionArea *= intersectionDimension.size();
      intersection.emplace_back(intersectionDimension);
      ++otherNSetIt;
    }
  if (Trie::is01)
    {
      return {SparseCrispTube::getDefaultMembership() * intersectionArea + tensor.nbOfPresentTuples(intersection) * AbstractRoughTensor::getUnit(), intersectionArea};
    }
  return {tensor.membershipSum(intersection), intersectionArea};
}

long long DendrogramNode::quadraticErrorVariation(const DendrogramNode& otherChild, const long long candidateDensity) const
{
  const long long thisDensity = membershipSum / area;
  const long long otherChildDensity = otherChild.membershipSum / otherChild.area;
  if (thisDensity <= candidateDensity)
    {
      if (otherChildDensity <= candidateDensity)
	{
	  return 0;
	}
      const long long densityDiff2 = otherChildDensity - candidateDensity;
      return densityDiff2 * densityDiff2 * otherChild.area;
    }
  const long long densityDiff1 = thisDensity - candidateDensity;
  if (otherChildDensity <= candidateDensity)
    {
      return densityDiff1 * densityDiff1 * area;
    }
  const pair<long long, unsigned int> interMembershipSumAndArea = intersectionMembershipSumAndArea(otherChild);
  const long long densityDiff2 = otherChildDensity - candidateDensity;
  if (thisDensity < otherChildDensity)
    {
      // cout << static_cast<float>(densityDiff1) / AbstractRoughTensor::getUnit() << "² * " << area << " + " << static_cast<float>(densityDiff2) / AbstractRoughTensor::getUnit() << "² * " << otherChild.area << " + (" << static_cast<float>(thisDensity) / AbstractRoughTensor::getUnit() << "² - " << static_cast<float>(candidateDensity) / AbstractRoughTensor::getUnit() << "²) * " << interMembershipSumAndArea.second << " - 2 * " << static_cast<float>(densityDiff1) / AbstractRoughTensor::getUnit() << " * " << static_cast<float>(interMembershipSumAndArea.first) / AbstractRoughTensor::getUnit() << '\n';
      return densityDiff1 * densityDiff1 * area + densityDiff2 * densityDiff2 * otherChild.area + (thisDensity * thisDensity - candidateDensity * candidateDensity) * interMembershipSumAndArea.second - 2 * densityDiff1 * interMembershipSumAndArea.first;
    }
  // cout << static_cast<float>(densityDiff1) / AbstractRoughTensor::getUnit() << "² * " << area << " + " << static_cast<float>(densityDiff2) / AbstractRoughTensor::getUnit() << "² * " << otherChild.area << " + (" << static_cast<float>(otherChildDensity) / AbstractRoughTensor::getUnit() << "² - " << static_cast<float>(candidateDensity) / AbstractRoughTensor::getUnit() << "²) * " << interMembershipSumAndArea.second << " - 2 * " << static_cast<float>(densityDiff2) / AbstractRoughTensor::getUnit() << " * " << static_cast<float>(interMembershipSumAndArea.first) / AbstractRoughTensor::getUnit() << '\n';
  return densityDiff1 * densityDiff1 * area + densityDiff2 * densityDiff2 * otherChild.area + (otherChildDensity * otherChildDensity - candidateDensity * candidateDensity) * interMembershipSumAndArea.second - 2 * densityDiff2 * interMembershipSumAndArea.first;
}

void DendrogramNode::output() const
{
  roughTensor->output(nSet, static_cast<float>(membershipSum) / area);
}

void DendrogramNode::setLeavesAndTensor(const vector<pair<vector<vector<unsigned int>>, long long>>& leaves, const AbstractRoughTensor* roughTensorParam, Trie& tensorParam)
{
  roughTensor = roughTensorParam;
  tensor = std::move(tensorParam);
  dendrogramFrontier.reserve(leaves.size());
  for (const pair<vector<vector<unsigned int>>, long long>& leaf : leaves)
    {
      dendrogramFrontier.emplace_back(leaf.first, leaf.second);
    }
}

void DendrogramNode::amalgamate(const bool isVerbose)
{
#ifdef DETAILED_TIME
  steady_clock::time_point startingPoint = steady_clock::now();
#endif
  if (dendrogramFrontier.size() != 1)
    {
      if (!AbstractRoughTensor::isDirectOutput())
	{
	  AbstractRoughTensor::reserveAdditionalCandidateVariables(2 * dendrogramFrontier.size() - 1);
	}
#ifdef DEBUG_AMALGAMATE
      cout << "* " << dendrogramFrontier.size() << " leaves\n";
#endif
      unsigned long long nbOfCandidates = static_cast<unsigned long long>(dendrogramFrontier.size()) * (dendrogramFrontier.size() - 1) / 2;
      if (isVerbose)
	{
	  CompletionMessage<unsigned long long>::set("Initializing pattern amalgamation", nbOfCandidates);
	}
      // Construct the initial candidates and find the one with the smallest quadratic error variation
      vector<CandidateNode>::iterator bestCandidateIt;
      long long minD = numeric_limits<long long>::max();
      candidates.reserve(nbOfCandidates + dendrogramFrontier.size() - 2); // enough space for all candidates plus the first new candidates with the first node above the leaves as a child
      const vector<DendrogramNode>::const_iterator begin = dendrogramFrontier.begin();
      const vector<DendrogramNode>::const_iterator end = dendrogramFrontier.end();
      for (vector<DendrogramNode>::const_iterator child2It = begin; child2It != end; ++child2It)
	{
#ifdef DEBUG_AMALGAMATE
	  cout << "pattern " << child2It->id << ": " << *child2It << '\n';
#endif
	  if (isVerbose)
	    {
	      nbOfCandidates -= child2It - begin;
	      CompletionMessage<unsigned long long>::print(nbOfCandidates);
	    }
	  for (vector<DendrogramNode>::const_iterator child1It = begin; child1It != child2It; ++child1It)
	    {
	      const long long d = child2It->quadraticErrorVariation(*child1It, child2It->unionDensity(*child1It));
	      candidates.emplace_back(child1It->id, child2It->id, d);
	      if (d < minD)
		{
		  minD = d;
		  bestCandidateIt = --candidates.end();
		}
	    }
	}
      swap(*bestCandidateIt, candidates.back());
      if (isVerbose)
	{
	  CompletionMessage<unsigned long long>::done();
	}
#ifdef DETAILED_TIME
#ifdef GNUPLOT
      cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
      cout << "Amalgamation initialization time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
      startingPoint = steady_clock::now();
#endif
      // Hierarchical amalgamation
#ifdef DEBUG_AMALGAMATE
      cout << "\n* Amalgamates:\n";
#endif
      if (isVerbose)
	{
	  CompletionMessage<unsigned long long>::set("Amalgamating grown patterns", static_cast<unsigned long long>(candidates.size()) * (dendrogramFrontier.size() + 1)); // three times the worst possible number of accesses to candidates; adding the worst possible number of accesses to nodes in the frontier gives "+ 4" instead of "+ 1", but the increase of the percentage then slows down
	  while (dendrogramFrontier.size() != 1)
	    {
	      // IMPROVE?: select all candidates (they may share some child...) with the same smallest quadraticErrorVariation: even if some selected candidate is a subset of another selected one, maximizing its density may make it grow into a pattern that is not a sub-pattern.  That would mean separately storing all the patterns that will enter the dendrogram, which would not form a tree anymore, eliminating repeated patterns, inserting them them in increasing order of area (some may be sub-pattern of others, hence not in the frontier) and finally building the candidates from those in the frontier.  Problem: four (or more) patterns in the frontier, all pairwise equidistant, may generate up to six (or more) patterns, which may all end up in the frontier.
	      dendrogramFrontier.push_back(DendrogramNode());
	      CompletionMessage<unsigned long long>::print(static_cast<unsigned long long>(candidates.size()) * (dendrogramFrontier.size() + 1));
	    }
	  CompletionMessage<unsigned long long>::done();
	}
      else
	{
	  while (dendrogramFrontier.size() != 1)
	    {
	      dendrogramFrontier.push_back(DendrogramNode());
	    }
	}
      candidates.shrink_to_fit();
    }
  tensor.clearAndFree();
  if (AbstractRoughTensor::isDirectOutput())
    {
      dendrogramFrontier.front().output();
    }
  else
    {
      AbstractRoughTensor::moveAsCandidateVariable(dendrogramFrontier.front().nSet);
    }
  dendrogramFrontier.clear();
  dendrogramFrontier.shrink_to_fit();
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
  cout << "Amalgamation time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
  startingPoint = steady_clock::now();
#endif
}

#ifdef DEBUG_AMALGAMATE
void DendrogramNode::printExtendDebug(const unsigned int dimensionId, const unsigned int elementId, const float membershipSumOnExtension) const
{
  // area is 1; it only computed once the density maximized, but operator<< needs it here
  for (const vector<unsigned int>& dimension : nSet)
    {
      const_cast<unsigned int&>(area) *= dimension.size();
    }
  cout << "Extending " << *this << " with ";
  AbstractRoughTensor::printElement(dimensionId, elementId, cout);
  cout << " and shifted density " << static_cast<float>(membershipSumOnExtension) * nSet[dimensionId].size() / area / AbstractRoughTensor::getUnit() << ": shifted density increases from " << static_cast<float>(membershipSum) / area / AbstractRoughTensor::getUnit() << " to ";
  const_cast<unsigned int&>(area) = 1;
}
#endif
