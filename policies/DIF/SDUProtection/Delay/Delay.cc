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

#include <Delay.h>

Register_Class(Delay);

void Delay::onPolicyInit(){
    inDelayTime= par("inDelayTime").doubleValue();
    outDelayTime = par("outDelayTime").doubleValue();
    inDelay = par("inDelay").boolValue();
    outDelay = par("outDelay").boolValue();
}

void Delay::processPDU(cMessage* msg)
{
  double delay = 0;
  cPacket* packet = dynamic_cast<cPacket*>(msg);

  if (!opp_strcmp(msg->getArrivalGate()->getName(), "protect$i"))
    {
      delay = inDelayTime * packet->getByteLength()/900;


      sendDelayed(msg, delay, gate("protect$o"));
    }
    else if (!opp_strcmp(msg->getArrivalGate()->getName(), "unprotect$i"))
    {

        delay = outDelayTime * packet->getByteLength()/900;
      sendDelayed(msg, delay, gate("unprotect$o"));
    }
}