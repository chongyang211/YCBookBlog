// pkg/transport/state.go - TCP 11 态枚举
//
// 第 3 次会话 Step 6.1
//
// RFC793 定义的 11 个状态:
//
//                               +---------+ ---------\      active OPEN
//                               |  CLOSED |            \    -----------
//                               +---------+<---------\   \   create TCB
//                                 |     ^              \   \  snd SYN
//                    passive OPEN |     |   CLOSE        \   \
//                    ------------ |     | ----------       \   \
//                     create TCB  |     | delete TCB         \   \
//                                 V     |                      \   \
//                               +---------+            CLOSE    |    \
//                               |  LISTEN |          ---------- |     |
//                               +---------+          delete TCB |     |
//                    rcv SYN      |     |     SEND              |     |
//                   -----------   |     |    -------            |     V
//  +---------+      snd SYN,ACK  /       \   snd SYN          +---------+
//  |         |<-----------------           ------------------>|         |
//  |   SYN   |                    rcv SYN                     |   SYN   |
//  |   RCVD  |<-----------------------------------------------|   SENT  |
//  |         |                    snd ACK                     |         |
//  |         |------------------           -------------------|         |
//  +---------+   rcv ACK of SYN  \       /  rcv SYN,ACK       +---------+
//    |           --------------   |     |   -----------
//    |                  x         |     |     snd ACK
//    |                            V     V
//    |  CLOSE                   +---------+
//    | -------                  |  ESTAB  |
//    | snd FIN                  +---------+
//    |                   CLOSE    |     |    rcv FIN
//    V                  -------   |     |    -------
//  +---------+          snd FIN  /       \   snd ACK          +---------+
//  |  FIN    |<-----------------           ------------------>|  CLOSE  |
//  | WAIT-1  |------------------                              |   WAIT  |
//  +---------+          rcv FIN  \                            +---------+
//    | rcv ACK of FIN   -------   |                            CLOSE  |
//    | --------------   snd ACK   |                           ------- |
//    V        x                   V                           snd FIN V
//  +---------+                  +---------+                   +---------+
//  |FINWAIT-2|                  | CLOSING |                   | LAST-ACK|
//  +---------+                  +---------+                   +---------+
//    |                rcv ACK of FIN |                 rcv ACK of FIN |
//    |  rcv FIN       -------------- |    Timeout=2MSL   ------------ |
//    |  -------              x       V    ------------        x       V
//     \ snd ACK                 +---------+delete TCB         +---------+
//      ------------------------>|TIME WAIT|------------------>| CLOSED  |
//                               +---------+                   +---------+
package transport

// TCPState TCP 11 态
type TCPState int

const (
	StateClosed      TCPState = iota // 0
	StateListen                      // 1
	StateSynSent                     // 2
	StateSynRcvd                     // 3
	StateEstablished                 // 4
	StateFinWait1                    // 5
	StateFinWait2                    // 6
	StateCloseWait                   // 7
	StateClosing                     // 8
	StateLastAck                     // 9
	StateTimeWait                    // 10
	// StateClosed 已包含,共 11 个
)

var stateNames = [...]string{
	"CLOSED", "LISTEN", "SYN_SENT", "SYN_RCVD", "ESTABLISHED",
	"FIN_WAIT_1", "FIN_WAIT_2", "CLOSE_WAIT", "CLOSING",
	"LAST_ACK", "TIME_WAIT",
}

func (s TCPState) String() string {
	if int(s) >= 0 && int(s) < len(stateNames) {
		return stateNames[s]
	}
	return "UNKNOWN"
}
