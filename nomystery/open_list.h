/*********************************************************************
 * Author: Malte Helmert (helmert@informatik.uni-freiburg.de)
 * (C) Copyright 2003-2004 Malte Helmert
 * Modified by: Matthias Westphal (westpham@informatik.uni-freiburg.de)
 * (C) Copyright 2008 Matthias Westphal
 *
 * This file is part of LAMA.
 *
 * LAMA is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the license, or (at your option) any later version.
 *
 * LAMA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 *********************************************************************/

#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include <vector>
#include <queue>
using namespace std;

template<class Entry>
class OpenList {
    class IsWorse {
		public:
		bool operator() (const pair <int, Entry>& a, const pair<int, Entry>& b) const {
			return a.first > b.first;
		};
    };

    priority_queue<pair<int, Entry>, vector<pair<int, Entry> >,
	typename OpenList<Entry>::IsWorse > queue;
public:
    OpenList();
    ~OpenList();

    void insert(int key, const Entry &entry);
    Entry remove_min();
    void clear();

    int min() const;
    bool empty() const;
};

#include "open_list.cc"

// HACK! Need a better strategy of dealing with templates, also in the Makefile.

#endif
