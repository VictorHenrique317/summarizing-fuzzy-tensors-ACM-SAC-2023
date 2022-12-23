// Copyright 2019 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of paf.

// paf is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// paf is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with paf.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CONFIRMATION_H_
#define CONFIRMATION_H_

#include <vector>

using namespace std;

class Confirmation
{
 public:
  Confirmation();
  Confirmation(Confirmation&& otherConfirmation);
  Confirmation(const unsigned int id);
  Confirmation(const unsigned int id, vector<unsigned int>& children);
  Confirmation(vector<unsigned int>& ancestors, vector<unsigned int>& parents, vector<unsigned int>& children);

  Confirmation& operator=(Confirmation&& otherConfirmation);

  bool infirmation() const;
  const vector<unsigned int>& getAncestors() const;
  const vector<unsigned int>& getParents() const;
  const vector<unsigned int>& getChildren() const;

 private:
  vector<unsigned int> ancestors;
  vector<unsigned int> parents;
  vector<unsigned int> children;
};

#endif	/* CONFIRMATION_H_ */
