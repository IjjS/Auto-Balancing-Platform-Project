#include "serialcomm.h"


CSerialComm::CSerialComm() {}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(char* portNum)
{
	if (!serial.OpenPort(portNum))
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT); //baud rate 115200 and byte size 8
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //set all 0 timeouts for real time communication

	return RETURN_SUCCESS;
}


int CSerialComm::sendCommand(char pos)
{
	if (serial.WriteByte(pos))
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

/* Incomplete method. need to work for reading string
char CSerialComm::readCommand()
{
	BYTE res;
	if (!serial.ReadByte(res)) {
		return RETURN_FAIL;
	}
	return res;
}*/

void CSerialComm::disconnect()
{
	serial.ClosePort();
}