//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <DTCPState.h>
#include <DTPState.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simutil.h>
#include <TxControlPolicyTCPCubic.h>
#include <algorithm>
#include <iostream>

Register_Class(TxControlPolicyTCPCubic);

TxControlPolicyTCPCubic::TxControlPolicyTCPCubic()
{

}

TxControlPolicyTCPCubic::~TxControlPolicyTCPCubic()
{

}

void TxControlPolicyTCPCubic::initialize() {
    sigStatTCPCubicCWND = registerSignal("cwnd");
    sigStatDelay = registerSignal("delay");
    sigStatEcnCounter = registerSignal("ecn_counter");

    cwnd = 10;
    rtt = 2;
    ssthreshold = 1000000000;
    wmax = 13;
    minRtt = 1000000000;

    cubicBeta = getSystemModule()->par("cubicBeta");
    cubicC = getSystemModule()->par("cubicC");
    fastAlpha = getSystemModule()->par("fastAlpha");
    fastGamma = getSystemModule()->par("fastGamma");
    fastWUpdateInterval = getSystemModule()->par("fastWUpdateInterval");

    cubicSlowStartMode = (uint32_t)getSystemModule()->par("cubicSlowStartMode"); //0: classic SS; 1: FAST SS
    phase = 0; //0: slow-start; 1-congestion avoidance
    fastMaxCwnd = 0;

    flowStartTime = 0;
    timeElapsedSinceDecrement = 0;
    lastCwndDecrementTime = 0;
    congNotificationCounter = 0;
    congestionDetected = false;

    if(cubicSlowStartMode == 1){
        m2 = new cMessage("fast_cwnd_calc");
        scheduleAt(simTime().dbl() + 0.1, m2);
    }

    m3 = new cMessage("misc_per_rtt");
    scheduleAt(simTime().dbl() + 0.1, m3);
}

void TxControlPolicyTCPCubic::onEachAck(DTPState* dtpState) {
    rtt = dtpState->getRtt() == 0 ? 0.1 : dtpState->getRtt();
    if (rtt < minRtt){
        minRtt = rtt;
    }

    if (cubicSlowStartMode == 0){
        if (phase == 0){
            cwnd++;
            emit(sigStatTCPCubicCWND, cwnd);
        }
    }
}

void TxControlPolicyTCPCubic::onDupAck() {
}

void TxControlPolicyTCPCubic::onECNSlowDown() {
    Enter_Method("TxControlPolicyTCPCubic");

    congNotificationCounter++;

    if(!congestionDetected){
        congestionDetected = true;
        scheduleAt(simTime().dbl() + std::max(rtt,0.001), new cMessage("clear_cong_notif"));
        //scheduleAt(simTime().dbl() + 0.001, new cMessage("clear_cong_notif"));

        if (phase == 0){
            phase = 1;
            wmax = cwnd;

            m1 = new cMessage("cubic_cwnd_calc");
            scheduleAt(simTime().dbl() + std::max(rtt,0.001), m1);

            if (cubicSlowStartMode == 1){
                cancelAndDelete(m2);
            }

            //cubicUpdate();

        } else {
            if(cwnd >= wmax){
                wmax = std::max(cwnd,(uint32_t)13);
            } else {
                wmax = std::max((uint32_t)(cwnd * (2 - cubicBeta)/2),(uint32_t)13);
            }
            lastCwndDecrementTime = simTime().dbl();
            cubicUpdate();
        }
    }
//    congNotificationCounter++;
//    if (congNotificationCounter % 3){
//        if (phase == 0){
//            phase = 1;
//            wmax = cwnd;
//
//            m1 = new cMessage("cubic_cwnd_calc");
//            scheduleAt(simTime().dbl() + std::max(rtt,0.02), m1);
//
//            if (cubicSlowStartMode == 1){
//                cancelAndDelete(m2);
//            }
//
//        } else {
//            if(cwnd >= wmax){
//                wmax = std::max(cwnd,(uint32_t)13);
//            } else {
//                wmax = std::max((uint32_t)(cwnd * (2 - cubicBeta)/2),(uint32_t)13);
//            }
//            lastCwndDecrementTime = simTime().dbl();
//            cubicUpdate();
//        }
//    }
}

void TxControlPolicyTCPCubic::cubicReset() {
}

void TxControlPolicyTCPCubic::handleMessage(cMessage* msg) {
    if(msg->isSelfMessage()){
        if(!strcmp(msg->getName(), "cubic_cwnd_calc")){
            cubicUpdate();

            m1 = new cMessage("cubic_cwnd_calc");
            scheduleAt(simTime().dbl() + std::max(rtt,0.02), m1);
        }

        if(!strcmp(msg->getName(), "fast_cwnd_calc")){

            if(cwnd > fastMaxCwnd){
                fastMaxCwnd = cwnd;
            } else {
                phase = 1;
                wmax = cwnd;
                lastCwndDecrementTime = simTime().dbl();
                cubicUpdate();

                m1 = new cMessage("cubic_cwnd_calc");
                scheduleAt(simTime().dbl() + std::max(rtt,0.02), m1);
            }

            if(phase == 0){
                wFast();
                m2 = new cMessage("fast_cwnd_calc");
                scheduleAt(simTime().dbl() + fastWUpdateInterval, m2);
            }
        }

        if(!strcmp(msg->getName(), "clear_cong_notif")){
            congestionDetected = false;
        }

        if(!strcmp(msg->getName(), "misc_per_rtt")){
            emit(sigStatEcnCounter, congNotificationCounter);
            congNotificationCounter = 0;

            m3 = new cMessage("misc_per_rtt");
            scheduleAt(simTime().dbl() + std::max(rtt,0.02), m3);
        }

        emit(sigStatTCPCubicCWND, cwnd);

        delete msg;
    }
}

void TxControlPolicyTCPCubic::cubicUpdate() {

    //Calculating the K value
    double K = pow(((wmax * cubicBeta)/cubicC),0.3333);

    //Calculating the new cwnd
    timeElapsedSinceDecrement = simTime().dbl() - std::max(flowStartTime, lastCwndDecrementTime);
    uint32_t cwndCubic = (uint32_t)(cubicC * pow((timeElapsedSinceDecrement - K), 3) + wmax);

    uint32_t cwndTCP = wmax * ( 1 - cubicBeta) + 3 * (cubicBeta/(2 - cubicBeta)) * timeElapsedSinceDecrement / rtt;

    cwnd = cwndCubic > cwndTCP ? cwndCubic : cwndTCP;
}

bool TxControlPolicyTCPCubic::run(DTPState* dtpState, DTCPState* dtcpState)
{
  Enter_Method("TxControlPolicyTCPCubic");

  if(flowStartTime == 0)
      flowStartTime = simTime().dbl();

  dtcpState->setSndCredit(std::max(cwnd,(uint32_t)10));
  //dtcpState->setSenderRightWinEdge(std::min((uint32_t)dtcpState->getSndLeftWinEdge() + cwnd - 1, (uint32_t)dtcpState->getRcvRightWinEdge()));
  dtcpState->setSenderRightWinEdge((uint32_t)dtcpState->getSndLeftWinEdge() + std::max(cwnd,(uint32_t)10));

  //Registering sendCredit and flightSize for statistics
//  emit(sigStatTCPCubicCWND, cwnd);

  defaultAction(dtpState, dtcpState);

  return false;
}

void TxControlPolicyTCPCubic::wFast() {
    cwnd = std::min(2 * cwnd, (uint32_t)((1 - fastGamma) * cwnd + fastGamma * (minRtt/rtt) * cwnd + fastAlpha));
}
