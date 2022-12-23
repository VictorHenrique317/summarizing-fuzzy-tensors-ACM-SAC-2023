// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "FuzzyTupleFileReader.h"

FuzzyTupleFileReader::FuzzyTupleFileReader(const char* tensorFileNameParam, const char* inputDimensionSeparatorParam, const char* inputElementSeparatorParam): tensorFileName(tensorFileNameParam), tensorFile(tensorFileName), inputDimensionSeparator(inputDimensionSeparatorParam), inputElementSeparator(inputElementSeparatorParam), lineNb(0), ids2Labels(), labels2Ids(), nSet(), membership(0), tupleIts()
{
  if (!tensorFile)
    {
      throw NoInputException(tensorFileNameParam);
    }
  init();
}

void FuzzyTupleFileReader::init()
{
  if (tensorFile.eof())
    {
      throw UsageException(("No fuzzy tuple in " + tensorFileName + '!').c_str());
    }
  ++lineNb;
  string nSetString;
  getline(tensorFile, nSetString);
  tokenizer<char_separator<char>> dimensions(nSetString, inputDimensionSeparator);
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
  if (dimensionIt == dimensions.end())
    {
      init();
      return;
    }
  ids2Labels.resize(std::distance(++dimensionIt, dimensions.end()));
  labels2Ids.resize(ids2Labels.size());
  nSet.resize(ids2Labels.size());
  tupleIts.resize(ids2Labels.size());
  while (!parseLine(dimensions.begin(), dimensions.end()))
    {
      if (tensorFile.eof())
	{
	  throw UsageException(("All fuzzy tuples in " + tensorFileName + " have null membership degrees!").c_str());
	}
      ++lineNb;
      getline(tensorFile, nSetString);
      dimensions = tokenizer<char_separator<char>>(nSetString, inputDimensionSeparator);
    }
#ifdef VERBOSE_PARSER
  cout << tensorFileName << ':' << lineNb << ": " << nSetString << '\n';
#endif
}

pair<vector<FuzzyTuple>, bool> FuzzyTupleFileReader::read()
{
  bool is01 = true;
  for (vector<FuzzyTuple> fuzzyTuples; ; )
    {
      fuzzyTuples.emplace_back(tupleIts, membership);
      // Advance tuple in nSet, little-endian-like
      vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
      for (vector<vector<unsigned int>>::const_iterator nSetIt = nSet.begin(); nSetIt != nSet.end() && ++*tupleItsIt == nSetIt->end(); ++nSetIt)
	{
	  *tupleItsIt++ = nSetIt->begin();
	}
      if (tupleItsIt == tupleIts.end())
	{
	  // All tuples in nSet enumerated: find and parse the next line (if any) with non-null membership
	  for (; ; )
	    {
	      if (tensorFile.eof())
		{
		  tensorFile.close();
		  fuzzyTuples.shrink_to_fit();
		  stable_sort(fuzzyTuples.begin(), fuzzyTuples.end());
		  fuzzyTuples.erase(unique(fuzzyTuples.begin(), fuzzyTuples.end()), fuzzyTuples.end());
		  return {fuzzyTuples, is01};
		}
	      ++lineNb;
	      string nSetString;
	      getline(tensorFile, nSetString);
	      tokenizer<char_separator<char>> dimensions(nSetString, inputDimensionSeparator);
	      if (dimensions.begin() != dimensions.end() && parseLine(dimensions.begin(), dimensions.end()))
		{
#ifdef VERBOSE_PARSER
		  cout << tensorFileName << ':' << lineNb << ": " << nSetString << '\n';
#endif
		  break;
		}
	    }
	  is01 = is01 && membership == 1;
	}
    }
}

vector<vector<string>>& FuzzyTupleFileReader::getIds2Labels()
{
  return ids2Labels;
}

bool FuzzyTupleFileReader::parseLine(const tokenizer<char_separator<char>>::const_iterator dimensionBegin, const tokenizer<char_separator<char>>::const_iterator dimensionEnd)
{
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensionBegin;
  tokenizer<char_separator<char>>::const_iterator membershipIt = dimensionBegin;
  while (++dimensionIt != dimensionEnd)
    {
      membershipIt = dimensionIt;
    }
  try
    {
      membership = lexical_cast<double>(*membershipIt);
      if (membership == 0)
	{
	  return false;
	}
      if (membership < 0 || membership > 1)
	{
	  throw bad_lexical_cast();
	}
    }
  catch (bad_lexical_cast &)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("the membership, " + *membershipIt + ", should be a double in [0, 1]!").c_str());
    }
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  dimensionIt = dimensionBegin;
  vector<vector<unsigned int>>::iterator nSetIt = nSet.begin();
  for (unordered_map<string, unsigned int>& labels2IdsInDimension : labels2Ids)
    {
      nSetIt->clear();
      tokenizer<char_separator<char>> elements(*dimensionIt, inputElementSeparator);
      const tokenizer<char_separator<char>>::const_iterator elementEnd = elements.end();
      tokenizer<char_separator<char>>::const_iterator elementIt = elements.begin();
      if (elementIt == elementEnd)
	{
	  throw DataFormatException(tensorFileName.c_str(), lineNb, ("no element in dimension " + lexical_cast<string>(nSetIt - nSet.begin()) + '!').c_str());
	}
      do
	{
	  const pair<unordered_map<string, unsigned int>::const_iterator, bool> label2Id = labels2IdsInDimension.insert({*elementIt, ids2LabelsIt->size()});
	  if (label2Id.second)
	    {
	      ids2LabelsIt->push_back(*elementIt);
	    }
	  nSetIt->push_back(label2Id.first->second);
	}
      while (++elementIt != elementEnd);
      if (++dimensionIt == dimensionEnd)
	{
	  throw DataFormatException(tensorFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
	}
      ++ids2LabelsIt;
      ++nSetIt;
    }
  if (dimensionIt != membershipIt)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  for (const vector<unsigned int>& dimension : nSet)
    {
      *tupleItsIt++ = dimension.begin();
    }
  return true;
}
