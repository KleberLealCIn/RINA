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

#include <ECNSlowDownPolicyFast/ECNSlowDownPolicyFast.h>
Register_Class(ECNSlowDownPolicyFast);

ECNSlowDownPolicyFast::ECNSlowDownPolicyFast() {
    // TODO Auto-generated constructor stub

}

ECNSlowDownPolicyFast::~ECNSlowDownPolicyFast() {
    // TODO Auto-generated destructor stub
}

void ECNSlowDownPolicyFast::initialize() {
    txPolicy = getRINAModule<TxControlPolicyFast*>(this, 1, {"txControlPolicy"});
}

bool ECNSlowDownPolicyFast::run(DTPState* dtpState, DTCPState* dtcpState) {
    Enter_Method("TxControlPolicyFast");

    txPolicy->onECNSlowDown();

    return true;
}

