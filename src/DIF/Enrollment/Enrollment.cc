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
 * @file Enrollment.cc
 * @author Kamil Jerabek (xjerab18@stud.fit.vutbr.cz)
 * @date Apr 1, 2015
 * @brief Enrollment and CACE
 * @detail
 */


#include "Enrollment.h"

Define_Module(Enrollment);

const char* PAR_AUTH_TYPE       = "authType";
const char* PAR_AUTH_NAME       = "authName";
const char* PAR_AUTH_OTHER      = "authOther";
const char* PAR_AUTH_PASS       = "authPassword";
const char* PAR_CON_RETRIES     = "maxConRetries";

const char* MSG_CONREQ                = "Connect/Auth";
const char* MSG_CONREQRETRY           = "ConnectRetry/Auth";
const char* MSG_CONRESPOS             = "Connect+/Auth";
const char* MSG_CONRESNEG             = "Connect-/Auth";

Enrollment::Enrollment() :
        StateTable(NULL), ribd(NULL)
{
}

Enrollment::~Enrollment(){
    StateTable = NULL;
    ribd = NULL;
}

void Enrollment::initialize()
{
    initSignalsAndListeners();
    initPointers();

    authType = par(PAR_AUTH_TYPE);
    authName = this->par(PAR_AUTH_NAME).stringValue();
    authPassword = this->par(PAR_AUTH_PASS).stringValue();
    authOther = this->par(PAR_AUTH_OTHER).stringValue();

    maxConRetries = this->par(PAR_CON_RETRIES);
}

void Enrollment::initPointers(){
    StateTable = dynamic_cast<EnrollmentStateTable*>(this->getParentModule()->getSubmodule(MOD_ENROLLMENTTABLE));
    ribd = dynamic_cast<RIBd*>(this->getParentModule()->getParentModule()->getSubmodule(MOD_RIBDAEMON)->getSubmodule(MOD_RIBD));
}

void Enrollment::initSignalsAndListeners() {
    cModule* catcher1 = this->getParentModule()->getParentModule();

    sigEnrollmentCACESendData   = registerSignal(SIG_ENROLLMENT_CACEDataSend);
    sigEnrollmentSendData       = registerSignal(SIG_ENROLLMENT_DataSend);
    sigEnrollmentStartEnrollReq = registerSignal(SIG_ENROLLMENT_StartEnrollmentRequest);
    sigEnrollmentStartEnrollRes = registerSignal(SIG_ENROLLMENT_StartEnrollmentResponse);
    sigEnrollmentStopEnrollReq  = registerSignal(SIG_ENROLLMENT_StopEnrollmentRequest);
    sigEnrollmentStopEnrollRes  = registerSignal(SIG_ENROLLMENT_StopEnrollmentResponse);
    sigEnrollmentStartOperReq   = registerSignal(SIG_ENROLLMENT_StartOperationRequest);
    sigEnrollmentStartOperRes   = registerSignal(SIG_ENROLLMENT_StartOperationResponse);
    sigEnrollmentFinish         = registerSignal(SIG_ENROLLMENT_Finished);

    lisEnrollmentAllResPosi = new LisEnrollmentAllResPosi(this);
    catcher1->subscribe(SIG_FAI_AllocateFinishManagement, lisEnrollmentAllResPosi);

    lisEnrollmentGetFlowFromFaiCreResPosi = new LisEnrollmentGetFlowFromFaiCreResPosi(this);
    catcher1->subscribe(SIG_FAI_CreateFlowResponsePositive, lisEnrollmentGetFlowFromFaiCreResPosi);

    lisEnrollmentStartEnrollReq = new LisEnrollmentStartEnrollReq(this);
    catcher1->subscribe(SIG_RIBD_StartEnrollmentRequest, lisEnrollmentStartEnrollReq);

    lisEnrollmentStartEnrollRes = new LisEnrollmentStartEnrollRes(this);
    catcher1->subscribe(SIG_RIBD_StartEnrollmentResponse, lisEnrollmentStartEnrollRes);

    lisEnrollmentStopEnrollReq = new LisEnrollmentStopEnrollReq(this);
    catcher1->subscribe(SIG_RIBD_StopEnrollmentRequest, lisEnrollmentStopEnrollReq);

    lisEnrollmentStopEnrollRes = new LisEnrollmentStopEnrollRes(this);
    catcher1->subscribe(SIG_RIBD_StopEnrollmentResponse, lisEnrollmentStopEnrollRes);

    lisEnrollmentStartOperationReq = new LisEnrollmentStopOperationReq(this);
    catcher1->subscribe(SIG_RIBD_StartOperationRequest, lisEnrollmentStartOperationReq);

    lisEnrollmentStartOperationRes = new LisEnrollmentStartOperationRes(this);
    catcher1->subscribe(SIG_RIBD_StartOperationResponse, lisEnrollmentStartOperationRes);

    lisEnrollmentConResPosi = new LisEnrollmentConResPosi(this);
    catcher1->subscribe(SIG_RIBD_ConnectionResponsePositive, lisEnrollmentConResPosi);

    lisEnrollmentConResNega = new LisEnrollmentConResNega(this);
    catcher1->subscribe(SIG_RIBD_ConnectionResponseNegative, lisEnrollmentConResNega);

    lisEnrollmentConReq = new LisEnrollmentConReq(this);
    catcher1->subscribe(SIG_RIBD_ConnectionRequest, lisEnrollmentConReq);
}

void Enrollment::startCACE(Flow* flow) {
    Enter_Method("startCACE()");

    auto entry = EnrollmentStateTableEntry(flow, EnrollmentStateTableEntry::CON_AUTHENTICATING, true);
    StateTable->insert(entry);

    CDAP_M_Connect* msg = new CDAP_M_Connect(MSG_CONREQ);

    authValue_t aValue;
    aValue.authName = authName;
    aValue.authPassword = authPassword;
    aValue.authOther = authOther;

    auth_t auth;
    auth.authType = authType;
    auth.authValue = aValue;

    msg->setAuth(auth);
    msg->setAbsSyntax(GPB);
    msg->setOpCode(M_CONNECT);

    APNamingInfo src = APNamingInfo(entry.getLocal().getApn(),
                entry.getLocal().getApinstance(),
                entry.getLocal().getAename(),
                entry.getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry.getRemote().getApn(),
            entry.getRemote().getApinstance(),
            entry.getRemote().getAename(),
            entry.getRemote().getAeinstance());
    /*
     * XXX: Vesely@Jerabek> Removing unnecessary *.msg ADT when there exists
     *                      exactly the same ADT in RINASim source codes.
    naming_t dst;
    dst.AEInst = entry.getRemote().getAeinstance();
    dst.AEName = entry.getRemote().getAename();
    dst.ApInst = entry.getRemote().getApinstance();
    dst.ApName = entry.getRemote().getApn().getName();

    naming_t src;
    src.AEInst = entry.getLocal().getAeinstance();
    src.AEName = entry.getLocal().getAename();
    src.ApInst = entry.getLocal().getApinstance();
    src.ApName = entry.getLocal().getApn().getName();
    */

    msg->setDst(dst);
    msg->setSrc(src);

    msg->setSrcAddr(Address(entry.getLocal().getApn()));
    msg->setDstAddr(Address(entry.getRemote().getApn()));

    //send data to ribd to send
    signalizeCACESendData(msg);
}

void Enrollment::insertStateTableEntry(Flow* flow){
    //insert only first flow created (management flow)
    if(StateTable->findEntryByDstAPN(APN(flow->getDstAddr().getApname().getName().c_str())) != NULL) {
        return;
    }

    StateTable->insert(EnrollmentStateTableEntry(flow, EnrollmentStateTableEntry::CON_CONNECTPENDING, false));
}

void Enrollment::receivePositiveConnectResponse(CDAPMessage* msg) {
    Enter_Method("receivePositiveConnectResponse()");

    CDAP_M_Connect_R* cmsg = check_and_cast<CDAP_M_Connect_R*>(msg);
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_AUTHENTICATING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_ESTABLISHED);

    startEnrollment(entry);
}

void Enrollment::receiveNegativeConnectResponse(CDAPMessage* msg) {
    Enter_Method("receiveNegativeConnectResponse()");

    CDAP_M_Connect_R* cmsg = check_and_cast<CDAP_M_Connect_R*>(msg);
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_AUTHENTICATING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    if (this->maxConRetries <= entry->getCurrentConnectRetries()) {
        entry->setCACEConStatus(EnrollmentStateTableEntry::CON_NIL);
        //TODO: send release and deallocate
        return;
    }


    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_CONNECTPENDING);
    entry->increaseCurrentConnectRetries();
    //create and send new connect retry
    processNewConReq(entry);
}

void Enrollment::receiveConnectRequest(CDAPMessage* msg) {
    Enter_Method("receiveConnectRequest()");

    CDAP_M_Connect* cmsg = check_and_cast<CDAP_M_Connect*>(msg);

    auto ent = EnrollmentStateTableEntry(
            cmsg->getDst(), cmsg->getSrc(), EnrollmentStateTableEntry::CON_CONNECTPENDING, false);
    StateTable->insert(ent);

    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(cmsg->getSrc().getApn());

    if (!entry) {
        EV << "Enrollment status not found for "
           << cmsg->getSrc().getApn() << endl;
        return;
    }

    //check appropriate state
    if (entry->getCACEConStatus() != EnrollmentStateTableEntry::CON_CONNECTPENDING) {
        //TODO: send M_Release and invoke deallocate
        return;
    }

    //check if message is valid
    if (cmsg->getAbsSyntax() != GPB ||
            cmsg->getOpCode() != M_CONNECT) {
        this->processConResNega(entry, cmsg);
        return;
    }

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_AUTHENTICATING);

    authenticate(entry, cmsg);
}

/*   enrollment initiator */

void Enrollment::startEnrollment(EnrollmentStateTableEntry* entry) {
    Enter_Method("startEnrollment()");

    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    enrollObj->setAddress(APN(ribd->getMyAddress().getIpcAddress().getName()));

    //TODO: add other necessary information

    //process enrollment object to ribd to send
    signalizeStartEnrollmentRequest(enrollObj);

    //set appropriate state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_RESPONSE_ENROLLMENT);
}

void Enrollment::receiveStartEnrollmentResponse(CDAPMessage* msg) {
    Enter_Method("receiveStartEnrollmentResponse()");

    CDAP_M_Start_R* smsg = check_and_cast<CDAP_M_Start_R*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = (check_and_cast<EnrollmentObj*>(smsg->getObject().objectVal))->dup();
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(APN(enrollRec->getSrcAddress().getApname().getName().c_str()));

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_START_RESPONSE_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    //assign new, received address
    Address newAddr = ribd->getMyAddress();
    newAddr.setIpcAddress(APN(enrollRec->getAddress().getName().c_str()));
    ribd->setMyAddress(newAddr);

    //TODO: assign other received values

    //change state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_STOP_ENROLLMENT);

    //TODO: wait for create messages and stop enrollment request
}

void Enrollment::receiveStopEnrollmentRequest(CDAPMessage* msg) {
    Enter_Method("receiveStopEnrollmentRequest()");


    CDAP_M_Stop* smsg = check_and_cast<CDAP_M_Stop*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = (check_and_cast<EnrollmentObj*>(smsg->getObject().objectVal))->dup();
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(APN(enrollRec->getSrcAddress().getApname().getName().c_str()));

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_STOP_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    //set immediate transition to operational state
    entry->setIsImmediateEnrollment(enrollRec->getIsImmediate());

    //set appropriate state
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_READ_RESPONSE);

    //TODO: send read requests, wait for responses, send Mstop enrollment
    //for now send stop enrollment response
    processStopEnrollmentResponse(entry);
}

void Enrollment::processStopEnrollmentResponse(EnrollmentStateTableEntry* entry) {

    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    signalizeStopEnrollmentResponse(enrollObj);

    if (entry->getIsImmediateEnrollment()) {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_ENROLLED);
        signalizeEnrollmentFinished(entry);
    }
    else {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_OPERATION);
        //TODO: continue enrollment here
    }
}

void Enrollment::receiveStartOperationRequest(CDAPMessage* msg) {
    Enter_Method("receiveStartOperationRequest()");

}

/* enrollment member */

void Enrollment::receiveStartEnrollmentRequest(CDAPMessage* msg) {
    Enter_Method("receiveStartEnrollmentRequest()");

    CDAP_M_Start* smsg = check_and_cast<CDAP_M_Start*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = (check_and_cast<EnrollmentObj*>(smsg->getObject().objectVal))->dup();
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(APN(enrollRec->getSrcAddress().getApname().getName().c_str()));

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_START_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }


    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    //TODO: repair this dummy address assign
    enrollObj->setAddress(APN(enrollRec->getAddress().getName()));

    //TODO: add other necessary information

    //process enrollment object to ribd to send
    signalizeStartEnrollmentResponse(enrollObj);

    //TODO: send create messages, wait for responses, then send stop enrollment
    //for now send stop enrollment
    processStopEnrollmentImmediate(entry);
}

void Enrollment::receiveStopEnrollmentResponse(CDAPMessage* msg) {
    Enter_Method("receiveStopEnrollmentResponse()");

    CDAP_M_Stop_R* smsg = check_and_cast<CDAP_M_Stop_R*>(msg);

    //not expected message
    if (!smsg) {
        //TODO: send release and deallocate
        return;
    }

    EnrollmentObj* enrollRec = (check_and_cast<EnrollmentObj*>(smsg->getObject().objectVal))->dup();
    EnrollmentStateTableEntry* entry = StateTable->findEntryByDstAPN(APN(enrollRec->getSrcAddress().getApname().getName().c_str()));

    //check for appropriate state
    if (entry->getEnrollmentStatus() != EnrollmentStateTableEntry::ENROLL_WAIT_STOP_RESPONSE_ENROLLMENT) {
        //TODO: send release and deallocate
        return;
    }

    if (entry->getIsImmediateEnrollment()) {
        entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_ENROLLED);
        //TODO: emit signal somewhere and probably send rib update ...
    }
    else {
        //TODO: add appropriate state for read and wait operation
    }
}

void Enrollment::receiveStartOperationResponse(CDAPMessage* msg) {
    Enter_Method("receiveStartOperationResponse()");

}

void Enrollment::processStopEnrollmentImmediate(EnrollmentStateTableEntry* entry) {

    auto enrollObj = new EnrollmentObj(Address(entry->getLocal().getApn()), Address(entry->getRemote().getApn()));

    //set immediate
    enrollObj->setIsImmediate(true);
    entry->setIsImmediateEnrollment(true);

    //TODO: add other necessary information

    signalizeStopEnrollmentRequest(enrollObj);

    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_STOP_RESPONSE_ENROLLMENT);
}


void Enrollment::authenticate(EnrollmentStateTableEntry* entry, CDAP_M_Connect* msg) {
    Enter_Method("authenticate()");

    //check and validate expected auth type
    if (msg->getAuth().authType == authType) {
        //none auth option
        if (msg->getAuth().authType == AUTH_NONE) {
            processConResPosi(entry, msg);
            return;

        }
        //passwd auth option
        else if (msg->getAuth().authType == AUTH_PASSWD) {
            //correct passwd
            if (!strcmp(msg->getAuth().authValue.authPassword.c_str(), authPassword.c_str())) {
                processConResPosi(entry, msg);
                return;
            }
        }
        //TODO: support for other options
    }

    //not valid auth send negative response
    processConResNega(entry, msg);
}

void Enrollment::processNewConReq(EnrollmentStateTableEntry* entry) {
    Enter_Method("processNewConReq()");

    //TODO: probably change values, this is retry

    CDAP_M_Connect* msg = new CDAP_M_Connect(MSG_CONREQRETRY);

    authValue_t aValue;
    aValue.authName = authName;
    aValue.authPassword = authPassword;
    aValue.authOther = authOther;

    auth_t auth;
    auth.authType = authType;
    auth.authValue = aValue;

    msg->setAuth(auth);
    msg->setAbsSyntax(GPB);
    msg->setOpCode(M_CONNECT);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    msg->setDst(dst);
    msg->setSrc(src);

    msg->setDstAddr(Address(entry->getRemote().getApn()));

    //send data to ribd to send
    signalizeCACESendData(msg);

    //change state to auth after send retry
    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_AUTHENTICATING);
}

void Enrollment::processConResPosi(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg) {
    CDAP_M_Connect_R* msg = new CDAP_M_Connect_R(MSG_CONRESPOS);
    CDAP_M_Connect* cmsg1 = check_and_cast<CDAP_M_Connect*>(cmsg);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    result_t result;
    result.resultValue = R_SUCCESS;

    auth_t auth;
    auth.authType = cmsg1->getAuth().authType;
    auth.authValue = cmsg1->getAuth().authValue;

    msg->setAbsSyntax(GPB);
    msg->setOpCode(M_CONNECT_R);
    msg->setResult(result);
    msg->setAuth(auth);
    msg->setDst(dst);
    msg->setSrc(src);

    msg->setDstAddr(Address(entry->getRemote().getApn()));

    //send data to ribd to send
    signalizeCACESendData(msg);

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_ESTABLISHED);
    entry->setEnrollmentStatus(EnrollmentStateTableEntry::ENROLL_WAIT_START_ENROLLMENT);
}

void Enrollment::processConResNega(EnrollmentStateTableEntry* entry, CDAPMessage* cmsg) {
    CDAP_M_Connect_R* msg = new CDAP_M_Connect_R(MSG_CONRESNEG);
    CDAP_M_Connect* cmsg1 = check_and_cast<CDAP_M_Connect*>(cmsg);

    APNamingInfo src = APNamingInfo(entry->getLocal().getApn(),
                entry->getLocal().getApinstance(),
                entry->getLocal().getAename(),
                entry->getLocal().getAeinstance());

    APNamingInfo dst = APNamingInfo(entry->getRemote().getApn(),
            entry->getRemote().getApinstance(),
            entry->getRemote().getAename(),
            entry->getRemote().getAeinstance());

    result_t result;
    result.resultValue = R_FAIL;

    auth_t auth;
    auth.authType = cmsg1->getAuth().authType;
    auth.authValue = cmsg1->getAuth().authValue;

    msg->setAbsSyntax(GPB);
    msg->setOpCode(M_CONNECT_R);
    msg->setResult(result);
    msg->setAuth(auth);
    msg->setDst(dst);
    msg->setSrc(src);

    msg->setDstAddr(Address(entry->getRemote().getApn()));

    //send data to send to ribd
    signalizeCACESendData(msg);

    entry->setCACEConStatus(EnrollmentStateTableEntry::CON_CONNECTPENDING);

    //increase number of connects
    entry->increaseCurrentConnectRetries();
}

void Enrollment::signalizeCACESendData(CDAPMessage* cmsg) {
    emit(sigEnrollmentCACESendData, cmsg);
}

void Enrollment::signalizeStartEnrollmentRequest(EnrollmentObj* obj) {
    emit(sigEnrollmentStartEnrollReq, obj);
}

void Enrollment::signalizeStartEnrollmentResponse(EnrollmentObj* obj) {
    emit(sigEnrollmentStartEnrollRes, obj);
}

void Enrollment::signalizeStopEnrollmentRequest(EnrollmentObj* obj) {
    emit(sigEnrollmentStopEnrollReq, obj);
}

void Enrollment::signalizeStopEnrollmentResponse(EnrollmentObj* obj) {
    emit(sigEnrollmentStopEnrollRes, obj);
}

void Enrollment::signalizeStartOperationRequest(OperationObj* obj) {
    emit(sigEnrollmentStartOperReq, obj);
}

void Enrollment::signalizeStartOperationResponse(OperationObj* obj) {
    emit(sigEnrollmentStartOperRes, obj);
}

void Enrollment::signalizeEnrollmentFinished(EnrollmentStateTableEntry* entry) {
    emit(sigEnrollmentFinish, entry->getFlow());
}

void Enrollment::handleMessage(cMessage *msg)
{

}
