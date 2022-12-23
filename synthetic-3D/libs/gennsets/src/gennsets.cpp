// Copyright 2009,2010,2011 Loïc Cerf (magicbanana@gmail.com)

// This file is part of gennsets.

// gennsets is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// gennsets is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with gennsets; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <boost/tokenizer.hpp>
#include "sysexits.h"

#include "NoFileException.h"
#include "CannotWriteFileException.h"

using namespace boost;

template<typename T> vector<T> getVectorFromString(const string& str)
{
  vector<T> tokens;
  T token;
  stringstream ss(str);
  while (ss >> token)
    {
      tokens.push_back(token);
    }
  return tokens;
}

int main(int argc, char* argv[])
{
  if (argc != 3 && argc != 4)
    {
      cerr << "Usage: gennsets dataset-sizes n-set-size-file [output-file]" << endl;
      return EX_USAGE;
    }
  const vector<unsigned int> sizes(getVectorFromString<unsigned int>(argv[1]));
  char* nSetSizeFileName = argv[2];
  ifstream nSetSizeFile(nSetSizeFileName);
  if (nSetSizeFile.fail())
    {
      cerr << NoFileException(nSetSizeFileName).what() << endl;
      return EX_IOERR;
    }
  ofstream outputFile;
  if (argc == 4)
    {
      outputFile.open(argv[3]);
      if (outputFile.fail())
	{
	  cerr << CannotWriteFileException(argv[3]).what() << endl;
	  return EX_IOERR;
	}
    }
  srand(time(NULL));
  while (nSetSizeFile.good())
    {
      string sizeString;
      getline(nSetSizeFile, sizeString);
      tokenizer<char_separator<char> > nSetSizes(sizeString);
      tokenizer<char_separator<char> >::const_iterator nSetSizeIt = nSetSizes.begin();
      if (nSetSizeIt != nSetSizes.end())
	{
	  for (vector<unsigned int>::const_iterator sizeIt = sizes.begin(); sizeIt != sizes.end(); ++sizeIt)
	    {
	      if (nSetSizeIt == nSetSizes.end())
		{
		  cerr << "The sizes of a n-set have strictly less than " << sizes.size() << " dimensions!" << endl;
		  return EX_USAGE;
		}
	      unsigned int nSetSize = atoi((*nSetSizeIt++).c_str());
	      if (*sizeIt < nSetSize)
		{
		  cerr << "A n-set cannot fit in the dataset!" << endl;
		  return EX_USAGE;
		}
	      set<unsigned int> dimension;
	      while (dimension.size() != nSetSize)
		{
		  dimension.insert(rand() % *sizeIt);
		}
	      if (sizeIt != sizes.begin())
		{
		  cout << ' ';
		  if (argc == 4)
		    {
		      outputFile << ' ';
		    }
		}
	      for (set<unsigned int>::const_iterator valueIt = dimension.begin(); valueIt != dimension.end(); ++valueIt)
		{
		  if (valueIt != dimension.begin())
		    {
		      cout << ',';
		      if (argc == 4)
			{
			  outputFile << ',';
			}
		    }
		  cout << *valueIt;
		  if (argc == 4)
		    {
		      outputFile << *valueIt;
		    }
		}	      
	    }
	  if (nSetSizeIt != nSetSizes.end())
	    {
	      cerr << "The sizes of a n-set have strictly more than " << sizes.size() << " dimensions!" << endl;
	      return EX_USAGE;
	    }
	  cout << endl;
	  if (argc == 4)
	    {
	      outputFile << endl;
	    }
	}
    }
  return EX_OK;
}
