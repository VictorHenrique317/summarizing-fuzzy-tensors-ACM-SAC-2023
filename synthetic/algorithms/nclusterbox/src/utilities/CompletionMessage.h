// Copyright (C) 2018,2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef COMPLETION_MESSAGE_H_
#define COMPLETION_MESSAGE_H_

#include <iostream>

using namespace std;

template<class T> class CompletionMessage
{
 public:
  static void set(const string& message, const T& max);
  static void print(const T& remaining);
  static void done();

 private:
  static string prefix;
  static T max;
  static T previous;
};

#endif /*COMPLETION_MESSAGE_H_*/
