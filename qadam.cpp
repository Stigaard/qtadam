/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Morten Stigaard Laursen <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qadam.h"
#include <iostream>
#include <qt4/QtCore/QByteArray>

QADAM::QADAM(QHostAddress ip): QThread()
{
  this->ip = ip;
  //this->start();
  this->run();
}

void QADAM::run()
{
  this->socket = new QTcpSocket(this);
  this->reconnect();
  if(this->socket->state()!=QAbstractSocket::ConnectedState)
    neverconnected = true;
  else
    neverconnected = false;
//  connect(&(this->connectionStateTimer), SIGNAL(timeout()), this, SLOT(reconnect()));
    connect(&(this->connectionStateTimer), SIGNAL(timeout()), this, SLOT(verifyOutputs()));
  this->connectionStateTimer.start(300);
 // connect(this->socket, SIGNAL(disconnected()), this, SLOT(reconnect()));
//  exec();
}

void QADAM::reconnect(void )
{
    
    if(this->socket->state()==QTcpSocket::UnconnectedState)
      {
      this->socketLock.lock();
      this->socket->connectToHost(this->ip, 502, QIODevice::ReadWrite);
      this->socket->waitForConnected(300);
      this->socketLock.unlock();
      }
    
}


void QADAM::write_bit(quint8 bit, bool data)
{
 // std::cout << "Was ordered to set " << (quint32)bit << " : " << data << std::endl;
  if(neverconnected)
    return;
  this->socketLock.lock();
  const unsigned char Transaction_id[2] = {00, 01};
  const unsigned char protocol_id[2] = {00, 00};
  const unsigned char length_field[2] = {00, 06};
  const unsigned char unit_id = 01;
  const unsigned char function_code = 05; //Write coil
  unsigned char coil_adr[2] = {00, 16+bit};
  unsigned char coil_force_data[2] = {00, 00};
  if(data)
    coil_force_data[0] = 255;
  
  char frame[] = {Transaction_id[0], Transaction_id[1], protocol_id[0], protocol_id[1], length_field[0], length_field[1],
		 unit_id, function_code, coil_adr[0], coil_adr[1], coil_force_data[0], coil_force_data[1]};
		 

//   for(int i=0;i<sizeof(frame);i++)
//     std::cout << std::dec << (unsigned int)frame[i] << " ";
//   std::cout << std::endl;
qint64 bytesWritten;
bool succededWrite;
bool outputStatus;
int attempts = 0;
  do
  {
    if(this->socket->state()==QTcpSocket::UnconnectedState)
      {
	std::cout << "Found in unconnected state!, inline" << std::endl;
	this->socketLock.unlock();
	this->reconnect();
	this->socketLock.lock();
      }
      bytesWritten = this->socket->write(frame, sizeof(frame));
      succededWrite = this->socket->waitForBytesWritten(100);
      this->socketLock.unlock();
      outputStatus = (this->getOutputStatus() & 1<<bit)>0;
      this->socketLock.lock();
      attempts++;
      if(attempts > 10)
	break;
  }while((succededWrite==false) || (bytesWritten!=sizeof(frame)) || (outputStatus!=data));
  //this->socket->waitForReadyRead();
  this->socketLock.unlock();
}

void QADAM::write_byte(quint8 data)
{
  if(neverconnected)
    return;
  this->socketLock.lock();
  const unsigned char Transaction_id[2] = {00, 01};
  const unsigned char protocol_id[2] = {00, 00};
  const unsigned char length_field[2] = {00, 06};
  const unsigned char unit_id = 01;
  const unsigned char function_code = 06; //Preset register
  const unsigned char reg_adr[2] = {0x01, 0x2e};
  unsigned char reg_data[2] = {00, data};
  
  char frame[] = {Transaction_id[0], Transaction_id[1], protocol_id[0], protocol_id[1], length_field[0], length_field[1],
		 unit_id, function_code, reg_adr[0], reg_adr[1], reg_data[0], reg_data[1]};
		 

  for(int i=0;i<sizeof(frame);i++)
    std::cout << std::dec << (unsigned int)frame[i] << " ";
  std::cout << std::endl;
  this->socket->write(frame, sizeof(frame));
  this->socket->waitForBytesWritten();
//  this->socket->waitForReadyRead();
  this->socketLock.unlock();
}

quint8 QADAM::getOutputStatus(void )
{
  if(neverconnected)
    return 0;
  this->socketLock.lock();
  const unsigned char Transaction_id[2] = {00, 01};
  const unsigned char protocol_id[2] = {00, 00};
  const unsigned char length_field[2] = {00, 06};
  const unsigned char unit_id = 01;
  const unsigned char function_code = 03; //Read register
  const unsigned char reg_adr[2] = {0x01, 0x2e};
  unsigned char num_data[2] = {00, 1};
  QDataStream in(socket);
  
  char frame[] = {Transaction_id[0], Transaction_id[1], protocol_id[0], protocol_id[1], length_field[0], length_field[1],
		 unit_id, function_code, reg_adr[0], reg_adr[1], num_data[0], num_data[1]};
		 
  this->socket->write(frame, sizeof(frame));
  this->socket->waitForBytesWritten();
  this->socket->waitForReadyRead();
  int bytesRead = 0;
  
  char data[11];
  int attempts = 0;
  while(this->socket->bytesAvailable()<11)
  {
    this->msleep(10);
    attempts++;
    if(attempts>10)
      break;
  }
  int readSize = this->socket->read(data, 11);
  this->socketLock.unlock();
  return data[10];
}

void QADAM::verifyOutputs(void )
{
  getOutputStatus();
}


quint8 QADAM::getInputState(void )
{
  if(neverconnected)
    return 0;
  this->socketLock.lock();
  const unsigned char Transaction_id[2] = {00, 01};
  const unsigned char protocol_id[2] = {00, 00};
  const unsigned char length_field[2] = {00, 06};
  const unsigned char unit_id = 01;
  const unsigned char function_code = 03; //Read register
  const unsigned char reg_adr[2] = {0x01, 0x2c};
  unsigned char num_data[2] = {00, 1};
  QDataStream in(socket);
  
  char frame[] = {Transaction_id[0], Transaction_id[1], protocol_id[0], protocol_id[1], length_field[0], length_field[1],
		 unit_id, function_code, reg_adr[0], reg_adr[1], num_data[0], num_data[1]};
		 
  this->socket->write(frame, sizeof(frame));
  this->socket->waitForBytesWritten();
  this->socket->waitForReadyRead();
  int bytesRead = 0;
  
  char data[11];
  while(this->socket->bytesAvailable()<11);
  int readSize = this->socket->read(data, 11);
  this->socketLock.unlock();
  return data[10];
}


#include "qadam.moc"
