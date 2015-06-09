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

#include <ExternConsts.h>

//Module names
const char* MOD_FLOWALLOC       = "flowAllocator";
const char* MOD_FA              = "fa";
const char* MOD_CONNTABLE       = "connectionTable";
const char* MOD_DA              = "da";
const char* MOD_DIFALLOC        = "difAllocator";
const char* MOD_FAITABLE        = "faiTable";
const char* MOD_EFCP            = "efcp";
const char* MOD_RESALLOC        = "resourceAllocator";
const char* MOD_RA              = "ra";
const char* MOD_IAE             = "iae";
const char* MOD_IRM             = "irm";
const char* MOD_IPCRESMANAGER   = "ipcResourceManager";
const char* MOD_RIBDAEMON       = "ribDaemon";
const char* MOD_RIBD            = "ribd";
const char* MOD_ENROLLMENTMODULE= "enrollmentModule";
const char* MOD_ENROLLMENT      = "enrollment";
const char* MOD_ENROLLMENTTABLE = "enrollmentStateTable";
const char* MOD_CDAP            = "commonDistributedApplicationProtocol";
const char* MOD_CDAPMSGLOG      = "cdapMsgLog";
const char* MOD_CDAPSPLIT       = "cdapSplitter";
const char* MOD_CDAPCACE        = "cace";
const char* MOD_CDAPAUTH        = "auth";
const char* MOD_CDAPCDAP        = "cdap";
const char* MOD_EFCPTABLE       = "efcpTable";
const char* MOD_DTP             = "dtp";
const char* MOD_DTCP            = "dtcp";
const char* MOD_DTP_STATE       = "dtpState";
const char* MOD_DTCP_STATE      = "dtcpState";
const char* MOD_EFCPI           = "efcpi_";

//Module type paths
const char* MOD_EFCP_PATH       = "rina.src.DIF.EFCP";
const char* MOD_EFCPI_PATH      = "rina.src.DIF.EFCP.EFCPI";
const char* MOD_DTP_PATH        = "rina.src.DIF.EFCP.DTP.DTP";
const char* MOD_DTP_STATE_PATH  = "rina.src.DIF.EFCP.DTP.DTPState";
const char* MOD_DTCP_PATH       = "rina.src.DIF.EFCP.DTCP.DTCP";
const char* MOD_DTCP_STATE_PATH = "rina.src.DIF.EFCP.DTCP.DTCPState";
const char* MOD_DELIMITING_PATH = "rina.src.DIF.Delimiting.Delimiting";
const char* MOD_RMT_PORTWRAPPER = "rina.src.DIF.RMT.RMTPortWrapper";
const char* MOD_RMT_QUEUE       = "rina.src.DIF.RMT.RMTQueue";



//Module parameters
const char* PAR_IPCADDR         = "ipcAddress";
const char* PAR_DIFNAME         = "difName";
const char* PAR_LOCALPORTID     = "localPortId";
const char* PAR_LOCALCEPID      = "localCEPId";
const char* PAR_REMOTEPORTID    = "remotePortId";
const char* PAR_REMOTECEPID     = "remoteCEPId";

const char* PAR_APNAME          = "apName";
const char* PAR_APINSTANCE      = "apInstance";
const char* PAR_AENAME          = "aeName";
const char* PAR_AEINSTANCE      = "aeInstance";
const char* PAR_CONFIGDATA      = "configData";
const char* PAR_CREREQTIMEOUT   = "createRequestTimeout";
const char* PAR_CURINVOKEID     = "currentInvokeId";

//Gate names
const char* GATE_SOUTHIO        = "southIo";
const char* GATE_NORTHIO        = "northIo";
const char* GATE_NORTHIO_       = "northIo_";
const char* GATE_SOUTHIO_       = "southIo_";
const char* GATE_EFCPIO_        = "efcpIo_";
const char* GATE_EFCPIO         = "efcpIo";
const char* GATE_RMT_           = "rmt_";
const char* GATE_RMTIO          = "rmtIo";
const char* GATE_APPIO_         = "appIo_";
const char* GATE_AEIO           = "aeIo";
const char* GATE_DATAIO         = "dataIo";
const char* GATE_SPLITIO        = "splitterIo";
const char* GATE_CACEIO         = "caceIo";
const char* GATE_AUTHIO         = "authIo";
const char* GATE_CDAPIO         = "cdapIo";
const char* GATE_DTP_NORTHIO    = "northIo";
const char* GATE_DTP_SOUTHIO    = "southIo";
const char* GATE_EFCPI_NORTHIO  = "northIo";
const char* GATE_EFCPI_SOUTHIO  = "southIo";

//XML attributes and elements
const char* ELEM_APN            = "APN";
const char* ATTR_APN            = "apn";
const char* ELEM_AVGBW               = "AverageBandwidth";
const char* ELEM_AVGSDUBW            = "AverageSDUBandwidth";
const char* ELEM_PEAKBWDUR           = "PeakBandwidthDuration";
const char* ELEM_PEAKSDUBWDUR        = "PeakSDUBandwidthDuration";
const char* ELEM_BURSTPERIOD         = "BurstPeriod";
const char* ELEM_BURSTDURATION       = "BurstDuration";
const char* ELEM_UNDETECTBITERR      = "UndetectedBitError";
const char* ELEM_PDUDROPPROBAB       = "PDUDroppingProbability";
const char* ELEM_MAXSDUSIZE          = "MaxSDUSize";
const char* ELEM_PARTIALDELIVER      = "PartialDelivery";
const char* ELEM_INCOMPLETEDELIVER   = "IncompleteDelivery";
const char* ELEM_FORCEORDER          = "ForceOrder";
const char* ELEM_MAXALLOWGAP         = "MaxAllowableGap";
const char* ELEM_DELAY               = "Delay";
const char* ELEM_JITTER              = "Jitter";
const char* ELEM_COSTTIME            = "CostTime";
const char* ELEM_COSTBITS            = "CostBits";
const char* ELEM_ATIME               = "ATime";
const char* ELEM_RXON                = "RxOn";
const char* ELEM_WINON               = "WinOn";
const char* ELEM_RATEON              = "RateOn";
const char* ELEM_EFCPPOL             = "EFCPPolicySet";


//Values
//TODO: Vesely @Marek - What about -1 as default DoNotCare value
//TODO: Marek @Vesely - OK
const int   VAL_QOSPARDONOTCARE = -1;
const bool  VAL_QOSPARDEFBOOL   = false;
const int   UNINIT_INVOKEID     = -1;
const int   DONTCARE_INVOKEID   = 0;
//const int   VAL_UNDEF_PORTID   = -1;
