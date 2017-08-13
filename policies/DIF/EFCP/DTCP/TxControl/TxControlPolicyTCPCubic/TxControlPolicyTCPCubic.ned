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

package rina.policies.DIF.EFCP.DTCP.TxControl.TxControlPolicyTCPCubic;

import rina.policies.DIF.EFCP.DTCP.TxControl.IntTxControlPolicy;


//
// TODO auto-generated type
//
simple TxControlPolicyTCPCubic like IntTxControlPolicy
{
    parameters:
        string policyName = "TxControl CUBIC";
        @display("i=block/socket");

        @signal[cwnd](type=unsigned long);
        @statistic[cwnd](title="Congestion window CWND"; source=cwnd; record=vector);

        @signal[delay](type=double);
        @statistic[delay](title="Flight Size"; source=delay; record=vector);
        
		@signal[ecn_counter](type=unsigned long);
        @statistic[ecn-received](title="Congestion notification counter"; source=ecn_counter; record=vector);   
}
