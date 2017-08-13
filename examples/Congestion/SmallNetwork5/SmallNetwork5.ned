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

package rina.examples.Congestion.SmallNetwork5;
import ned.DatarateChannel;
import rina.src.CS.*;


network SmallNetworkAgg
{
    @display("bgb=672,372");
    double ldelay @unit(s) = default(0.5us);
    double wdelay @unit(s) = default(1.5ms);
    double lber = default(0.0);
    double slowDatarate @unit(bps) = default(3Mbps);
    double normalDatarate @unit(bps) = default(10Mbps);
    int cubicSlowStartMode = default(0);
    double cubicBeta = default(0.2);
    double cubicC = default(0.4);
    int fastAlpha = default(100);
    double fastGamma = default(0.5);
    double fastWUpdateInterval @unit(s) = default(20ms);
    int congDetection = default(0);

    types:
        channel SlowP extends DatarateChannel
        {
            parameters:
                datarate = slowDatarate;
                per = 0;
        }
        channel NormalP extends DatarateChannel
        {
            parameters:
                datarate = normalDatarate;
                per = 0;
        }
        channel HighSpeedP extends DatarateChannel
        {
            parameters:
                datarate = 20Mbps;
                per = 0;
        }
    submodules:
        host11: Host1AP {
            @display("p=115,53");
        }
        host12: Host1AP {
            @display("p=115,119");
        }
        host13: Host1AP {
            @display("p=115,182");
        }
        host14: Host1AP {
            @display("p=115,243");
        }
        host15: Host1AP {
            @display("p=115,311");
        }
        host21: Host1AP {
            @display("p=545,47");
        }
        host22: Host1AP {
            @display("p=545,113");
        }
        host23: Host1AP {
            @display("p=545,182");
        }
        host24: Host1AP {
            @display("p=545,243");
        }
        host25: Host1AP {
            @display("p=545,311");
        }
        router1: InteriorRouterNInt {
            @display("p=213,182");
        }
        router2: InteriorRouterNInt {
            @display("p=438,182");
        }
        router3: InteriorRouter2Int {
            @display("p=322,182");
        }
    connections:
        router1.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host11.medium;
        router1.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host12.medium;
        router1.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host13.medium;
        router1.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host14.medium;
        router1.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host15.medium;
        router1.medium++ <--> NormalP {  delay = wdelay; ber = 0; } <--> router3.medium[0];
        router3.medium[1] <--> NormalP {  delay = wdelay; ber = 0; } <--> router2.medium++;
        router2.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host21.medium;
        router2.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host22.medium;
        router2.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host23.medium;
        router2.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host24.medium;
        router2.medium++ <--> SlowP {  delay = ldelay; ber = 0; } <--> host25.medium;
}
