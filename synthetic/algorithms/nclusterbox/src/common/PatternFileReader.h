// Copyright 2018-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PATTERN_FILE_READER_H_
#define PATTERN_FILE_READER_H_

#include "AbstractRoughTensor.h"

class PatternFileReader
{
 public:
  PatternFileReader(const char* noisyNSetFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const vector<vector<string>>& ids2Labels);

  unsigned int getLineNb() const;
  vector<vector<unsigned int>> next();

 private:
  const string noisyNSetFileName;
  ifstream noisyNSetFile;
  const char_separator<char> inputDimensionSeparator;
  const char_separator<char> inputElementSeparator;
  vector<unordered_map<string, unsigned int>> labels2Ids;
  unsigned int lineNb;
};

#endif /*PATTERN_FILE_READER_H_*/
