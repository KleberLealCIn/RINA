// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Brno University of Technology, PRISTINE project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "SimpleDCForwarding.h"

namespace NSPSimpleDC {

using namespace std;

    struct fFWDEntry {
        char entryType;         //0 == NONE, 1 == UP, 2 DOWN, 3 BOTH
        bool inverseStorage;
        set<Port> ports;

        fFWDEntry();
    };

    class SimpleFabricForwarding: public iSimpleDCForwarding {

    public:
        bool setNeigh(const DCAddr & n_addr, Port port);
        void setDst(const DCAddr & n_addr, const set<DCAddr> & next);
        void finish();


    protected:
        int upCount, downCount;
        Port * portsArray;
        vector<Port> upV, downV, bothV;
        map<DCAddr, fFWDEntry> table;

        void onPolicyInit();
        vector<Port> search(const DCAddr & n_addr);

        fFWDEntry getFWDEntryUP(const set<int> & pIds);
        fFWDEntry getFWDEntryDOWN(const set<int> & pIds);
        fFWDEntry getFWDEntryBOTH(const set<int> & pIdsU, const set<int> & pIdsD);
    };

}
