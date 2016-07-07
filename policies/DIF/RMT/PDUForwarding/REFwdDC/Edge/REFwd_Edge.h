#pragma once
#include "RuleExceptionsFWD.h"


namespace REFwdDC {

using namespace std;
using namespace NSPSimpleDC;

    class REFwd_Edge: public RuleExceptionsFWD {
    public:
        bool setNeigh(const DCAddr & n_addr, Port port);
        void setDst(const DCAddr & n_addr, const set<DCAddr> & next);

    protected:
        unsigned char nSpine;
        portList downList;

        void initializeSubPolicy();

        portList readException(const Exception & e);
        portList readRawException(const RawException & raw);
        portList executeRule(const DCAddr & n_addr);
        Exception computeException(const RawException & raw);
    };


}