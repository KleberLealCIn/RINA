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

#ifndef DIF_EFCP_DTCP_SENDERACK_SENDERACKPOLICYFAST_SENDERACKPOLICYFAST_H_
#define DIF_EFCP_DTCP_SENDERACK_SENDERACKPOLICYFAST_SENDERACKPOLICYFAST_H_

#include <SenderAckPolicyBase.h>
#include "DIF/EFCP/DTCP/TxControl/TxControlPolicyFast/TxControlPolicyFast.h"

class SenderAckPolicyFast: public virtual SenderAckPolicyBase {
public:
    SenderAckPolicyFast();
    virtual ~SenderAckPolicyFast();
    virtual void initialize();
    virtual bool run(DTPState* dtpState, DTCPState* dtcpState);

protected:
    TxControlPolicyFast *txPolicy;
};

#endif /* DIF_EFCP_DTCP_SENDERACK_SENDERACKPOLICYFAST_SENDERACKPOLICYFAST_H_ */
