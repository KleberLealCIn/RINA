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

#ifndef DIF_EFCP_DTCP_ECNSLOWDOWN_ECNSLOWDOWNPOLICYFAST_ECNSLOWDOWNPOLICYFAST_H_
#define DIF_EFCP_DTCP_ECNSLOWDOWN_ECNSLOWDOWNPOLICYFAST_ECNSLOWDOWNPOLICYFAST_H_

#include "DIF/EFCP/DTCP/ECNSlowDown/ECNSlowDownPolicyBase.h"
#include "DIF/EFCP/DTCP/ECNSlowDown/ECNSlowDownPolicyFast/ECNSlowDownPolicyFast.h"
#include "DIF/EFCP/DTCP/TxControl/TxControlPolicyFast/TxControlPolicyFast.h"

class ECNSlowDownPolicyFast: public virtual ECNSlowDownPolicyBase {
public:
    ECNSlowDownPolicyFast();
    virtual ~ECNSlowDownPolicyFast();
    virtual bool run(DTPState* dtpState, DTCPState* dtcpState);

protected:
    virtual void initialize();
    TxControlPolicyFast *txPolicy;
};

#endif /* DIF_EFCP_DTCP_ECNSLOWDOWN_ECNSLOWDOWNPOLICYFAST_ECNSLOWDOWNPOLICYFAST_H_ */
