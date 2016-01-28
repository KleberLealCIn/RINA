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
 * @file TxControlPolicyLG.h
 * @author Marcel Marek (imarek@fit.vutbr.cz)
 * @date May 3, 2015
 * @brief This is an example policy class implementing LG Initial Sequence Number behavior
 * @detail
 */

#ifndef TXCONTROLPOLICYLG_H_
#define TXCONTROLPOLICYLG_H_

#include "TxControlPolicyBase.h"
#include "RTTEstimatorPolicyLG.h"

#define INITIAL_RATE    0.03
#define ALPHA           0.10
#define RST_WND         1
#define SEGMENT_SIZE    1500
#define BANDWIDTH       95000000//9800000

class TxControlPolicyLG : public TxControlPolicyBase
{
private:
    double rate;
    double alpha;
    double flightSize;
    double sendCredit;
    double cwnd;

    double totalPackets;
    double ecnMarkedPackets;
    double usedLoad;

    simtime_t lastUpdate;

    simsignal_t sigStatRate;
    simsignal_t sigStatFlightSize;
    simsignal_t sigStatUsedLoad;

    RTTEstimatorPolicyLG* rttEstimatorPolicyLG;

public:
    TxControlPolicyLG();
    virtual ~TxControlPolicyLG();
    virtual bool run(DTPState* dtpState, DTCPState* dtcpState);

    void updateRate(double load, double acked, double lastECN);
    double getActualRate(DTPState* dtpState);
    double getRate();

    void initialize();
};

#endif /* TXCONTROLPOLICYLG_H_ */
