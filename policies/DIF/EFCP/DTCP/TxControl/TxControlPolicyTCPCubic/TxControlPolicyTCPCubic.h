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

#ifndef DIF_EFCP_DTCP_TXCONTROL_TXCONTROLPOLICYTCPCUBIC_TXCONTROLPOLICYTCPCUBIC_H_
#define DIF_EFCP_DTCP_TXCONTROL_TXCONTROLPOLICYTCPCUBIC_TXCONTROLPOLICYTCPCUBIC_H_

#include <TxControlPolicyBase.h>

#include <omnetpp.h>

#include "TxControlPolicyBase.h"
#include <math.h>

class TxControlPolicyTCPCubic : public TxControlPolicyBase
{

public:
    TxControlPolicyTCPCubic();
    virtual ~TxControlPolicyTCPCubic();
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

    //Events
    void onEachAck(DTPState* dtpState);
    void onDupAck();
    void onECNSlowDown();

    //Actions
    void cubicReset();

    virtual bool run(DTPState* dtpState, DTCPState* dtcpState);

    //Statistic signals
    simsignal_t sigStatTCPCubicCWND;
    simsignal_t sigStatDelay;
    simsignal_t sigStatEcnCounter;

    cMessage* m1;
    cMessage* m2;
    cMessage* m3;

protected:
    void cubicUpdate();
    void wFast();

    double cubicBeta, cubicC;
    uint32_t fastAlpha;
    double fastGamma;
    double fastWUpdateInterval;
    uint32_t fastMaxCwnd;
    double rtt, minRtt;
    uint32_t cwnd, ssthreshold, wmax;
    uint32_t phase, cubicSlowStartMode;
    uint32_t congNotificationCounter;
    double flowStartTime;
    double lastCwndDecrementTime, timeElapsedSinceDecrement;
    bool congestionDetected;
};


#endif /* DIF_EFCP_DTCP_TXCONTROL_TXCONTROLPOLICYTCPCUBIC_TXCONTROLPOLICYTCPCUBIC_H_ */
