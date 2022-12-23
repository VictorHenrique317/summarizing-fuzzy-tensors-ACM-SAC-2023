// Copyright 2019-2022 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef N_SET_HASH_H_
#define N_SET_HASH_H_

#include <vector>
#include <boost/functional/hash.hpp>

template<typename T>
struct nSetHash
{
  size_t operator()(const std::vector<std::vector<T>>& nSet) const
  {
    size_t seed = 0;
    for (const std::vector<T>& dimension : nSet)
      {
	boost::hash_combine(seed, boost::hash_range(dimension.begin(), dimension.end()));
      }
    return seed;
  }
};

#endif /*N_SET_HASH_H_*/
