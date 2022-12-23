// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseRoughTensor.h"
#include "SparseRoughTensor.h"

vector<vector<string>> AbstractRoughTensor::ids2Labels;
double AbstractRoughTensor::nullModelQuadraticError;
vector<unsigned int> AbstractRoughTensor::cardinalities;
int AbstractRoughTensor::unit;
bool AbstractRoughTensor::isNoSelection;
vector<unsigned int> AbstractRoughTensor::external2InternalDimensionOrder;
vector<vector<vector<unsigned int>>> AbstractRoughTensor::candidateVariables;
bool AbstractRoughTensor::isPrintLambda;

ofstream AbstractRoughTensor::outputFile;
string AbstractRoughTensor::outputDimensionSeparator;
string AbstractRoughTensor::outputElementSeparator;
string AbstractRoughTensor::hierarchyPrefix;
string AbstractRoughTensor::hierarchySeparator;
string AbstractRoughTensor::sizePrefix;
string AbstractRoughTensor::sizeSeparator;
string AbstractRoughTensor::areaPrefix;
bool AbstractRoughTensor::isSizePrinted;
bool AbstractRoughTensor::isAreaPrinted;

#if defined TIME || defined DETAILED_TIME
steady_clock::time_point AbstractRoughTensor::overallBeginning;
#endif
#ifdef DETAILED_TIME
steady_clock::time_point AbstractRoughTensor::shiftingBeginning;
#endif

AbstractRoughTensor::~AbstractRoughTensor()
{
  outputFile.close();
}

void AbstractRoughTensor::printPattern(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const
{
  vector<vector<string>>::const_iterator ids2LabelsIt = ids2Labels.begin();
  for (const unsigned int internalDimensionId : external2InternalDimensionOrder)
    {
      const vector<unsigned int>& dimension = nSet[internalDimensionId];
      out << (*ids2LabelsIt)[dimension.front()];
      const vector<unsigned int>::const_iterator end = dimension.end();
      for (vector<unsigned int>::const_iterator elementIt = dimension.begin(); ++elementIt != end; )
	{
	  out << outputElementSeparator << (*ids2LabelsIt)[*elementIt];
	}
      out << outputDimensionSeparator;
      ++ids2LabelsIt;
    }
  if (isPrintLambda)
    {
      out << density / unit;
    }
  else
    {
      out << density / unit + getAverageShift(nSet);
    }
  if (isSizePrinted)
    {
      out << sizePrefix << nSet[external2InternalDimensionOrder.front()].size();
      const vector<unsigned int>::const_iterator end = external2InternalDimensionOrder.end();
      for (vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin(); ++internalDimensionIdIt != end; )
	{
	  out << sizeSeparator << nSet[*internalDimensionIdIt].size();
	}
    }
  if (isAreaPrinted)
    {
      unsigned int area = 1;
      for (const vector<unsigned int>& dimension : nSet)
	{
	  area *= dimension.size();
	}
      out << areaPrefix << area;
    }
}

void AbstractRoughTensor::output(const vector<vector<unsigned int>>& nSet, const float density) const
{
  printPattern(nSet, density, outputFile);
  outputFile << '\n';
}

void AbstractRoughTensor::output(const vector<vector<unsigned int>>& nSet, const float density, const double quadraticError) const
{
  printPattern(nSet, density, outputFile);
  outputFile << ' ' << quadraticError / unit / unit << '\n';
}

void AbstractRoughTensor::output(const vector<vector<unsigned int>>& nSet, const float density, const vector<unsigned int>& children) const
{
  printPattern(nSet, density, outputFile);
  if (!children.empty())
    {
      outputFile << hierarchyPrefix << children.front() + 1;
      const vector<unsigned int>::const_iterator end = children.end();
      for (vector<unsigned int>::const_iterator childIdIt = children.begin(); ++childIdIt != end; )
	{
	  outputFile << hierarchySeparator << *childIdIt + 1;
	}
    }
  outputFile << '\n';
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double densityThreshold, const double shift)
{
  if (Trie::is01)
    {
      if ((8 * sizeof(double) + 1) * getTensorArea() < 8 * (3 * sizeof(unsigned int*) + sizeof(unsigned int) * (ids2Labels.size() + 1) + sizeof(double)) * fuzzyTuples.size())
      	{
	  // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
	  return new DenseRoughTensor(fuzzyTuples, shift);
      	}
      setMetadata(fuzzyTuples, shift);
      SparseCrispTube::setDefaultMembershipAndSizeLimit(unit * -shift, densityThreshold * cardinalities.back() / sizeof(unsigned int) / 8);
      DenseCrispTube::setSize(cardinalities.back());
      return new SparseRoughTensor(fuzzyTuples, shift);
    }
  if ((sizeof(unsigned int) + sizeof(double)) * getTensorArea() < (3 * sizeof(unsigned int*) + sizeof(unsigned int) * (ids2Labels.size() + 2) + sizeof(double)) * fuzzyTuples.size())
    {
      // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
      return new DenseRoughTensor(fuzzyTuples, shift);
    }
  setMetadata(fuzzyTuples, shift);
  SparseFuzzyTube::setDefaultMembershipAndSizeLimit(unit * -shift, densityThreshold * cardinalities.back() / 2);
  DenseFuzzyTube::setSize(cardinalities.back());
  return new SparseRoughTensor(fuzzyTuples, shift);
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyTuples = getFuzzyTuples(tensorFileName, inputDimensionSeparator, inputElementSeparator, isVerbose);
  double shift;
  if (Trie::is01)
    {
      shift = fuzzyTuples.size();
    }
  else
    {
      shift = 0;
      for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
	{
	  shift += fuzzyTuple.getMembership();
	}
    }
  for (const vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      shift /= ids2LabelsInDimension.size();
    }
  return makeRoughTensor(fuzzyTuples, densityThreshold, shift);
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const double shift, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyTuples = getFuzzyTuples(tensorFileName, inputDimensionSeparator, inputElementSeparator, isVerbose);
  return makeRoughTensor(fuzzyTuples, densityThreshold, shift);
}

void AbstractRoughTensor::setOutput(const char* outputFileName, const char* outputDimensionSeparatorParam, const char* outputElementSeparatorParam, const char* hierarchyPrefixParam, const char* hierarchySeparatorParam, const char* sizePrefixParam, const char* sizeSeparatorParam, const char* areaPrefixParam, const bool isPrintLambdaParam, const bool isSizePrintedParam, const bool isAreaPrintedParam, const bool isNoSelectionParam)
{
#ifdef DETAILED_TIME
#ifdef GNUPLOT
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - shiftingBeginning).count();
#else
  cout << "Tensor shifting time: " << duration_cast<duration<double>>(steady_clock::now() - shiftingBeginning).count() << "s\n";
#endif
#endif
  outputDimensionSeparator = outputDimensionSeparatorParam;
  outputElementSeparator = outputElementSeparatorParam;
  hierarchyPrefix = hierarchyPrefixParam;
  hierarchySeparator = hierarchySeparatorParam;
  sizePrefix = sizePrefixParam;
  sizeSeparator = sizeSeparatorParam;
  areaPrefix = areaPrefixParam;
  isPrintLambda = isPrintLambdaParam;
  isSizePrinted = isSizePrintedParam;
  isAreaPrinted = isAreaPrintedParam;
  outputFile.open(outputFileName);
  isNoSelection = isNoSelectionParam;
  if (!outputFile)
    {
      throw NoOutputException(outputFileName);
    }
}

int AbstractRoughTensor::getUnit()
{
  return unit;
}

bool AbstractRoughTensor::isDirectOutput()
{
  return isNoSelection;
}

const vector<unsigned int>& AbstractRoughTensor::getCardinalities()
{
  return cardinalities;
}

const vector<vector<string>>& AbstractRoughTensor::getIds2Labels()
{
  return ids2Labels;
}

const vector<unsigned int>& AbstractRoughTensor::getExternal2InternalDimensionOrder()
{
  return external2InternalDimensionOrder;
}

unsigned int AbstractRoughTensor::getNbOfCandidateVariables()
{
  return candidateVariables.size();
}

double AbstractRoughTensor::unitDenominatorGivenNullModelQuadraticError()
{
  if (nullModelQuadraticError > 1)
    {
      // sqrt(nullModelQuadraticError) is a possible unit denominator (sqrt because the quadratic error is stored in a long long)
      return sqrt(nullModelQuadraticError);
    }
  return 1;
}

void AbstractRoughTensor::setUnit(const int unitParam)
{
#if defined NUMERIC_PRECISION && !defined GNUPLOT
  cout << "Numeric precision: " << 1. / unitParam << '\n';
#endif
  unit = unitParam;
}

void AbstractRoughTensor::setUnitForProjectedTensor(const double quadraticError, const vector<double>& elementNegativeMemberships, const vector<double>& elementPositiveMemberships)
{
  if (quadraticError > 1)
    {
#if defined NUMERIC_PRECISION && defined GNUPLOT
      cout << '\t' << max(sqrt(quadraticError), max(*max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end()), *max_element(elementPositiveMemberships.begin(), elementPositiveMemberships.end()))) / numeric_limits<int>::max();
#endif
      setUnit(static_cast<double>(numeric_limits<int>::max()) / max(sqrt(quadraticError), max(*max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end()), *max_element(elementPositiveMemberships.begin(), elementPositiveMemberships.end()))));
      return;
    }
#if defined NUMERIC_PRECISION && defined GNUPLOT
  cout << '\t' << max(1., max(*max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end()), *max_element(elementPositiveMemberships.begin(), elementPositiveMemberships.end()))) / numeric_limits<int>::max();
#endif
  setUnit(static_cast<double>(numeric_limits<int>::max()) / max(1., max(*max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end()), *max_element(elementPositiveMemberships.begin(), elementPositiveMemberships.end()))));
}

vector<FuzzyTuple> AbstractRoughTensor::getFuzzyTuples(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isVerbose)
{
#if defined TIME || defined DETAILED_TIME
  overallBeginning = steady_clock::now();
#endif
  if (isVerbose)
    {
      cout << "Parsing fuzzy tensor ... " << flush;
    }
  FuzzyTupleFileReader fuzzyTupleFileReader(tensorFileName, inputDimensionSeparator, inputElementSeparator);
  pair<vector<FuzzyTuple>, bool> fuzzyTuplesAndIs01 = fuzzyTupleFileReader.read();
  Trie::is01 = fuzzyTuplesAndIs01.second;
  ids2Labels = std::move(fuzzyTupleFileReader.getIds2Labels());
  if (isVerbose)
    {
      cout << "\rParsing fuzzy tensor: " << fuzzyTuplesAndIs01.first.size() << '/' << getTensorArea() << " tuples with non-null membership degrees.\n";
    }
#ifdef DETAILED_TIME
  shiftingBeginning = steady_clock::now();
#ifdef GNUPLOT
  cout << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count();
#else
  cout << "Tensor parsing time: " << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count() << "s\n";
#endif
#endif
  if (isVerbose)
    {
      cout << "Shifting fuzzy tensor ... " << flush;
    }
  return fuzzyTuplesAndIs01.first;
}

unsigned long long AbstractRoughTensor::getTensorArea()
{
  unsigned long long area = 1;
  for (const vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      area *= ids2LabelsInDimension.size();
    }
  return area;
}

void AbstractRoughTensor::orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities()
{
  vector<pair<unsigned int, unsigned int>> dimensions;
  dimensions.reserve(ids2Labels.size());
  unsigned int dimensionId = 0;
  for (vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      dimensions.emplace_back(ids2LabelsInDimension.size(), dimensionId);
      ++dimensionId;
    }
  // Order dimensions by increasing cardinality
  sort(dimensions.begin(), dimensions.end(), [](const pair<unsigned int, unsigned int>& dimension1, const pair<unsigned int, unsigned int>& dimension2) {return dimension1.first < dimension2.first;});
  // Compute external2InternalDimensionOrder and cardinalities
  cardinalities.reserve(dimensions.size());
  external2InternalDimensionOrder.resize(dimensions.size());
  for (const pair<unsigned int, unsigned int>& dimension : dimensions)
    {
      external2InternalDimensionOrder[dimension.second] = cardinalities.size();
      cardinalities.push_back(dimension.first);
    }
}

void AbstractRoughTensor::setMetadata(vector<FuzzyTuple>& fuzzyTuples, const double shift)
{
  // Sparse tensor
  unsigned long long area = getTensorArea();
  orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities();
  double unitDenominator;
  if (Trie::is01)
    {
      nullModelQuadraticError = (static_cast<double>(shift) * area - 2 * fuzzyTuples.size()) * shift + fuzzyTuples.size();
      for (FuzzyTuple& fuzzyTuple : fuzzyTuples)
	{
	  fuzzyTuple.shiftMembership(shift);
	}
      unitDenominator = unitDenominatorGivenNullModelQuadraticError();
      const double maxElementNegativeMembership = shift * (area / cardinalities.back());
      if (maxElementNegativeMembership > unitDenominator)
	{
	  unitDenominator = maxElementNegativeMembership;
	}
    }
  else
    {
      nullModelQuadraticError = static_cast<double>(shift) * shift * (area - fuzzyTuples.size());
      for (FuzzyTuple& fuzzyTuple : fuzzyTuples)
	{
	  fuzzyTuple.shiftMembership(shift);
	  nullModelQuadraticError += fuzzyTuple.getMembershipSquared();
	}
      unitDenominator = unitDenominatorGivenNullModelQuadraticError();
    }
  unsigned int dimensionId = 0;
  for (vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      const unsigned int nbOfElements = ids2LabelsInDimension.size();
      // Computing positive and, for fuzzy tensors, negative memberships of the elements
      vector<pair<double, unsigned int>> elementPositiveMemberships;
      elementPositiveMemberships.reserve(nbOfElements);
      for (unsigned int id = 0; id != nbOfElements; ++id)
	{
	  elementPositiveMemberships.emplace_back(0, id);
	}
      if (Trie::is01)
	{
	  const double oneMinusShift = 1. - shift;
	  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
	    {
	      const unsigned int elementId = fuzzyTuple.getElementId(dimensionId);
	      elementPositiveMemberships[elementId].first += oneMinusShift;
	    }
	}
      else
	{
	  vector<double> elementNegativeMemberships(nbOfElements, shift * (area / nbOfElements)); // assumes every membership null and correct that in the loop below
	  for (const FuzzyTuple& fuzzyTuple : fuzzyTuples)
	    {
	      const unsigned int elementId = fuzzyTuple.getElementId(dimensionId);
	      const double membership = fuzzyTuple.getMembership();
	      if (membership > 0)
		{
		  elementPositiveMemberships[elementId].first += membership;
		  elementNegativeMemberships[elementId] -= shift;
		}
	      else
		{
		  elementNegativeMemberships[elementId] -= membership + shift;
		}
	    }
	  const double maxNegativeMembership = *max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end());
	  if (maxNegativeMembership > unitDenominator)
	    {
	      unitDenominator = maxNegativeMembership;
	    }
	}
      sort(elementPositiveMemberships.begin(), elementPositiveMemberships.end(), [](const pair<double, unsigned int>& elementPositiveMembership1, const pair<double, unsigned int>& elementPositiveMembership2) {return elementPositiveMembership1.first < elementPositiveMembership2.first;});
      if (elementPositiveMemberships.back().first > unitDenominator)
	{
	  unitDenominator = elementPositiveMemberships.back().first;
	}
      // Computing the new ids, in increasing order of the positive membership (for faster lower_bound in SparseFuzzyTube::sumOnSlice and for Trie::maxExtension to choose the element with the greatest membership in case of equality) and reorder ids2LabelsInDimension accordingly
      vector<unsigned int> mapping(nbOfElements);
      vector<string> newIds2LabelsInDimension;
      newIds2LabelsInDimension.reserve(nbOfElements);
      unsigned int newId = 0;
      for (const pair<double, unsigned int>& elementPositiveMembership : elementPositiveMemberships)
	{
	  newIds2LabelsInDimension.emplace_back(std::move(ids2LabelsInDimension[elementPositiveMembership.second]));
	  mapping[elementPositiveMembership.second] = newId++;
	}
      ids2LabelsInDimension = std::move(newIds2LabelsInDimension);
      // Remap the element of the fuzzyTuples accordingly
      for (FuzzyTuple& fuzzyTuple : fuzzyTuples)
	{
	  unsigned int& elementId = fuzzyTuple.getElementId(dimensionId);
	  elementId = mapping[elementId];
	}
      ++dimensionId;
    }
  // Reorder fuzzy tuples, according to the new dimension order
  for (FuzzyTuple& fuzzyTuple : fuzzyTuples)
    {
      fuzzyTuple.reorder(external2InternalDimensionOrder);
    }
  // Set unit
#if defined NUMERIC_PRECISION && defined DETAILED_TIME && defined GNUPLOT
  cout << '\t' << unitDenominator / numeric_limits<int>::max();
#endif
  setUnit(static_cast<double>(numeric_limits<int>::max()) / unitDenominator);
}

void AbstractRoughTensor::setMetadata(vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, const double maxNegativeMembership)
{
  // Dense tensor
  orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities();
  double unitDenominator = unitDenominatorGivenNullModelQuadraticError();
  if (maxNegativeMembership > unitDenominator)
    {
      unitDenominator = maxNegativeMembership;
    }
  vector<vector<pair<double, unsigned int>>>::iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
  for (vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      sort(elementPositiveMembershipsInDimensionIt->begin(), elementPositiveMembershipsInDimensionIt->end(), [](const pair<double, unsigned int>& elementPositiveMembership1, const pair<double, unsigned int>& elementPositiveMembership2) {return elementPositiveMembership1.first < elementPositiveMembership2.first;});
      const unsigned int nbOfElements = ids2LabelsInDimension.size();
      // Computing the new ids, in increasing order of the positive membership (for faster lower_bound in SparseFuzzyTube::sumOnSlice and for Trie::maxExtension to choose the element with the greatest membership in case of equality) and reorder ids2LabelsInDimension accordingly
      vector<string> newIds2LabelsInDimension;
      newIds2LabelsInDimension.reserve(nbOfElements);
      for (const pair<double, unsigned int>& elementPositiveMembership : *elementPositiveMembershipsInDimensionIt)
	{
	  newIds2LabelsInDimension.emplace_back(std::move(ids2LabelsInDimension[elementPositiveMembership.second]));
	}
      ids2LabelsInDimension = std::move(newIds2LabelsInDimension);
      if (elementPositiveMembershipsInDimensionIt->back().first > unitDenominator)
	{
	  unitDenominator = elementPositiveMembershipsInDimensionIt->back().first;
	}
      ++elementPositiveMembershipsInDimensionIt;
    }
#if defined NUMERIC_PRECISION && defined DETAILED_TIME && defined GNUPLOT
  cout << '\t' << unitDenominator / numeric_limits<int>::max();
#endif
  setUnit(static_cast<double>(numeric_limits<int>::max()) / unitDenominator);
}

vector<vector<unsigned int>> AbstractRoughTensor::projectMetadata(const unsigned int nbOfPatternsHavingAllElements, const bool isReturningOld2New)
{
  // Compute new cardinalities, new ids and rewrite ids2Labels and candidateVariables accordingly
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<vector<unsigned int>> idMapping(external2InternalDimensionOrder.size());
  for (const unsigned int internalDimensionId : external2InternalDimensionOrder)
    {
      unsigned int& cardinality = cardinalities[internalDimensionId];
      dynamic_bitset<> elementsInDimension(cardinality);
      const vector<vector<vector<unsigned int>>>::const_iterator end = candidateVariables.end();
      for (vector<vector<vector<unsigned int>>>::const_iterator patternIt = end - nbOfPatternsHavingAllElements; patternIt != end; ++patternIt)
	{
	  for (const unsigned int id : (*patternIt)[internalDimensionId])
	    {
	      elementsInDimension.set(id);
	    }
	}
      vector<unsigned int> oldIds2NewIdsInDimension(cardinality, numeric_limits<unsigned int>::max());
      cardinality = 0;
      if (isReturningOld2New)
	{
	  for (dynamic_bitset<>::size_type id = elementsInDimension.find_first(); id != dynamic_bitset<>::npos; id = elementsInDimension.find_next(id))
	    {
	      (*ids2LabelsIt)[id].swap((*ids2LabelsIt)[cardinality]);
	      oldIds2NewIdsInDimension[id] = cardinality++;
	    }
	}
      else
	{
	  vector<unsigned int>& newIds2OldIdsInDimension = idMapping[internalDimensionId];
	  newIds2OldIdsInDimension.reserve(elementsInDimension.count());
	  for (dynamic_bitset<>::size_type id = elementsInDimension.find_first(); id != dynamic_bitset<>::npos; id = elementsInDimension.find_next(id))
	    {
	      newIds2OldIdsInDimension.push_back(id);
	      (*ids2LabelsIt)[id].swap((*ids2LabelsIt)[cardinality]);
	      oldIds2NewIdsInDimension[id] = cardinality++;
	    }
	}
      for (vector<vector<unsigned int>>& candidateVariable : candidateVariables)
	{
	  for (unsigned int& id : candidateVariable[internalDimensionId])
	    {
	      id = oldIds2NewIdsInDimension[id];
	    }
	}
      if (isReturningOld2New)
	{
	  idMapping[internalDimensionId] = std::move(oldIds2NewIdsInDimension);
	}
      ++ids2LabelsIt;
    }
  return idMapping;
}

void AbstractRoughTensor::copyAsCandidateVariable(const vector<vector<unsigned int>>& nSet)
{
  candidateVariables.emplace_back(nSet);
}

void AbstractRoughTensor::moveAsCandidateVariable(vector<vector<unsigned int>>& nSet)
{
  candidateVariables.emplace_back(std::move(nSet));
}

void AbstractRoughTensor::reserveAdditionalCandidateVariables(const unsigned int nbOfAdditionalCandidateVariables)
{
  candidateVariables.reserve(candidateVariables.size() + nbOfAdditionalCandidateVariables);
}

vector<vector<vector<unsigned int>>>& AbstractRoughTensor::getCandidateVariables()
{
  return candidateVariables;
}

double AbstractRoughTensor::getNullModelQuadraticError()
{
  return nullModelQuadraticError * unit * unit;
}

#if defined DEBUG_NCLUSTERBOX || defined DEBUG_GROW || defined DEBUG_AGGLOMERATE || defined ASSERT
void AbstractRoughTensor::printElement(const unsigned int dimensionId, const unsigned int elementId, ostream& out)
{
  vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  for (; *internalDimensionIdIt != dimensionId; ++internalDimensionIdIt)
    {
    }
  const unsigned int externalId = internalDimensionIdIt - external2InternalDimensionOrder.begin();
  out << ids2Labels[externalId][elementId] << " of dimension " << externalId;
}
#endif

#ifdef TIME
void AbstractRoughTensor::printCurrentDuration()
{
#ifdef GNUPLOT
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined AICC || defined DETAILED_TIME
  cout << '\t';
#endif
  cout << duration_cast<duration<double>>(steady_clock::now() - overallBeginning).count();
#else
  cout << "Total time: " << duration_cast<duration<double>>(steady_clock::now() - overallBeginning).count() << "s\n";;
#endif
}
#endif
