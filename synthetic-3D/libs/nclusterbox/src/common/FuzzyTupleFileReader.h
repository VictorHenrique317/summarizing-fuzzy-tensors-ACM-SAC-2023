// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FUZZY_TUPLE_FILE_READER_H_
#define FUZZY_TUPLE_FILE_READER_H_

#include <unordered_map>
#include <fstream>
#include <boost/tokenizer.hpp>

#include "../../Parameters.h"
#include "../utilities/UsageException.h"
#include "../utilities/NoInputException.h"
#include "../utilities/DataFormatException.h"
#include "FuzzyTuple.h"

#ifdef VERBOSE_PARSER
#include <iostream>
#endif

using namespace boost;

class FuzzyTupleFileReader
{
 public:
  FuzzyTupleFileReader(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator);

  pair<vector<FuzzyTuple>, bool> read(); /* returns the unique fuzzy tuples, ordered by FuzzyTuple::operator<, and whether they all have memberships equal to 1 (or 0, but these are not returned) */

  vector<vector<string>>& getIds2Labels();

 private:
  const string tensorFileName;
  ifstream tensorFile;
  const char_separator<char> inputDimensionSeparator;
  const char_separator<char> inputElementSeparator;
  unsigned int lineNb;
  vector<vector<string>> ids2Labels;
  vector<unordered_map<string, unsigned int>> labels2Ids;
  vector<vector<unsigned int>> nSet;
  double membership;
  vector<vector<unsigned int>::const_iterator> tupleIts;

  void init();			/* membership remains 0 if and only if the tensor is empty */
  bool parseLine(const tokenizer<char_separator<char>>::const_iterator dimensionBegin, const tokenizer<char_separator<char>>::const_iterator dimensionEnd); /* returns whether the line is to be considered (non-null membership) */
};

#endif /*FUZZY_TUPLE_FILE_READER_H_*/
