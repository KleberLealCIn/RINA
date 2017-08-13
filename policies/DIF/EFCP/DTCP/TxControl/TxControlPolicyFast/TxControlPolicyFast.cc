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
 * @file TxControlPolicyFast.cc
 * @author Kleber Leal (kal2@cin.ufpe.br)
 * @date May 9, 2017
 * @brief This class implement the FAST TCP algorithm in RINA
 * @detail
 */

#include "TxControlPolicyFast.h"

Register_Class(TxControlPolicyFast);

TxControlPolicyFast::TxControlPolicyFast()
{

}

TxControlPolicyFast::~TxControlPolicyFast()
{

}

void TxControlPolicyFast::handleMessage(cMessage* msg) {
    if(msg->isSelfMessage()){
        if(!strcmp(msg->getName(), "cwnd_calc")){
            cwnd = std::max(4,wFast(cwnd, rtt));
            emit(sigStatWFast, cwnd);

            m1 = new cMessage("cwnd_calc");
            scheduleAt(simTime() + 0.02, m1);
        }

        if(!strcmp(msg->getName(), "misc_per_rtt")){
            emit(sigStatEcnCounter, congNotificationCounter);
            congNotificationCounter = 0;

            if (RttBase != 10000000)
                emit(sigStatRttBase, RttBase);

            m2 = new cMessage("misc_per_rtt");
            scheduleAt(simTime() + std::max(rtt,0.02), m2);
        }

        delete msg;


    }
}

void TxControlPolicyFast::onECNSlowDown() {
//    if (congNotificationCounter == 0){
//        if (congDetection == 1){
//            RttBase = rtt;
//            cwnd = 10;
//        }
//    }

    if (congDetection == 1 && congNotificationCounter == 0){
        RttBase = rtt;
        cwnd = 10;
    }
    congNotificationCounter++;
}

void TxControlPolicyFast::onEachAck(DTPState* dtpState) {
    K++;
    rtt = dtpState->getRtt();

    if (congDetection == 0 && rtt < RttBase){
        RttBase = rtt;
    }

    if ((congDetection == 1 || congDetection == 2) && rtt < RttBase && K <= 100){
        K = 0;
        RttBase = rtt;
    }

    if (congDetection == 2 && rtt > 2 * RttBase){
        K = 0;
        cwnd = 10;
        RttBase = rtt;
    }
}

void TxControlPolicyFast::initialize(int step) {
    sigStatWFast = registerSignal("W_Fast");
    sigStatEcnCounter = registerSignal("ecn_counter");
    sigStatRttBase = registerSignal("rtt_base");

    packetSize = par("packetSize");  // default 1500
    cwnd = par("initialSendCredit"); // default 4
    RttBase = 10000000.0;
    lastWindowRecalcTime = simTime().dbl();
    m1 = new cMessage("cwnd_calc");
    scheduleAt(simTime()+0.02, m1);
    congNotificationCounter = 0;

    m2 = new cMessage("misc_per_rtt");
    scheduleAt(simTime().dbl() + 0.1, m2);

    fastAlpha = getSystemModule()->par("fastAlpha");
    fastGamma = getSystemModule()->par("fastGamma");
    fastWUpdateInterval = getSystemModule()->par("fastWUpdateInterval");
    congDetection = getSystemModule()->par("congDetection");

    K = 0;
}

int TxControlPolicyFast::wFast(uint32_t cwnd, double rtt) {
   cwnd = std::min(2 * cwnd, (uint32_t)((1 - fastGamma) * cwnd + fastGamma * (RttBase/rtt) * cwnd + fastAlpha));
   return cwnd;
}

bool TxControlPolicyFast::run(DTPState* dtpState, DTCPState* dtcpState)
{
  Enter_Method("TxControlPolicyFast");

  dtcpState->setSenderRightWinEdge(dtcpState->getSndLeftWinEdge() + cwnd);

  defaultAction(dtpState, dtcpState);

  return false;
}
