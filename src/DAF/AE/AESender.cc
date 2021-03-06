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

#include "AESender.h"

Define_Module(AESender);

PingMsgg::PingMsgg(){
    pingAt = simTime();
}

PingMsgg * PingMsgg::dup() const {
    auto *t = new PingMsgg(*this);
    t->pingAt = pingAt;
    return t;
}

PongMsgg::PongMsgg(simtime_t _pingAt){
    pingAt = _pingAt;
    pongAt = simTime();
}

PongMsgg * PongMsgg::dup() const {
       auto *t = new PongMsgg(pingAt);
       t->pongAt = pongAt;
       return t;
   }

AESender::AESender() : AE() {
    S_TIM_START           = "StartCommunication";
    S_TIM_COM             = "MakeCommunication";
    S_TIM_STOP            = "StopCommunication";

    S_PAR_START           = "startAt";
    S_PAR_STOP            = "stopAt";
    S_PAR_SEND            = "sendAfter";

    S_PAR_DSTAPNAME       = "dstApName";
    S_PAR_DSTAPINSTANCE   = "dstApInstance";
    S_PAR_DSTAENAME       = "dstAeName";
    S_PAR_DSTAEINSTANCE   = "dstAeInstance";

    S_PAR_RATE            = "rate";
    S_PAR_RATE_VAR        = "ratevar";
    S_PAR_SIZE            = "size";
    S_PAR_SIZE_VAR        = "sizevar";

    S_MSG_PING            = "PING-";
    S_PAR_PING            = "pingAt";
    S_VAL_MODULEPATH      = "getFullPath()";
}

AESender::~AESender() {
    connectionState = NIL;
    FlowObject = NULL;
    Irm = NULL;
    Cdap = NULL;
}

void AESender::prepareAllocateRequest() {
    //Schedule AllocateRequest
    cMessage* m1 = new cMessage(S_TIM_START);
    scheduleAt(startAt, m1);
}


void AESender::prepareDeallocateRequest() {
    //Schedule DeallocateRequest
    cMessage* m3 = new cMessage(S_TIM_STOP);
    scheduleAt(stopAt, m3);
}

void AESender::initialize()
{
    //Init pointers
    initPointers();
    //Source info
    initNamingInfo();
    //Setup signals
    initSignalsAndListeners();
    //Init QoSRequirements
    initQoSRequiremets();

    //Timers
    startAt = simTime() + par(S_PAR_START);
    stopAt  = simTime() + par(S_PAR_STOP);
    sendAfter  = par(S_PAR_SEND);
    if(sendAfter<1){
        sendAfter = 1;
    }

    //Time between PDUS (s)
    rate    = par(S_PAR_RATE).doubleValue();
    ratevar    = par(S_PAR_RATE_VAR).doubleValue();

    //SIZE of PDU
    size    = par(S_PAR_SIZE);
    sizevar    = par(S_PAR_SIZE_VAR);

    //Destination for flow
    dstApName     = this->par(S_PAR_DSTAPNAME).stringValue();
    dstApInstance = this->par(S_PAR_DSTAPINSTANCE).stringValue();
    dstAeName     = this->par(S_PAR_DSTAENAME).stringValue();
    dstAeInstance = this->par(S_PAR_DSTAEINSTANCE).stringValue();

    if (!dstAeName.compare("AeErr")) {
        EV << "Destination AEName is set to default which is AeErr. AeErr is for special testing purposes. Are you sure that it is right?" << endl;
    }

    //Schedule AllocateRequest
    if (startAt > 0)
        prepareAllocateRequest();
    //Schedule DeallocateRequest
    /*
    if (stopAt > 0)
        prepareDeallocateRequest();
    */
    myPath = this->getFullPath();

    send = 0;
    received = 0;
    pingreceived = 0;
    sendSize = 0;
    receivedSize = 0;
    pingreceivedSize = 0;

    minDelay = 999;
    maxDelay = -1;
    firstR = -1;
    lastR = 0;

    recTimes = par("recTimes").boolValue();

    pduburst = par("pduburst").longValue();
    if(pduburst<1) { pduburst = 1; }

    //Watchers
    WATCH(FlowObject);
    WATCH(send);
    WATCH(received);
    WATCH(pingreceived);
    WATCH(sendSize);
    WATCH(receivedSize);
    WATCH(pingreceivedSize);
    WATCH(minDelay);
    WATCH(maxDelay);
}

void AESender::finish()
{
    if(par("printAtEnd").boolValue()){
        EV << "At "<<this->getApni()<<endl;
        if(FlowObject != NULL) {
            EV << "With QoS " << FlowObject->getConId().getQoSId() <<endl;
        }
        EV << "send " << send << " ("<<sendSize << ")"<<endl;
  //      EV << "pingsRcv "  << pingreceived << " ("<<pingreceivedSize << ")"<<endl;
        EV << "pongsRcv "  << received << " ("<<receivedSize << ")"<<endl;
        EV << "delay "  << minDelay << " / "<<maxDelay<<endl;
        EV << "timestamps "  << firstR << " -> "<<lastR<<endl;

        if(recTimes){
            EV << "-----------------"<<endl;
            for(std::map<double, int>::iterator it = times.begin(); it!=times.end(); it++) {
                EV << "  " << it->first << " " << it->second <<endl;
            }
            EV << "-----------------"<<endl;
        }
        EV << "-----------------"<<endl;
    }
}

void AESender::handleSelfMessage(cMessage *msg) {
    //EV << flows.back().info() << endl;
    if ( !strcmp(msg->getName(), S_TIM_START) ) {
        //FIXME: Vesely - last flow in a list?!

        //Flow
        APNamingInfo src = this->getApni();
        APNamingInfo dst = APNamingInfo( APN(this->dstApName), this->dstApInstance,
                                         this->dstAeName, this->dstAeInstance);

        FlowObject = new Flow(src, dst);
        FlowObject->setQosRequirements(this->getQoSRequirements());

        //Insert it to the Flows ADT
        insertFlow();

        sendAllocationRequest(FlowObject);

        //Schedule ComRequest
        cMessage* m = new cMessage(S_TIM_COM);
        scheduleAt(simTime()+sendAfter+omnetpp::uniform(omnetpp::getEnvir()->getRNG(0), 0,rate), m);
    }
    else if ( !strcmp(msg->getName(), S_TIM_STOP) ) {
        sendDeallocationRequest(FlowObject);
    }
    else if ( !strcmp(msg->getName(), S_TIM_COM) ) {
        if(stopAt > simTime()){
            int tburst = omnetpp::intuniform(omnetpp::getEnvir()->getRNG(0), 1,pduburst);

            double msgWait = tburst*rate;
            for(int i = 0; i < tburst; i++){
                int msgSize = size + omnetpp::intuniform(omnetpp::getEnvir()->getRNG(0), -sizevar,sizevar);
                msgWait += omnetpp::uniform(omnetpp::getEnvir()->getRNG(0), -ratevar,ratevar);
                //Create PING messsage
                CDAP_M_Read* ping = new PingMsgg();

            //    std::cout << "sendPing"<<endl;
                ping->setByteLength(msgSize);

                //Send message
                sendData(FlowObject, ping);
                send++;
                sendSize += msgSize;
            }
            //Schedule ComRequest
            cMessage* m = new cMessage(S_TIM_COM);
            scheduleAt(simTime()+msgWait, m);
        }
    }
    else
        EV << this->getFullPath() << " received unknown self-message " << msg->getName();
    delete(msg);
}

void AESender::handleMessage(cMessage *msg)
{
    if ( msg->isSelfMessage() )
            this->handleSelfMessage(msg);
}

void AESender::processMRead(CDAPMessage* msg) {
    PingMsgg* ping = check_and_cast<PingMsgg*>(msg);
    if(ping){
        PongMsgg* pong = new PongMsgg(ping->pingAt);
        pong->setByteLength(msg->getByteLength());

        sendData(FlowObject, pong);
/*
        pingreceived++;
        pingreceivedSize += msg->getByteLength();
        simtime_t delay = simTime() - ping->pingAt;
        if(minDelay>delay){
            minDelay = delay;
        }
        if(maxDelay<delay){
            maxDelay = delay;
        }
        if(firstR<0)  {
            firstR = simTime();
        }
        lastR = simTime();
*/
    }
}

void AESender::processMReadR(CDAPMessage* msg) {
    PongMsgg* pong = check_and_cast<PongMsgg*>(msg);
    if(pong){
        received++;
        receivedSize += msg->getByteLength();
        simtime_t delay = simTime() - pong->pingAt;
        if(minDelay>delay){
            minDelay = delay;
        }
        if(maxDelay<delay){
            maxDelay = delay;
        }
        if(firstR<0)  {
            firstR = simTime();
        }
        lastR = simTime();

        if(recTimes){
            double dl = dround(delay.dbl(), 3);
            times[dl]++;
        }
    }
}


double AESender::dround(double a, int ndigits) {

  int    exp_base10 = round(log10(a));
  double man_base10 = a*pow(10.0,-exp_base10);
  double factor     = pow(10.0,-ndigits+1);
  double truncated_man_base10 = man_base10 - fmod(man_base10,factor);
  double rounded_remainder    = fmod(man_base10,factor)/factor;

  rounded_remainder = rounded_remainder > 0.5 ? 1.0*factor : 0.0;

  return (truncated_man_base10 + rounded_remainder)*pow(10.0,exp_base10) ;
}
