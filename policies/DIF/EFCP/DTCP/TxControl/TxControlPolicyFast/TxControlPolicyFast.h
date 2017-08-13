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
/**
 * @file TxControlPolicyFast.h
 * @author Kleber Leal (kal2@cin.ufpe.br)
 * @date May 9, 2017
 * @brief This class implement the FAST TCP algorithm in RINA
 * @detail
 */

#ifndef TXCONTROLPOLICYFAST_H_
#define TXCONTROLPOLICYFAST_H_

#include <omnetpp.h>
#include "TxControlPolicyBase.h"

//#define ALPHA  1.0
//#define BETA   3.0

class TxControlPolicyFast : public TxControlPolicyBase
{
protected:
    uint32_t packetSize, flightSize, cwnd;
    double rtt, RttBase, lastWindowRecalcTime;
    int wFast(uint32_t cwnd, double rtt);
    uint32_t congNotificationCounter;
    uint32_t fastAlpha;
    double fastGamma;
    double fastWUpdateInterval;
    uint32_t congDetection;
    uint32_t K;

public:
    TxControlPolicyFast();
    virtual ~TxControlPolicyFast();
    virtual bool run(DTPState* dtpState, DTCPState* dtcpState);
    virtual void handleMessage(cMessage* msg);
    virtual void onECNSlowDown();
    virtual void onEachAck(DTPState* dtpState);

    //Statistic signals
    simsignal_t sigStatWFast;
    simsignal_t sigStatEcnCounter;
    simsignal_t sigStatRttBase;
    using TxControlPolicyBase::initialize;
    virtual void initialize(int step);
    cMessage* m1;
    cMessage* m2;
};

#endif /* TXCONTROLPOLICYFAST_H_ */
