// Copyright 2019,2020 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VERTEX_H_
#define VERTEX_H_

#include <cmath>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>

#include "../utilities/InclusionResult.h"
#include "Confirmation.h"

using namespace boost;

class Vertex
{
 public:
  Vertex(Vertex&& otherVertex);
  Vertex(vector<vector<unsigned int>>& nSet, const int density, const long long quadraticError);

  virtual ~Vertex();

  virtual bool sink() const;

  const vector<vector<unsigned int>>& getNSet() const;
  int getDensity() const;
  unsigned int getArea() const;
  long long getG(const int backgroundDensity) const;

  virtual void visit(dynamic_bitset<>& descendants) const;

  bool overlapsWith(const vector<vector<unsigned int>>& otherNSet) const;
  bool subPatternOf(const vector<vector<unsigned int>>& otherNSet) const;
  bool superPatternOf(const vector<vector<unsigned int>>& otherNSet) const;
  InclusionResult subOrSuperPatternOf(const vector<vector<unsigned int>>& otherNSet) const;

  virtual Confirmation confirm(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity, dynamic_bitset<>& visitedVertices) const;

 protected:
  const unsigned int id;
  const vector<vector<unsigned int>> nSet;
  const int density;
  long long quadraticErrorOfSubmodel;

  static vector<Vertex*> model;

  long long getQuadraticErrorDecreaseOfNullSubmodel(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity) const;
};

#endif	/* VERTEX_H_ */
