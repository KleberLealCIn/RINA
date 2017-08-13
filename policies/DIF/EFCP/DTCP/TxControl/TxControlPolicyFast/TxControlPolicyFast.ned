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

package rina.policies.DIF.EFCP.DTCP.TxControl.TxControlPolicyFast;
import rina.policies.DIF.EFCP.DTCP.TxControl.IntTxControlPolicy;

simple TxControlPolicyFast like IntTxControlPolicy
{
    parameters:
        @display("i=block/socket");
        
        @signal[W_Fast](type=unsigned long);
        @statistic[cwnd](title="W Vegas CWND"; source=W_Fast; record=last,vector,mean);
        
        @signal[FLIGHT_SIZE](type=unsigned long);
        @signal[PKTSENT](type=unsigned long);

        
        @statistic[Flight-Size](title="Flight Size"; source=FLIGHT_SIZE; record=vector);
        @statistic[PktSent](title="Packets sent"; source=PKTSENT; record=vector);
        
        @signal[ecn_counter](type=unsigned long);
        @statistic[ecn-received](title="Congestion notification counter"; source=ecn_counter; record=vector); 
        
        @signal[rtt_base](type=double);
        @statistic[rtt-base](title="RTTBase"; source=rtt_base; record=vector);
        
        string policyName = "TxControlPolicyFast";
        int packetSize = default(1500);
        int initialSendCredit = default(4);
        int minWmax = default(4);

}
