#include "Connector.h"

void Connector::login(string user, string pass) {
	this->user = user;
	this->pass = pass;
}

void Connector::Send(const myString& data)
{
	conn.Send((char*)&(data.n), sizeof(int), 0);
	conn.Send(data.s, data.n, 0);
}

myString Connector::Receive()
{
	conn.Receive((char*)&buf.n, sizeof(int), 0);
	conn.Receive(buf.s, buf.n, 0);
	return buf;
}

Connector::Connector() {
	logined = false;

}

void Connector::Close() {
	logined = false;
	delete buf.s;
	conn.Close();
}
