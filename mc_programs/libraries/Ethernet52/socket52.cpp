#include "w5200.h"
#include "socket52.h"
#include "IPAddress.h"  // might be able to get rid of this
#include "Ethernet52.h"
#include "EthernetServer52.h"  // pretty sure this can go

#include "Arduino.h"

#if ARDUINO >= 156 || TEENSYDUINO >= 120
extern void yield(void);
#else
#define yield()
#endif

// TODO: randomize this when not using DHCP, but how?
static uint16_t local_port = 49152;  // 49152 to 65535

typedef struct {
	uint16_t RX_RSR; // Number of bytes received
	uint16_t RX_RD;  // Address to read
	uint16_t TX_FSR; // Free space ready for transmit
	uint8_t  RX_inc; // how much have we advanced RX_RD
} socketstate_t;

static socketstate_t state[MAX_SOCK_NUM];


static uint16_t getSnTX_FSR(uint8_t s);
static uint16_t getSnRX_RSR(uint8_t s);
static void send_data_processing(uint8_t s, uint16_t offset, const uint8_t *data, uint16_t len);
static void read_data(uint8_t s, uint16_t src, volatile uint8_t *dst, uint16_t len);



/*****************************************/
/*          Socket management            */
/*****************************************/


void socketPortRand(uint16_t n) {
	n &= 0x3FFF;
	local_port ^= n;
	//Serial.printf("socketPortRand %d, srcport=%d\n", n, local_port);
}

uint8_t socketBegin(uint8_t protocol, uint16_t port) {
	uint8_t s, status[MAX_SOCK_NUM];
	
	// EthernetClass52::_server_port[sock] = 0;  // don't know which socket we'll pick, and once we 
	//   do, we'll set it to an actual port number anyways
	
	//Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	// look at all the hardware sockets, use any that are closed (unused)
	for (s=0; s < EthernetClass52::_u8MaxUsedSocks; s++) {
		status[s] = W5200.readSnSR(s);
		if (status[s] == SnSR::CLOSED) goto makesocket;
		// try going to begin with sock parameter after check for closed, otherwise wait to try and 
		//   force close
		// if (status[s] == SnSR::CLOSED) {
			// return socketBegin(protocol, port, s);
		// }
	}
	//Serial.printf("W5000socket step2\n");
	// as a last resort, forcibly close any already closing
	for (s=0; s < EthernetClass52::_u8MaxUsedSocks; s++) {
		uint8_t stat = status[s];
		if (stat == SnSR::LAST_ACK) goto closemakesocket;
		if (stat == SnSR::TIME_WAIT) goto closemakesocket;
		if (stat == SnSR::FIN_WAIT) goto closemakesocket;
		if (stat == SnSR::CLOSING) goto closemakesocket;
		
		// switch (stat) {
			// case SnSR::LAST_ACK:
			// case SnSR::TIME_WAIT:
			// case SnSR::FIN_WAIT:
			// case SnSR::CLOSING:
			  // return socketBegin(protocol, port, s);
				// break;
		// }
	}
#if 0
	Serial.printf("W5000socket step3\n");
	// next, use any that are effectively closed
	for (s=0; s < EthernetClass52::_u8MaxUsedSocks; s++) {
		uint8_t stat = status[s];
		// TODO: this also needs to check if no more data
		if (stat == SnSR::CLOSE_WAIT) goto closemakesocket;
		// if (stat == SnSR::CLOSE_WAIT) {
			// return socketBegin(protocol, port, s);
		// }
	}
#endif
	SPI.endTransaction();
	return MAX_SOCK_NUM; // all sockets are in use
closemakesocket:
	//Serial.printf("W5000socket close\n");
	W5200.execCmdSn(s, Sock_CLOSE);
makesocket:
	//Serial.printf("W5000socket %d\n", s);
	// EthernetServer::server_port[s] = 0;
	delayMicroseconds(250); // TODO: is this needed??
	W5200.writeSnMR(s, protocol);
	W5200.writeSnIR(s, 0xFF);
	if (port > 0) {
		W5200.writeSnPORT(s, port);
		EthernetClass52::_server_port[s] = port;
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		W5200.writeSnPORT(s, local_port);
		EthernetClass52::_server_port[s] = local_port;
	}
	W5200.execCmdSn(s, Sock_OPEN);
	state[s].RX_RSR = 0;
	state[s].RX_RD  = W5200.readSnRX_RD(s); // always zero?
	state[s].RX_inc = 0;
	state[s].TX_FSR = 0;
	//Serial.printf("W5000socket prot=%d, RX_RD=%d\n", W5100.readSnMR(s), state[s].RX_RD);
	SPI.endTransaction();
	return s;
}

uint8_t socketBegin(uint8_t protocol, uint16_t port, uint8_t sock) {
	EthernetClass52::_server_port[sock] = 0;  // shouldn't this be zero iff we could not set up the port?
	
	if (!(sock < EthernetClass52::_u8MaxUsedSocks)) {  // sock is too large for sure
		return MAX_SOCK_NUM;
	}
	
	//Serial.printf("W5000socket begin, protocol=%d, port=%d\n", protocol, port);
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	// close sock if necessary
	uint8_t stat = W5200.readSnSR(sock);
	
	switch (stat) {
		case SnSR::CLOSED:
			break; // exit switch if already closed
		case SnSR::LAST_ACK:
		case SnSR::TIME_WAIT:
		case SnSR::FIN_WAIT:
		case SnSR::CLOSING:
			//Serial.printf("W5000socket close\n");
			W5200.execCmdSn(sock, Sock_CLOSE);
			break;
		case SnSR::CLOSE_WAIT: // check if Rx buffer has contents
			if (socketRecvAvailable(sock) <= 0) {
				//Serial.printf("W5000socket close\n");
				W5200.execCmdSn(sock, Sock_CLOSE);
			}
			else {
				SPI.endTransaction();
				return MAX_SOCK_NUM;
			}
			break;
		default:
			// otherwise return error
			SPI.endTransaction();
			return MAX_SOCK_NUM; // all sockets are in use
	}

	
	delayMicroseconds(250); // TODO: is this needed??
	W5200.writeSnMR(sock, protocol);
	W5200.writeSnIR(sock, 0xFF);
	if (port > 0) {
		W5200.writeSnPORT(sock, port);
		EthernetClass52::_server_port[sock] = port;
	} else {
		// if don't set the source port, set local_port number.
		if (++local_port < 49152) local_port = 49152;
		W5200.writeSnPORT(sock, local_port);
		EthernetClass52::_server_port[sock] = local_port;
	}
	W5200.execCmdSn(sock, Sock_OPEN);
	state[sock].RX_RSR = 0;
	state[sock].RX_RD  = W5200.readSnRX_RD(sock); // always zero?
	state[sock].RX_inc = 0;
	state[sock].TX_FSR = 0;
	//Serial.printf("W5000socket prot=%d, RX_RD=%d\n", W5100.readSnMR(sock), sock.RX_RD);
	SPI.endTransaction();
	return sock;
}


// Return the socket's status
//
uint8_t socketStatus(uint8_t s)
{
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	uint8_t status = W5200.readSnSR(s);
	SPI.endTransaction();
	return status;
}


// Immediately close.  If a TCP connection is established, the
// remote host is left unaware we closed.
//
void socketClose(uint8_t s)
{
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.execCmdSn(s, Sock_CLOSE);
	SPI.endTransaction();
}


// establish a TCP connection in Active (client) mode.
//
void socketConnect(uint8_t s, uint8_t * addr, uint16_t port)
{
	// set destination IP
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.writeSnDIPR(s, addr);
	W5200.writeSnDPORT(s, port);
	W5200.execCmdSn(s, Sock_CONNECT);
	SPI.endTransaction();
}


// Gracefully disconnect a TCP connection.
//
void socketDisconnect(uint8_t s)
{
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.execCmdSn(s, Sock_DISCON);
	SPI.endTransaction();
}


// Place the socket in listening (server) mode
//
uint8_t socketListen(uint8_t s)
{
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	if (W5200.readSnSR(s) != SnSR::INIT) {
		SPI.endTransaction();
		return 0;
	}
	W5200.execCmdSn(s, Sock_LISTEN);
	SPI.endTransaction();
	return 1;
}


// Get the remote port of the socket
// 
uint16_t socketRemotePort(uint8_t s) {
	uint16_t u16_sockPort;
	
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	u16_sockPort = W5200.readSnDPORT(s);
	SPI.endTransaction();
	return u16_sockPort;
}


// get the remote ip of the socket
//
void socketRemoteIP(uint8_t s, uint8_t remoteIP[4]) {
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.readSnDIPR(s, remoteIP);
	SPI.endTransaction();
	return;
}


/*****************************************/
/*    Socket Data Receive Functions      */
/*****************************************/


static uint16_t getSnRX_RSR(uint8_t s) {
#if 1
	uint16_t val, prev;

	prev = W5200.readSnRX_RSR(s);
	while (1) {
		val = W5200.readSnRX_RSR(s);
		if (val == prev) {
			state[s].RX_RSR = val;
			// state[s].RX_RD = W5200.readSnRX_RD(s);  // does this help ??????????????????????????
			return val;
		}
		prev = val;
	}
#else
	uint16_t val = W5200.readSnRX_RSR(s);
	state[s].RX_RSR = val;
	return val;
#endif
}

static void read_data(uint8_t s, uint16_t src, volatile uint8_t *dst, uint16_t len)
{
	uint16_t size;
	uint16_t src_mask;
	uint16_t src_ptr;

	//Serial.printf("read_data, len=%d, at:%d\n", len, src);
	src_mask = (uint16_t)src & W5200.SMASK[s];
	src_ptr = W5200.RBASE[s] + src_mask;

	if ((src_mask + len) > W5200.SSIZE[s]) {
		size = W5200.SSIZE[s] - src_mask;
		W5200.read(src_ptr, (uint8_t *)dst, size);
		dst += size;
		W5200.read(W5200.RBASE[s], (uint8_t *) dst, len - size);
	} else {
		W5200.read(src_ptr, (uint8_t *) dst, len);
	}
}

// Receive data.  Returns size, or -1 for no data, or 0 if connection closed
//
int socketRecv(uint8_t s, uint8_t *buf, int16_t len)
{
	// Check how much data is available
	int ret = state[s].RX_RSR;
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	if (ret < len) {
		ret = getSnRX_RSR(s);
	}
	if (ret == 0) {
		// No data available.
		uint8_t status = W5200.readSnSR(s);
		if ( status == SnSR::LISTEN || status == SnSR::CLOSED ||
		  status == SnSR::CLOSE_WAIT ) {
			// The remote end has closed its side of the connection,
			// so this is the eof state
			ret = 0;
		} else {
			// The connection is still up, but there's no data waiting to be read
			ret = -1;
		}
	} else {
		if (ret > len) ret = len; // more data available than buffer length
		
		uint16_t ptr = state[s].RX_RD;
		read_data(s, ptr, buf, ret);
		ptr += len;
		state[s].RX_RD = ptr;
		state[s].RX_RSR -= ret;
		uint16_t inc = state[s].RX_inc + ret;
		if (inc >= 80 || state[s].RX_RSR == 0) {
			state[s].RX_inc = 0;
			W5200.writeSnRX_RD(s, ptr);
			W5200.execCmdSn(s, Sock_RECV);
			//Serial.printf("Sock_RECV cmd, RX_RD=%d\n", state[s].RX_RD);
		} else {
			state[s].RX_inc = inc;
		}
	}
	SPI.endTransaction();
	return ret;
}

uint16_t socketRecvAvailable(uint8_t s)
{
	uint16_t ret = state[s].RX_RSR;
	if (ret == 0) {
		SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
		ret = getSnRX_RSR(s);
		SPI.endTransaction();
	}
	//Serial.printf("sock.recvAvailable s=%d, num=%d\n", s, ret);
	return ret;
}

// get the first byte in the receive queue (no checking)
//
uint8_t socketPeek(uint8_t s)
{
	uint8_t b;
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	uint16_t ptr = state[s].RX_RD;
	W5200.read((ptr & W5200.SMASK[s]) + W5200.RBASE[s], &b, 1);
	SPI.endTransaction();
	return b;
}



/*****************************************/
/*    Socket Data Transmit Functions     */
/*****************************************/

static uint16_t getSnTX_FSR(uint8_t s){
	uint16_t val, prev;

	prev = W5200.readSnTX_FSR(s);
	while (1) {
		val = W5200.readSnTX_FSR(s);
		if (val == prev) {
			state[s].TX_FSR = val;
			return val;
		}
		prev = val;
	}
}


static void send_data_processing(uint8_t s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
	uint16_t ptr = W5200.readSnTX_WR(s);
	ptr += data_offset;
	uint16_t offset = ptr & W5200.SMASK[s];
	uint16_t dstAddr = offset + W5200.SBASE[s];

	if (offset + len > W5200.SSIZE[s]) {
		// Wrap around circular buffer
		uint16_t size = W5200.SSIZE[s] - offset;
		W5200.write(dstAddr, data, size);
		W5200.write(W5200.SBASE[s], data + size, len - size);
	} else {
		W5200.write(dstAddr, data, len);
	}
	ptr += len;
	W5200.writeSnTX_WR(s, ptr);
}


/**
 * @brief	This function used to send the data in TCP mode
 * @return	1 for success else 0.
 */
uint16_t socketSend(uint8_t s, const uint8_t * buf, uint16_t len)
{
	uint8_t status=0;
	uint16_t ret=0;
	uint16_t freesize=0;

	if (len > W5200.SSIZE[s]) {
		ret = W5200.SSIZE[s]; // check size not to exceed MAX size.
	} else {
		ret = len;
	}

	// if freebuf is available, start.
	do {
		SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
		freesize = getSnTX_FSR(s);
		status = W5200.readSnSR(s);
		SPI.endTransaction();
		if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT)) {
			ret = 0;
			break;
		}
		yield();
	} while (freesize < ret);

	// copy data
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	send_data_processing(s, 0, (uint8_t *)buf, ret);
	W5200.execCmdSn(s, Sock_SEND);

	/* +2008.01 bj */
	while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) {
		/* m2008.01 [bj] : reduce code */
		if ( W5200.readSnSR(s) == SnSR::CLOSED ) {
			SPI.endTransaction();
			return 0;
		}
		SPI.endTransaction();
		yield();
		SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	}
	/* +2008.01 bj */
	W5200.writeSnIR(s, SnIR::SEND_OK);
	SPI.endTransaction();
	return ret;
}


uint16_t socketBufferData(uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len)
{
	//Serial.printf("  bufferData, offset=%d, len=%d\n", offset, len);
	uint16_t ret =0;
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	uint16_t txfree = getSnTX_FSR(s);
	if (len > txfree) {
		ret = txfree; // check size not to exceed MAX size.
	} else {
		ret = len;
	}
	send_data_processing(s, offset, buf, ret);
	SPI.endTransaction();
	return ret;
}

int socketStartUDP(uint8_t s, uint8_t* addr, uint16_t port)
{
	if ( ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
	  ((port == 0x00)) ) {
		return 0;
	}
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.writeSnDIPR(s, addr);
	W5200.writeSnDPORT(s, port);
	SPI.endTransaction();
	return 1;
}

int socketSendUDP(uint8_t s)
{
	SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	W5200.execCmdSn(s, Sock_SEND);

	/* +2008.01 bj */
	while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) {
		if (W5200.readSnIR(s) & SnIR::TIMEOUT) {
			/* +2008.01 [bj]: clear interrupt */
			W5200.writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
			SPI.endTransaction();
			//Serial.printf("sendUDP timeout\n");
			return 0;
		}
		SPI.endTransaction();
		yield();
		SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
	}

	/* +2008.01 bj */
	W5200.writeSnIR(s, SnIR::SEND_OK);
	SPI.endTransaction();

	//Serial.printf("sendUDP ok\n");
	/* Sent ok */
	return 1;
}

// Everything below line is old ####################################################################
// #################################################################################################
// #################################################################################################
// #################################################################################################

// static uint16_t local_port;

// /**
 // * @brief	This Socket function initialize the channel in perticular mode, and set the port and wait for W5200 done it.
 // * @return 	1 for success else 0.
 // */
// uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
// {
  // if ((protocol == SnMR::TCP) || (protocol == SnMR::UDP) || (protocol == SnMR::IPRAW) || (protocol == SnMR::MACRAW) || (protocol == SnMR::PPPOE))
  // {
    // close(s);
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // W5200.writeSnMR(s, protocol | flag);
    // if (port != 0) {
      // W5200.writeSnPORT(s, port);
    // } 
    // else {
      // local_port++; // if don't set the source port, set local_port number.
      // W5200.writeSnPORT(s, local_port);
    // }
    // W5200.execCmdSn(s, Sock_OPEN);
    // SPI.endTransaction();
    // return 1;
  // }

  // return 0;
// }


// uint8_t socketStatus(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // uint8_t status = W5200.readSnSR(s);
  // SPI.endTransaction();
  // return status;
// }


// /**
 // * @brief	This function close the socket and parameter is "s" which represent the socket number
 // */
// void close(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.execCmdSn(s, Sock_CLOSE);
  // W5200.writeSnIR(s, 0xFF);
  // SPI.endTransaction();
// }


// /**
 // * @brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
 // * @return	1 for success else 0.
 // */
// uint8_t listen(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // if (W5200.readSnSR(s) != SnSR::INIT) {
    // SPI.endTransaction();
    // return 0;
  // }
  // W5200.execCmdSn(s, Sock_LISTEN);
  // SPI.endTransaction();
  // return 1;
// }


// /**
 // * @brief	This function established  the connection for the channel in Active (client) mode. 
 // * 		This function waits for the untill the connection is established.
 // * 		
 // * @return	1 for success else 0.
 // */
// uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
// {
  // if 
    // (
  // ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
    // ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    // (port == 0x00) 
    // ) 
    // return 0;

  // // set destination IP
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.writeSnDIPR(s, addr);
  // W5200.writeSnDPORT(s, port);
  // W5200.execCmdSn(s, Sock_CONNECT);
  // SPI.endTransaction();

  // return 1;
// }



// /**
 // * @brief	This function used for disconnect the socket and parameter is "s" which represent the socket number
 // * @return	1 for success else 0.
 // */
// void disconnect(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.execCmdSn(s, Sock_DISCON);
  // SPI.endTransaction();
// }


// /**
 // * @brief	This function used to send the data in TCP mode
 // * @return	1 for success else 0.
 // */
// uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len)
// {
  // uint8_t status=0;
  // uint16_t ret=0;
  // uint16_t freesize=0;

  // if (len > W5200.SSIZE[s]) 
    // ret = W5200.SSIZE[s]; // check size not to exceed MAX size.
  // else 
    // ret = len;

  // // if freebuf is available, start.
  // do 
  // {
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // freesize = W5200.getTXFreeSize(s);
    // status = W5200.readSnSR(s);
    // SPI.endTransaction();
    // if ((status != SnSR::ESTABLISHED) && (status != SnSR::CLOSE_WAIT))
    // {
      // ret = 0; 
      // break;
    // }
    // yield();
  // }
  // while (freesize < ret);

  // // copy data
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.send_data_processing(s, (uint8_t *)buf, ret);
  // W5200.execCmdSn(s, Sock_SEND);

  // /* +2008.01 bj */
  // while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  // {
    // /* m2008.01 [bj] : reduce code */
    // if ( W5200.readSnSR(s) == SnSR::CLOSED )
    // {
      // SPI.endTransaction();
      // close(s);
      // return 0;
    // }
    // SPI.endTransaction();
    // yield();
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // }
  // /* +2008.01 bj */
  // W5200.writeSnIR(s, SnIR::SEND_OK);
  // SPI.endTransaction();
  // return ret;
// }


// /**
 // * @brief	This function is an application I/F function which is used to receive the data in TCP mode.
 // * 		It continues to wait for data as much as the application wants to receive.
 // * 		
 // * @return	received data size for success else -1.
 // */
// int16_t recv(SOCKET s, uint8_t *buf, int16_t len)
// {
  // // Check how much data is available
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // int16_t ret = W5200.getRXReceivedSize(s);
  // if ( ret == 0 )
  // {
	  // ret = 0;  // MJT change, I don't like having to check for -1, nor do I understand why there needs to be
	            // //     a distinction
				
    // // No data available.
    // // uint8_t status = W5200.readSnSR(s);
    // // if ( status == SnSR::LISTEN || status == SnSR::CLOSED || status == SnSR::CLOSE_WAIT )
    // // {
      // // // The remote end has closed its side of the connection, so this is the eof state
      // // ret = 0;
    // // }
    // // else
    // // {
      // // // The connection is still up, but there's no data waiting to be read
      // // ret = -1;
    // // }
  // }
  // else if (ret > len)
  // {
    // ret = len;
  // }

  // if ( ret > 0 )
  // {
    // W5200.recv_data_processing(s, buf, ret);
    // W5200.execCmdSn(s, Sock_RECV);
  // }
  // SPI.endTransaction();
  // return ret;
// }


// int16_t recvAvailable(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // int16_t ret = W5200.getRXReceivedSize(s);
  // SPI.endTransaction();
  // return ret;
// }


// /**
 // * @brief	Returns the first byte in the receive queue (no checking)
 // * 		
 // * @return
 // */
// uint16_t peek(SOCKET s, uint8_t *buf)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.recv_data_processing(s, buf, 1, 1);
  // SPI.endTransaction();
  // return 1;
// }


// /**
 // * @brief	This function is an application I/F function which is used to send the data for other then TCP mode. 
 // * 		Unlike TCP transmission, The peer's destination address and the port is needed.
 // * 		
 // * @return	This function return send data size for success else -1.
 // */
// uint16_t sendto(SOCKET s, const uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t port)
// {
  // uint16_t ret=0;

  // if (len > W5200.SSIZE[s]) ret = W5200.SSIZE[s]; // check size not to exceed MAX size.
  // else ret = len;

  // if
    // (
  // ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
    // ((port == 0x00)) ||(ret == 0)
    // ) 
  // {
    // /* +2008.01 [bj] : added return value */
    // ret = 0;
  // }
  // else
  // {
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // W5200.writeSnDIPR(s, addr);
    // W5200.writeSnDPORT(s, port);

    // // copy data
    // W5200.send_data_processing(s, (uint8_t *)buf, ret);
    // W5200.execCmdSn(s, Sock_SEND);

    // /* +2008.01 bj */
    // while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
    // {
      // if (W5200.readSnIR(s) & SnIR::TIMEOUT)
      // {
        // /* +2008.01 [bj]: clear interrupt */
        // W5200.writeSnIR(s, (SnIR::SEND_OK | SnIR::TIMEOUT)); /* clear SEND_OK & TIMEOUT */
        // SPI.endTransaction();
        // return 0;
      // }
      // SPI.endTransaction();
      // yield();
      // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // }

    // /* +2008.01 bj */
    // W5200.writeSnIR(s, SnIR::SEND_OK);
    // SPI.endTransaction();
  // }
  // return ret;
// }


// /**
 // * @brief	This function is an application I/F function which is used to receive the data in other then
 // * 	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well. 
 // * 	
 // * @return	This function return received data size for success else -1.
 // */
// uint16_t recvfrom(SOCKET s, uint8_t *buf, uint16_t len, uint8_t *addr, uint16_t *port)
// {
  // uint8_t head[8];
  // uint16_t data_len=0;
  // uint16_t ptr=0;

  // if ( len > 0 )
  // {
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // ptr = W5200.readSnRX_RD(s);
    // switch (W5200.readSnMR(s) & 0x07)
    // {
    // case SnMR::UDP :
      // W5200.read_data(s, ptr, head, 0x08);
      // ptr += 8;
      // // read peer's IP address, port number.
      // addr[0] = head[0];
      // addr[1] = head[1];
      // addr[2] = head[2];
      // addr[3] = head[3];
      // *port = head[4];
      // *port = (*port << 8) + head[5];
      // data_len = head[6];
      // data_len = (data_len << 8) + head[7];

      // W5200.read_data(s, ptr, buf, data_len); // data copy.
      // ptr += data_len;

      // W5200.writeSnRX_RD(s, ptr);
      // break;

    // case SnMR::IPRAW :
      // W5200.read_data(s, ptr, head, 0x06);
      // ptr += 6;

      // addr[0] = head[0];
      // addr[1] = head[1];
      // addr[2] = head[2];
      // addr[3] = head[3];
      // data_len = head[4];
      // data_len = (data_len << 8) + head[5];

      // W5200.read_data(s, ptr, buf, data_len); // data copy.
      // ptr += data_len;

      // W5200.writeSnRX_RD(s, ptr);
      // break;

    // case SnMR::MACRAW:
      // W5200.read_data(s,ptr,head,2);
      // ptr+=2;
      // data_len = head[0];
      // data_len = (data_len<<8) + head[1] - 2;

      // W5200.read_data(s,ptr,buf,data_len);
      // ptr += data_len;
      // W5200.writeSnRX_RD(s, ptr);
      // break;

    // default :
      // break;
    // }
    // W5200.execCmdSn(s, Sock_RECV);
    // SPI.endTransaction();
  // }
  // return data_len;
// }


// uint16_t igmpsend(SOCKET s, const uint8_t * buf, uint16_t len)
// {
  // uint16_t ret=0;

  // if (len > W5200.SSIZE[s]) 
    // ret = W5200.SSIZE[s]; // check size not to exceed MAX size.
  // else 
    // ret = len;

  // if (ret == 0)
    // return 0;

  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.send_data_processing(s, (uint8_t *)buf, ret);
  // W5200.execCmdSn(s, Sock_SEND);

  // while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  // {
    // if (W5200.readSnIR(s) & SnIR::TIMEOUT)
    // {
      // /* in case of igmp, if send fails, then socket closed */
      // /* if you want change, remove this code. */
      // SPI.endTransaction();
      // close(s);
      // return 0;
    // }
    // SPI.endTransaction();
    // yield();
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // }

  // W5200.writeSnIR(s, SnIR::SEND_OK);
  // SPI.endTransaction();
  // return ret;
// }

// uint16_t bufferData(SOCKET s, uint16_t offset, const uint8_t* buf, uint16_t len)
// {
  // uint16_t ret =0;
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // if (len > W5200.getTXFreeSize(s))
  // {
    // ret = W5200.getTXFreeSize(s); // check size not to exceed MAX size.
  // }
  // else
  // {
    // ret = len;
  // }
  // W5200.send_data_processing_offset(s, offset, buf, ret);
  // SPI.endTransaction();
  // return ret;
// }

// int startUDP(SOCKET s, uint8_t* addr, uint16_t port)
// {
  // if
    // (
     // ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
     // ((port == 0x00))
    // ) 
  // {
    // return 0;
  // }
  // else
  // {
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    // W5200.writeSnDIPR(s, addr);
    // W5200.writeSnDPORT(s, port);
    // SPI.endTransaction();
    // return 1;
  // }
// }

// int sendUDP(SOCKET s)
// {
  // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // W5200.execCmdSn(s, Sock_SEND);
		
  // /* +2008.01 bj */
  // while ( (W5200.readSnIR(s) & SnIR::SEND_OK) != SnIR::SEND_OK ) 
  // {
    // if (W5200.readSnIR(s) & SnIR::TIMEOUT)
    // {
      // /* +2008.01 [bj]: clear interrupt */
      // W5200.writeSnIR(s, (SnIR::SEND_OK|SnIR::TIMEOUT));
      // SPI.endTransaction();
      // return 0;
    // }
    // SPI.endTransaction();
    // yield();
    // SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  // }

  // /* +2008.01 bj */	
  // W5200.writeSnIR(s, SnIR::SEND_OK);
  // SPI.endTransaction();

  // /* Sent ok */
  // return 1;
// }

