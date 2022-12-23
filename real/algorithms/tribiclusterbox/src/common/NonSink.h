// Copyright 2019,2020 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef NON_SINK_H_
#define NON_SINK_H_

#include <map>
#include <iomanip>

#include "Vertex.h"
#include "AbstractRoughTensor.h"
#include "CandidateVariable.h"

class NonSink final : public Vertex
{
 public:
  NonSink(Vertex& sink);	/* upgrade of sink; the only child is *model.back() */
  NonSink(vector<vector<unsigned int>>& nSet, const int density, const long long quadraticErrorOfSubmodel); /* direct construction of a NonSink; autoConfirm to be called right after */

  bool sink() const;

  const vector<unsigned int>& getChildren() const; /* to be called after a positive auto-confirmation; result in argument of addChildAndRemoveGrandChildren */

  void visit(dynamic_bitset<>& descendants) const;

  void addDescendant();		/* to be called on ancestors after pushing back the confirmed Vertex in model */
  void addChild();		/* to be called on parents after pushing back the confirmed sink in model */
  void addChildAndRemoveGrandChildren(); /* to be called on parents after pushing back the confirmed and auto-confirmed non-sink in model */
  Confirmation confirm(const vector<vector<unsigned int>>& candidateNSet, const int candidateDensity, dynamic_bitset<>& visitedVertices) const;
  bool autoConfirm(const vector<unsigned int>& children);

  static void forwardSelect(const unsigned int nbOfCandidateVariablesHavingAllElements, AbstractRoughTensor* roughTensor, const bool isVerbose, const bool isHierarchyPrinted);

 private:
  vector<unsigned int> children;
  TrieWithPrediction tensorOfSubmodel;
  unsigned int nbOfParametersOfSubmodel;

  static TrieWithPrediction tensor; /* input tensor projected on the elements involved in the candidate variables */
  static long long nbOfTuples;	/* in the input tensor */
  static double quadraticError;	/* of the current model */
  static long long minQuadraticErrorVariation; /* either quadraticErrorVariationKeepingAICcConstant or, if smaller, the (negative) quadratic error variation of the best pattern in updated */
  static long long quadraticErrorVariationKeepingAICcConstant;	/* the (negative) quadratic error variation that would keep the AICc constant */
  static multimap<long long, CandidateVariable> candidates; /* ordered by lower bound of the quadratic error variation if selected */
  static multimap<long long, pair<long long, CandidateVariable>> updated; /* ordered by quadratic error variation if selected */

  vector<vector<unsigned int>> nSetInSubmodel(const vector<vector<unsigned int>>& subpatternNSet) const;

  static void updateStaticVariablesAfterConfirmation(Vertex* vertex);
  static void updateStaticVariablesAfterInfirmation();
};

#endif	/* NON_SINK_H_ */
