// Copyright 2009,2010,2011 Loïc Cerf (magicbanana@gmail.com)

// This file is part of num-noise.

// num-noise is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// num-noise is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with num-noise; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include <vector>
#include <set>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/math/special_functions/beta.hpp>
#include "sysexits.h"

using namespace std;
using namespace boost;
using namespace boost::math;

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

vector<vector<set<unsigned int> > > selectNSetsHavingValue(const unsigned int dimensionId, const unsigned int value, const vector<vector<set<unsigned int> > >& nSets)
{
  vector<vector<set<unsigned int> > > selection;
  for (vector<vector<set<unsigned int> > >::const_iterator nSetIt = nSets.begin(); nSetIt != nSets.end(); ++nSetIt)
    {
      const set<unsigned int>& dimension = (*nSetIt)[dimensionId];
      if (dimension.find(value) != dimension.end())
	{
	  selection.push_back(*nSetIt);
	}
    }
  return selection;
}

bool isTupleInANSet(const vector<unsigned int>::const_iterator& tupleBegin, const vector<unsigned int>::const_iterator& tupleEnd, const vector<vector<set<unsigned int> > >& nSets, unsigned int dimensionId = 0)
{
  if (tupleBegin == tupleEnd)
    {
      return !nSets.empty();
    }
  const vector<unsigned int>::const_iterator nextTupleIt = tupleBegin + 1;
  return isTupleInANSet(nextTupleIt, tupleEnd, selectNSetsHavingValue(dimensionId, *tupleBegin, nSets), dimensionId + 1);
}

void output(const vector<unsigned int>& tuple)
{
  for (vector<unsigned int>::const_iterator dimensionIt = tuple.begin(); dimensionIt != tuple.end(); ++dimensionIt)
    {
      cout << *dimensionIt << ' ';
    }
}

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      cerr << "Usage: num-noise dataset-sizes nb-of-correct-observations-per-tuple nb-of-incorrect-observations-per-tuple" << endl;
      return EX_USAGE;
    }
  const vector<unsigned int> sizes(getVectorFromString<unsigned int>(argv[1]));
  char_separator<char> dimensionSeparator(" ");
  char_separator<char> valueSeparator(",");
  vector<vector<set<unsigned int> > > perfectContext;
  vector<vector<unsigned int> > trueNTuples;
  while (cin.good())
    {
      vector<set<unsigned int> > nSet;
      nSet.reserve(sizes.size());
      string nSetString;
      getline(cin, nSetString);
      tokenizer<char_separator<char> > dimensions(nSetString, dimensionSeparator);
      tokenizer<char_separator<char> >::const_iterator dimensionIt = dimensions.begin();
      if (dimensionIt != dimensions.end())
	{
	  for (vector<unsigned int>::const_iterator sizeIt = sizes.begin(); sizeIt != sizes.end(); ++sizeIt)
	    {
	      if (dimensionIt == dimensions.end())
		{
		  cerr << "A n-set has strictly less than " << sizes.size() << " dimensions!" << endl;
		  return EX_DATAERR;
		}
	      set<unsigned int> dimension;
	      tokenizer<char_separator<char> > values(*dimensionIt, valueSeparator);
	      for (tokenizer<char_separator<char> >::const_iterator valueIt = values.begin(); valueIt != values.end(); ++valueIt)
		{
		  unsigned int value = lexical_cast<unsigned int>(valueIt->c_str());
		  if (value >= *sizeIt)
		    {
		      cerr << "A n-set cannot fit in the dataset!" << endl;
		      return EX_DATAERR;
		    }
		  dimension.insert(value);
		}
	      nSet.push_back(dimension);
	      ++dimensionIt;
	    }
	  if (dimensionIt != dimensions.end())
	    {
	      cerr << "A n-set has strictly more than " << sizes.size() << " dimensions!" << endl;
	      return EX_DATAERR;
	    }
	  perfectContext.push_back(nSet);
	}
    }
  double alpha = lexical_cast<double>(argv[2]) + 1;
  double beta = lexical_cast<double>(argv[3]) + 1;
  vector<unsigned int> lastTupleBrowsed(sizes.size(), 0);
  vector<unsigned int>::reverse_iterator lastDimensionIncreasedIt = lastTupleBrowsed.rbegin();
  mt19937 generator(time(NULL));
  uniform_01<mt19937> zeroone(generator);
  for (; ; )
    {
      output(lastTupleBrowsed);
      if (isTupleInANSet(lastTupleBrowsed.begin(), lastTupleBrowsed.end(), perfectContext))
	{
	  cout << ibeta_inv(alpha, beta, zeroone()) << endl;
	}
      else
	{
	  cout << ibeta_inv(beta, alpha, zeroone()) << endl;
	}
      lastDimensionIncreasedIt = lastTupleBrowsed.rbegin();
      for (vector<unsigned int>::const_reverse_iterator sizeIt = sizes.rbegin(); sizeIt != sizes.rend() && *lastDimensionIncreasedIt == *sizeIt - 1; ++sizeIt)
	{
	  *lastDimensionIncreasedIt = 0;
	  ++lastDimensionIncreasedIt;
	}
      if (lastDimensionIncreasedIt != lastTupleBrowsed.rend())
	{
	  ++(*lastDimensionIncreasedIt);
	}
      else
	{
	  break;
	}
    }
  return EX_OK;
}
