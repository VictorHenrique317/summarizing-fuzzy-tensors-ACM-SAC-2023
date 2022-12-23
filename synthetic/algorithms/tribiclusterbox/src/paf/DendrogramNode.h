// Copyright 2018-2020 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENDROGRAM_NODE_H_
#define DENDROGRAM_NODE_H_

#include "../utilities/CompletionMessage.h"
#include "../common/AbstractRoughTensor.h"
#include "CandidateNode.h"

class DendrogramNode
{
 public:
  DendrogramNode(const DendrogramNode& otherDendrogramNode);
  DendrogramNode(DendrogramNode&& otherDendrogramNode);
  DendrogramNode(const vector<vector<unsigned int>>& nSet, const long long membershipSum);

  DendrogramNode& operator=(DendrogramNode&& otherDendrogramNode);

  static void setLeavesAndTensor(const vector<pair<vector<vector<unsigned int>>, long long>>& leaves, const AbstractRoughTensor* roughTensor, Trie& tensor);
  static void amalgamate(const bool isVerbose);

 private:
  unsigned int id;
  vector<vector<unsigned int>> nSet;
  unsigned int area;
  long long membershipSum;

  static const AbstractRoughTensor* roughTensor;
  static Trie tensor;
  static unsigned int maxId;
  static vector<DendrogramNode> dendrogramFrontier;
  static vector<CandidateNode> candidates;

  DendrogramNode();

  friend ostream& operator<<(ostream& out, const DendrogramNode& pattern);

  pair<vector<vector<unsigned int>>, unsigned int> unionAndArea(const DendrogramNode& otherDendrogramNode) const;
  long long unionDensity(const DendrogramNode& otherDendrogramNode) const;
  long long unionDensityIfNotSupersetOf(const DendrogramNode& otherDendrogramNode) const; /* returns numeric_limits<long long>::min() if *this is a super-pattern of otherDendrogramNode */
  pair<long long, unsigned int> intersectionMembershipSumAndArea(const DendrogramNode& otherDendrogramNode) const;
  long long quadraticErrorVariation(const DendrogramNode& otherChild, const long long candidateDensity) const;
  void output() const;

#ifdef DEBUG_AMALGAMATE
  void printExtendDebug(const unsigned int dimensionId, const unsigned int elementId, const float membershipSumOnExtension) const;
#endif
};

#endif	/* DENDROGRAM_NODE_H_ */
