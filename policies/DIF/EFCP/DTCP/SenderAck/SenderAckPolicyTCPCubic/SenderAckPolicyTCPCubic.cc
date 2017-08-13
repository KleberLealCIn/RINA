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

#include <SenderAckPolicyTCPCubic/SenderAckPolicyTCPCubic.h>
Register_Class(SenderAckPolicyTCPCubic);

SenderAckPolicyTCPCubic::SenderAckPolicyTCPCubic() {
    // TODO Auto-generated constructor stub

}

SenderAckPolicyTCPCubic::~SenderAckPolicyTCPCubic() {
    // TODO Auto-generated destructor stub
}

void SenderAckPolicyTCPCubic::initialize() {
    txPolicy = getRINAModule<TxControlPolicyTCPCubic*>(this, 1, {"txControlPolicy"});
}

bool SenderAckPolicyTCPCubic::run(DTPState* dtpState, DTCPState* dtcpState) {
    Enter_Method("SenderAckPolicyTCPCubic");

    txPolicy->onEachAck(dtpState);

    defaultAction(dtpState, dtcpState);

    return false;
}

