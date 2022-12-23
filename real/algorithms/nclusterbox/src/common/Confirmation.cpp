// Copyright 2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#include "Confirmation.h"

Confirmation::Confirmation(): ancestors(), parents(), children()
{
}

Confirmation::Confirmation(Confirmation&& otherConfirmation): ancestors(std::move(otherConfirmation.ancestors)), parents(std::move(otherConfirmation.parents)), children(std::move(otherConfirmation.children))
{
}

Confirmation::Confirmation(const unsigned int id): ancestors(), parents(1, id), children()
{
}

Confirmation::Confirmation(const unsigned int id, vector<unsigned int>& childrenParam): ancestors(), parents(1, id), children(move(childrenParam))
{
}

Confirmation::Confirmation(vector<unsigned int>& ancestorsParam, vector<unsigned int>& parentsParam, vector<unsigned int>& childrenParam): ancestors(ancestorsParam), parents(parentsParam), children(childrenParam)
{
}

Confirmation& Confirmation::operator=(Confirmation&& otherConfirmation)
{
  ancestors = std::move(otherConfirmation.ancestors);
  parents = std::move(otherConfirmation.parents);
  children = std::move(otherConfirmation.children);
  return *this;
}

bool Confirmation::infirmation() const
{
  return parents.empty() && ancestors.empty();
}

const vector<unsigned int>& Confirmation::getAncestors() const
{
  return ancestors;
}

const vector<unsigned int>& Confirmation::getParents() const
{
  return parents;
}

const vector<unsigned int>& Confirmation::getChildren() const
{
  return children;
}
