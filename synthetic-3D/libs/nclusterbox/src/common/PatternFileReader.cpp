// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "PatternFileReader.h"

PatternFileReader::PatternFileReader(const char* noisyNSetFileNameParam, const char* inputDimensionSeparatorParam, const char* inputElementSeparatorParam, const vector<vector<string>>& ids2Labels): noisyNSetFileName(noisyNSetFileNameParam), noisyNSetFile(noisyNSetFileName), inputDimensionSeparator(inputDimensionSeparatorParam), inputElementSeparator(inputElementSeparatorParam), labels2Ids(), lineNb(0)
{
  if (!noisyNSetFile)
    {
      throw NoInputException(noisyNSetFileNameParam);
    }
  labels2Ids.reserve(ids2Labels.size());
  for (const vector<string>& labelsInDimension : ids2Labels)
    {
      unordered_map<string, unsigned int> labels2IdsInDimension;
      labels2IdsInDimension.reserve(labelsInDimension.size());
      unsigned int id = 0;
      for (const string& label : labelsInDimension)
	{
	  labels2IdsInDimension[label] = id++;
	}
      labels2Ids.emplace_back(std::move(labels2IdsInDimension));
    }
}

unsigned int PatternFileReader::getLineNb() const
{
  return lineNb;
}

vector<vector<unsigned int>> PatternFileReader::next()
{
  if (noisyNSetFile.eof())
    {
      noisyNSetFile.close();
      return {};
    }
  ++lineNb;
  string noisyNSetString;
  getline(noisyNSetFile, noisyNSetString);
  tokenizer<char_separator<char>> dimensions(noisyNSetString, inputDimensionSeparator);
  if (dimensions.begin() == dimensions.end())
    {
      return next();
    }
#ifdef VERBOSE_PARSER
  cout << noisyNSetFileName << ':' << lineNb << ": " << noisyNSetString << '\n';
#endif
  const tokenizer<char_separator<char>>::const_iterator dimensionEnd = dimensions.end();
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
  vector<vector<unsigned int>> nSet(labels2Ids.size());
  vector<unordered_map<string, unsigned int>>::const_iterator labels2IdsIt = labels2Ids.begin();
  for (const unsigned int internalDimensionId : AbstractRoughTensor::getExternal2InternalDimensionOrder())
    {
      if (dimensionIt == dimensionEnd)
	{
	  throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("less than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
	}
      vector<unsigned int>& nSetDimension = nSet[internalDimensionId];
      const unordered_map<string, unsigned int>::const_iterator label2IdEnd = labels2IdsIt->end();
      tokenizer<char_separator<char>> elements(*dimensionIt, inputElementSeparator);
      for (const string& element : elements)
	{
	  const unordered_map<string, unsigned int>::const_iterator label2IdIt = labels2IdsIt->find(element);
	  if (label2IdIt == label2IdEnd)
	    {
	      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, (element + " is not in dimension " + lexical_cast<string>(labels2IdsIt - labels2Ids.begin()) + " of fuzzy tensor!").c_str());
	    }
	  nSetDimension.push_back(label2IdIt->second);
	}
      sort(nSetDimension.begin(), nSetDimension.end());
      if (adjacent_find(nSetDimension.begin(), nSetDimension.end()) != nSetDimension.end())
	{
	  throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("repeated elements in dimension " + lexical_cast<string>(labels2IdsIt - labels2Ids.begin()) + '!').c_str());
	}
      ++labels2IdsIt;
      ++dimensionIt;
    }
  if (dimensionIt != dimensionEnd)
    {
      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
  return nSet;
}
