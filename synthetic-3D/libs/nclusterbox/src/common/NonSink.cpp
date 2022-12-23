// Copyright 2019,2020 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "NonSink.h"

TrieWithPrediction NonSink::tensor;
long long NonSink::nbOfTuples;
double NonSink::quadraticError;
long long NonSink::minQuadraticErrorVariation;
long long NonSink::quadraticErrorVariationKeepingAICcConstant;
multimap<long long, CandidateVariable> NonSink::candidates;
multimap<long long, pair<long long, CandidateVariable>> NonSink::updated;

NonSink::NonSink(Vertex& sink): Vertex(std::move(sink)), children(1, model.size() - 1), tensorOfSubmodel(tensor.subtensor(nSet, density)), nbOfParametersOfSubmodel(2)
{
  quadraticErrorOfSubmodel -= getQuadraticErrorDecreaseOfNullSubmodel(model.back()->getNSet(), model.back()->getDensity());
  tensorOfSubmodel.addFirstPatternToModel(nSetInSubmodel(model.back()->getNSet()), model.back()->getDensity());
}

NonSink::NonSink(vector<vector<unsigned int>>& nSetParam, const int densityParam, const long long quadraticErrorOfSubmodelParam): Vertex(nSetParam, densityParam, quadraticErrorOfSubmodelParam), children(), tensorOfSubmodel(), nbOfParametersOfSubmodel(1)
{
}

bool NonSink::sink() const
{
  return false;
}

const vector<unsigned int>& NonSink::getChildren() const
{
  return children;
}

void NonSink::visit(dynamic_bitset<>& descendants) const
{
  for (const unsigned int childId : children)
    {
      if (!descendants[childId])
	{
	  descendants.set(childId);
	  model[childId]->visit(descendants);
	}
    }
}

void NonSink::addDescendant()
{
  quadraticErrorOfSubmodel += tensorOfSubmodel.addPatternToModelAndGetQuadraticErrorVariation(nSetInSubmodel(model.back()->getNSet()), model.back()->getDensity());
  ++nbOfParametersOfSubmodel;
}

void NonSink::addChild()
{
  addDescendant();
  children.push_back(model.size() - 1);
}

void NonSink::addChildAndRemoveGrandChildren()
{
  vector<unsigned int> stillChildren;
  set_difference(children.begin(), children.end(), static_cast<NonSink*>(model.back())->children.begin(), static_cast<NonSink*>(model.back())->children.end(), back_inserter(stillChildren));
  children = std::move(stillChildren);
  addChild();
}

Confirmation NonSink::confirm(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity, dynamic_bitset<>& visitedVertices) const
{
  const unsigned int area = getArea();
  if (tensorOfSubmodel.quadraticErrorVariation(nSetInSubmodel(candidateNSet), candidateDensity) >= quadraticErrorOfSubmodel * (exp(2 * (1. - area) / (area - nbOfParametersOfSubmodel - 1) / (area - nbOfParametersOfSubmodel - 2)) - 1))
    {
      return Confirmation();
    }
  const vector<unsigned int>::const_iterator end = children.end();
  vector<unsigned int>::const_iterator childIdIt = children.begin();
  InclusionResult result = model[*childIdIt]->subOrSuperPatternOf(candidateNSet);
  while (result == incomparable)
    {
      visitedVertices.set(*childIdIt);
      if (++childIdIt == end)
	{
	  break;
	}
      // subOrSuperPatternOf called even if visitedVertices[*childIdIt], to know whether *this is a parent or an ancestor
      result = model[*childIdIt]->subOrSuperPatternOf(candidateNSet);
    }
  if (result == incomparable)
    {
      return Confirmation(id);
    }
  vector<unsigned int> childrenOfCandidate;
  if (result == sub)
    {
      if (!visitedVertices[*childIdIt])
	{
	  visitedVertices.set(*childIdIt);
	  childrenOfCandidate.push_back(*childIdIt);
	}
      while (++childIdIt != end)
	{
	  if (!visitedVertices[*childIdIt])
	    {
	      visitedVertices.set(*childIdIt);
	      if (model[*childIdIt]->subPatternOf(candidateNSet))
		{
		  childrenOfCandidate.push_back(*childIdIt);
		}
	    }
	}
      return Confirmation(id, childrenOfCandidate);
    }
  vector<unsigned int> ancestorsOfCandidate;
  vector<unsigned int> parentsOfCandidate;
  Confirmation confirmation;
  if (!visitedVertices[*childIdIt])
    {
      confirmation = model[*childIdIt]->confirm(candidateNSet, candidateDensity, visitedVertices);
      if (confirmation.infirmation())
	{
	  return confirmation;
	}
      ancestorsOfCandidate = std::move(confirmation.getAncestors());
      parentsOfCandidate = std::move(confirmation.getParents());
      childrenOfCandidate = std::move(confirmation.getChildren());
      visitedVertices.set(*childIdIt);
    }
  while (++childIdIt != end)
    {
      if (!visitedVertices[*childIdIt])
	{
	  const Vertex& child = *model[*childIdIt];
	  if (child.superPatternOf(candidateNSet))
	    {
	      confirmation = child.confirm(candidateNSet, candidateDensity, visitedVertices);
	      if (confirmation.infirmation())
		{
		  return confirmation;
		}
	      ancestorsOfCandidate.insert(ancestorsOfCandidate.end(), confirmation.getAncestors().begin(), confirmation.getAncestors().end());
	      parentsOfCandidate.insert(parentsOfCandidate.end(), confirmation.getParents().begin(), confirmation.getParents().end());
	      childrenOfCandidate.insert(childrenOfCandidate.end(), confirmation.getChildren().begin(), confirmation.getChildren().end());
	    }
	  visitedVertices.set(*childIdIt);
	}
    }
  ancestorsOfCandidate.push_back(id);
  return Confirmation(ancestorsOfCandidate, parentsOfCandidate, childrenOfCandidate);
}

bool NonSink::autoConfirm(const vector<unsigned int>& childrenParam)
{
  const unsigned int area = getArea();
  dynamic_bitset<> descendants(model.size());
  for (const unsigned int childId : childrenParam)
    {
      descendants.set(childId);
      model[childId]->visit(descendants);
    }
  dynamic_bitset<>::size_type descendantId = descendants.find_first();
  const Vertex& firstDescendant = *model[descendantId];
  long long deltaRSS = -firstDescendant.getG(density);
  if (deltaRSS >= quadraticErrorOfSubmodel * (exp(2 * (1. - nbOfTuples) / (nbOfTuples - 2) / (nbOfTuples - 3)) - 1))
    {
      // AICc increases
      return false;
    }
  quadraticErrorOfSubmodel += deltaRSS;
  tensorOfSubmodel = tensor.subtensor(nSet, density);
  tensorOfSubmodel.addFirstPatternToModel(nSetInSubmodel(firstDescendant.getNSet()), firstDescendant.getDensity());
  children.push_back(descendantId);
  for (descendantId = descendants.find_next(descendantId); descendantId != dynamic_bitset<>::npos; descendantId = descendants.find_next(descendantId))
    {
      const Vertex& descendant = *model[descendantId];
      deltaRSS = tensorOfSubmodel.addPatternToModelAndGetQuadraticErrorVariation(nSetInSubmodel(descendant.getNSet()), descendant.getDensity());
      if (deltaRSS >= quadraticErrorOfSubmodel * (exp(2 * (1. - area) / (area - nbOfParametersOfSubmodel - 1) / (area - nbOfParametersOfSubmodel - 2)) - 1))
	{
	  // AICc increases
	  return false;
	}
      quadraticErrorOfSubmodel += deltaRSS;
      ++nbOfParametersOfSubmodel;
      children.push_back(descendantId);
    }
  return true;
}

void NonSink::forwardSelect(const unsigned int nbOfCandidateVariablesHavingAllElements, AbstractRoughTensor* roughTensor, const bool isVerbose, const bool isHierarchyPrinted)
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
  nbOfTuples = 1;
  for (const unsigned int cardinality : roughTensor->getCardinalities())
    {
      nbOfTuples *= cardinality;
    }
  if (isVerbose)
    {
      cout << "Reducing fuzzy tensor to elements in patterns ... " << flush;
    }
  tensor = roughTensor->projectTensor(nbOfCandidateVariablesHavingAllElements);
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
  quadraticError = AbstractRoughTensor::getNullModelQuadraticError();
  minQuadraticErrorVariation = quadraticError * (exp(2 * (1. - nbOfTuples) / (nbOfTuples - 2) / (nbOfTuples - 3)) - 1); // a larger quadratic error variation increases AICc (with already one parameter, the shift)
  // Construct the candidates
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
      cout << " decreases the quadratic error, " << quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << ", the most, by " << static_cast<double>(-minQuadraticErrorVariation) * 100 / quadraticError << "% > " << 100 * (1. - exp(2 * (1. - nbOfTuples) / (nbOfTuples - 2) / (nbOfTuples - 3))) << "%, the minimal acceptable\n";
#endif
      quadraticError += minQuadraticErrorVariation;
      model.push_back(new Vertex(bestCandidateIt->second.nSet, bestCandidateIt->second.density, tensor.addFirstPatternToModelAndGetQuadraticErrorOfNullSubmodel(bestCandidateIt->second.nSet, bestCandidateIt->second.density)));
      candidates.erase(bestCandidateIt);
      quadraticErrorVariationKeepingAICcConstant = quadraticError * (exp(2 * (1. - nbOfTuples) / (nbOfTuples - 3) / (nbOfTuples - 4)) - 1);
      minQuadraticErrorVariation = quadraticErrorVariationKeepingAICcConstant;
      vector<unsigned int> sources(1, 0);
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
	  if (updated.empty() || updated.begin()->first >= quadraticErrorVariationKeepingAICcConstant)
	    {
	      break;
	    }
	  CandidateVariable& selected = updated.begin()->second.second;
#ifdef DEBUG_SELECT
	  roughTensor->printPattern(selected.nSet, selected.density, cout);
	  cout << " decreases the quadratic error, " << quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << ", the most, by " << static_cast<double>(-minQuadraticErrorVariation) * 100 / quadraticError << "% > " << 100 * (1. - exp(2 * (1. - nbOfTuples) / (nbOfTuples - model.size() - 2) / (nbOfTuples - model.size() - 3))) << "%, the minimal acceptable, ";
#endif
	  // Confirm or infirm selected
	  const vector<unsigned int>::const_iterator sourceEnd = sources.end();
	  vector<unsigned int>::const_iterator sourceIdIt = sources.begin();
	  InclusionResult result = model[*sourceIdIt]->subOrSuperPatternOf(selected.nSet);
	  while (result == incomparable && ++sourceIdIt != sourceEnd)
	    {
	      result = model[*sourceIdIt]->subOrSuperPatternOf(selected.nSet);
	    }
	  if (result == incomparable)
	    {
#ifdef DEBUG_SELECT
	      cout << "and requires no further confirmation\n";
#endif
	      sources.push_back(model.size());
	      updateStaticVariablesAfterConfirmation(new Vertex(selected.nSet, selected.density, tensor.addPatternToModelAndGetQuadraticErrorOfNullSubmodel(selected.nSet, selected.density)));
	    }
	  else
	    {
	      if (result == super)
		{
		  dynamic_bitset<> visitedVertices(model.size());
		  Confirmation confirmation = model[*sourceIdIt]->confirm(selected.nSet, selected.density, visitedVertices);
		  if (!confirmation.infirmation())
		    {
		      vector<unsigned int> ancestorsOfCandidate(confirmation.getAncestors());
		      vector<unsigned int> parentsOfCandidate(confirmation.getParents());
		      vector<unsigned int> childrenOfCandidate(confirmation.getChildren());
		      while (++sourceIdIt != sourceEnd)
			{
			  const Vertex& source = *model[*sourceIdIt];
			  if (source.superPatternOf(selected.nSet))
			    {
			      confirmation = source.confirm(selected.nSet, selected.density, visitedVertices);
			      if (confirmation.infirmation())
				{
				  break;
				}
			      ancestorsOfCandidate.insert(ancestorsOfCandidate.end(), confirmation.getAncestors().begin(), confirmation.getAncestors().end());
			      parentsOfCandidate.insert(parentsOfCandidate.end(), confirmation.getParents().begin(), confirmation.getParents().end());
			      childrenOfCandidate.insert(childrenOfCandidate.end(), confirmation.getChildren().begin(), confirmation.getChildren().end());
			    }
			}
		      if (sourceIdIt == sourceEnd)
			{
#ifdef DEBUG_SELECT
			  cout << "and is confirmed by its:\n  ancestors:";
			  for (const unsigned int ancestorId : ancestorsOfCandidate)
			    {
			      cout << "\n    ";
			      roughTensor->printPattern(model[ancestorId]->getNSet(), model[ancestorId]->getDensity(), cout);
			    }
			  cout << "\n  parents:";
			  for (const unsigned int parentId : parentsOfCandidate)
			    {
			      cout << "\n    ";
			      roughTensor->printPattern(model[parentId]->getNSet(), model[parentId]->getDensity(), cout);
			    }
#endif
			  if (childrenOfCandidate.empty())
			    {
#ifdef DEBUG_SELECT
			      cout << "\n  (no child)\n";
#endif
			      updateStaticVariablesAfterConfirmation(new Vertex(selected.nSet, selected.density, tensor.addPatternToModelAndGetQuadraticErrorOfNullSubmodel(selected.nSet, selected.density)));
			      for (const unsigned int ancestorId : ancestorsOfCandidate)
				{
				  static_cast<NonSink*>(model[ancestorId])->addDescendant();
				}
			      for (const unsigned int parentId : parentsOfCandidate)
				{
				  Vertex*& parent = model[parentId];
				  if (parent->sink())
				    {
				      NonSink* upgradedSink = new NonSink(*parent);
				      delete parent;
				      parent = upgradedSink;
				    }
				  else
				    {
				      static_cast<NonSink*>(parent)->addChild();
				    }
				}
			    }
			  else
			    {
#ifdef DEBUG_SELECT
			      cout << "\nwith children:";
			      for (const unsigned int childId : childrenOfCandidate)
				{
				  cout << "\n    ";
				  roughTensor->printPattern(model[childId]->getNSet(), model[childId]->getDensity(), cout);
				}
#endif
			      NonSink* newNonSink = new NonSink(selected.nSet, selected.density, tensor.quadraticErrorOfNullSubmodel(selected.nSet, selected.density));
			      if (newNonSink->autoConfirm(childrenOfCandidate))
				{
#ifdef DEBUG_SELECT
				  cout << "\nAuto-confirmed\n";
#endif
				  updateStaticVariablesAfterConfirmation(newNonSink);
				  tensor.addPatternToModel(newNonSink->getNSet(), newNonSink->getDensity());
				  for (const unsigned int ancestorId : ancestorsOfCandidate)
				    {
				      static_cast<NonSink*>(model[ancestorId])->addDescendant();
				    }
				  for (const unsigned int parentId : parentsOfCandidate)
				    {
				      Vertex*& parent = model[parentId];
				      if (parent->sink())
					{
					  NonSink* upgradedSink = new NonSink(*parent);
					  delete parent;
					  parent = upgradedSink;
					}
				      else
					{
					  static_cast<NonSink*>(parent)->addChildAndRemoveGrandChildren();
					}
				    }
				}
			      else
				{
#ifdef DEBUG_SELECT
				  cout << "\nAuto-infirmed\n";
#endif
				  delete newNonSink;
				  updateStaticVariablesAfterInfirmation();
				}
			    }
			}
		      else
			{
#ifdef DEBUG_SELECT
			  cout << "but is infirmed by an ancestor or a parent\n";
#endif
			  updateStaticVariablesAfterInfirmation();
			}
		    }
		  else
		    {
#ifdef DEBUG_SELECT
		      cout << "but is infirmed by an ancestor or a parent\n";
#endif
		      updateStaticVariablesAfterInfirmation();
		    }
		}
	      else
		{
		  vector<unsigned int> childrenOfCandidate(1, *sourceIdIt);
		  while (++sourceIdIt != sourceEnd)
		    {
		      if (model[*sourceIdIt]->subPatternOf(selected.nSet))
			{
			  childrenOfCandidate.push_back(*sourceIdIt);
			}
		    }
#ifdef DEBUG_SELECT
		  cout << "and has children (but no parent):";
		  for (const unsigned int childId : childrenOfCandidate)
		    {
		      cout << "\n    ";
		      roughTensor->printPattern(model[childId]->getNSet(), model[childId]->getDensity(), cout);
		    }
#endif
		  NonSink* newSource = new NonSink(selected.nSet, selected.density, tensor.quadraticErrorOfNullSubmodel(selected.nSet, selected.density));
		  if (newSource->autoConfirm(childrenOfCandidate))
		    {
#ifdef DEBUG_SELECT
		      cout << "\nAuto-confirmed\n";
#endif
		      tensor.addPatternToModel(newSource->getNSet(), newSource->getDensity());
		      vector<unsigned int> stillSources;
		      set_difference(sources.begin(), sources.end(), newSource->getChildren().begin(), newSource->getChildren().end(), back_inserter(stillSources));
		      sources = std::move(stillSources);
		      sources.push_back(model.size());
		      updateStaticVariablesAfterConfirmation(newSource);
		    }
		  else
		    {
#ifdef DEBUG_SELECT
		      cout << "\nAuto-infirmed\n";
#endif
		      delete newSource;
		      updateStaticVariablesAfterInfirmation();
		    }
		}
	    }
	}
      if (isVerbose)
	{
	  cout << "\rSelecting patterns: " << model.size() << " patterns selected (AICc = " << setprecision(12) << (log(quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit()) + 2 * static_cast<double>(model.size() + 1) / (nbOfTuples - model.size() - 2)) * nbOfTuples << " + C).\n";
	}
#ifdef NB_OF_PATTERNS
#ifdef GNUPLOT
      cout << '\t' << model.size();
#else
      cout << "Nb of selected patterns: " << model.size() << '\n';
#endif
#endif
#ifdef AICC
#ifdef GNUPLOT
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined DETAILED_TIME
      cout << '\t';
#endif
      cout << setprecision(12) << (log(quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit()) + 2 * static_cast<double>(model.size() + 1) / (nbOfTuples - model.size() - 2)) * nbOfTuples;
#else
      cout << "AICc: " << setprecision(12) << (log(quadraticError / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit()) + 2 * static_cast<double>(model.size() + 1) / (nbOfTuples - model.size() - 2)) * nbOfTuples << '\n';
#endif
#endif
      if (isHierarchyPrinted)
	{
	  for (Vertex* pattern : model)
	    {
	      if (pattern->sink())
		{
		  roughTensor->output(pattern->getNSet(), pattern->getDensity());
		}
	      else
		{
		  roughTensor->output(pattern->getNSet(), pattern->getDensity(), static_cast<NonSink*>(pattern)->getChildren());
		}
	      delete pattern;
	    }
	}
      else
	{
	  for (Vertex* pattern : model)
	    {
	      roughTensor->output(pattern->getNSet(), pattern->getDensity());
	      delete pattern;
	    }
	}
    }
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count();
#else
  cout << "Selection time: " << duration_cast<duration<double>>(steady_clock::now() - startingPoint).count() << "s\n";
#endif
#endif
}

vector<vector<unsigned int>> NonSink::nSetInSubmodel(const vector<vector<unsigned int>>& subpatternNSet) const
{
  vector<vector<unsigned int>>::const_iterator nSetDimensionIt = nSet.begin();
  vector<vector<unsigned int>> v;
  v.reserve(subpatternNSet.size());
  for (const vector<unsigned int>& subpatternDimension : subpatternNSet)
    {
      vector<unsigned int> vDimension;
      const vector<unsigned int>::const_iterator begin = nSetDimensionIt->begin();
      vector<unsigned int>::const_iterator elementIt = begin;
      for (const unsigned int subpatternElement : subpatternDimension)
	{
	  for (; *elementIt != subpatternElement; ++elementIt)
	    {
	    }
	  vDimension.push_back(elementIt++ - begin);
	}
      v.emplace_back(std::move(vDimension));
      ++nSetDimensionIt;
    }
  return v;
}

void NonSink::updateStaticVariablesAfterConfirmation(Vertex* vertex)
{
  const multimap<long long, pair<long long, CandidateVariable>>::iterator end = updated.end();
  for (multimap<long long, pair<long long, CandidateVariable>>::iterator patternIt = updated.erase(updated.begin()); patternIt != end; )
    {
      // Slower but still correct if the test below is considered always true
      if (vertex->overlapsWith(patternIt->second.second.nSet))
  	{
  	  candidates.emplace(std::move(patternIt->second));
  	  patternIt = updated.erase(patternIt);
  	}
      else
  	{
  	  ++patternIt;
  	}
    }
  quadraticError += minQuadraticErrorVariation;
  model.push_back(vertex);
  quadraticErrorVariationKeepingAICcConstant = quadraticError * (exp(2 * (1. - nbOfTuples) / (nbOfTuples - model.size() - 2) / (nbOfTuples - model.size() - 3)) - 1);
  if (updated.empty() || updated.begin()->first > quadraticErrorVariationKeepingAICcConstant)
    {
      minQuadraticErrorVariation = quadraticErrorVariationKeepingAICcConstant;
      return;
    }
  minQuadraticErrorVariation = updated.begin()->first;
}

void NonSink::updateStaticVariablesAfterInfirmation()
{
  // Everything in updated is clean and the same candidate variables would be reselected (after a costly call of TrieWithPrediction::lowerBoundAndQuadraticErrorVariation) if moved back into candidates: doing so is *much* slower because the execution of forwardSelect ends with many infirmations
  updated.erase(updated.begin());
  if (updated.empty() || updated.begin()->first > quadraticErrorVariationKeepingAICcConstant)
    {
      minQuadraticErrorVariation = quadraticErrorVariationKeepingAICcConstant;
      return;
    }
  minQuadraticErrorVariation = updated.begin()->first;
}
