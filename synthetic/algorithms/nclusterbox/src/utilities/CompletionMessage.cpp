// Copyright (C) 2018,2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "CompletionMessage.h"

template<class T> string CompletionMessage<T>::prefix;
template<class T> T CompletionMessage<T>::max;
template<class T> T CompletionMessage<T>::previous;

template<class T> void CompletionMessage<T>::set(const string& message, const T& maxParam)
{
  cout << message << ":  0.0%" << flush;
  prefix = message + ": ";
  max = maxParam;
  previous = 1000;
}

template<class T> void CompletionMessage<T>::print(const T& remaining)
{
  T perMille = 1000 * remaining / max;
  if (perMille != previous && perMille != 0)
    {
      previous = perMille;
      cout << '\r' << prefix;
      if (perMille > 900)
	{
	  cout << ' ';
	}
      perMille = 1000 - perMille;
      cout << perMille / 10 << '.' << perMille % 10 << '%' << flush;
    }
}

template<class T> void CompletionMessage<T>::done()
{
  cout << '\r' << prefix << "done.\n";
}

template class CompletionMessage<unsigned int>;
template class CompletionMessage<unsigned long long>;
