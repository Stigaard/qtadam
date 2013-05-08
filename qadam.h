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


#ifndef QADAM_H
#define QADAM_H

#include <qt4/QtCore/QThread>
#include <qt4/QtNetwork/QTcpSocket>
#include <qt4/QtNetwork/QHostAddress>

class QADAM : public QThread
{
  Q_OBJECT
public:
    QADAM(QHostAddress ip);
    quint8 getOutputStatus(void);
    quint8 getInputState(void);
public slots:
  void write_bit(quint8 bit, bool data);
  void write_byte(quint8 data);
private:
  QTcpSocket *socket;
  QHostAddress ip;
};

#endif // QADAM_H
