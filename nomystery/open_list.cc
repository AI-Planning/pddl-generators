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

// #include "open_list.h"

// HACK! Ignore this if used as a top-level compile target.
#ifdef OPEN_LIST_H

using namespace std;

/*
  Priority_queue based implementation of an open list.
*/

template<class Entry>
OpenList<Entry>::OpenList() {
}

template<class Entry>
OpenList<Entry>::~OpenList() {
}

template<class Entry>
inline void OpenList<Entry>::insert(int key, const Entry &entry) {
    queue.push(make_pair(key, entry));
}

template<class Entry>
inline int OpenList<Entry>::min() const {
    return queue.top.first();
}

template<class Entry>
inline Entry OpenList<Entry>::remove_min() {
    Entry best = queue.top().second;
    queue.pop();
    return best;
}

template<class Entry>
inline bool OpenList<Entry>::empty() const {
    return queue.empty();
}

template<class Entry>
inline void OpenList<Entry>::clear() {
    queue = priority_queue<pair<int, Entry>, vector<pair<int, Entry> >,
	typename OpenList<Entry>::IsWorse >();
}

#endif
