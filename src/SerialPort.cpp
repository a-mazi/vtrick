/* Copyright © Artur Maziarek MMXX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#include <fcntl.h>
#include <unistd.h>
#include <SerialPort.h>
#include <Log.h>
#include <termios.h>

// LCOV_EXCL_START
SerialPort::SerialPort() :
  serialPortName{},
  serialPortFD{0}
{
}

SerialPort::SerialPort(std::string serialPortName_) :
  serialPortName{serialPortName_},
  serialPortFD{0}
{
}

void SerialPort::openSerial()
{
  serialPortFD = open(serialPortName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
}

void SerialPort::configSerial()
{
  ::termios tty{};
  if (::tcgetattr(serialPortFD, &tty) != 0)
  {
    return;
  }

  if (::cfsetispeed(&tty, B4800) != 0)
  {
    return;
  }
  if (::cfsetospeed(&tty, B4800) != 0)
  {
    return;
  }

  tty.c_cc[VEOF]  = 26; // CTRL-Z
  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = 0;

  tty.c_iflag |= IGNBRK | INPCK;
  tty.c_iflag &= ~(BRKINT | IGNPAR | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IUCLC | IXON | IXANY | IXOFF | IMAXBEL | IUTF8);

  tty.c_oflag &= ~(OPOST | OLCUC | ONLCR | OCRNL | ONOCR | ONLRET | OFILL | OFDEL | VTDLY | VT0 | VT1);
#if defined __USE_MISC || defined __USE_XOPEN
  tty.c_oflag &= ~(NLDLY | NL0 | NL1 | CRDLY | CR0 | CR1 | CR2 | CR3 | TABDLY | TAB0 | TAB1 | TAB2 | TAB3 | BSDLY | BS0 | BS1 | FFDLY | FF0 | FF1);
#endif
#ifdef __USE_MISC
  tty.c_oflag &= ~(XTABS);
#endif

  tty.c_lflag |= NOFLSH | ECHOE | ECHOK;
  tty.c_lflag &= ~(ISIG | ICANON | ECHO | ECHONL | TOSTOP | IEXTEN);
#if defined __USE_MISC || (defined __USE_XOPEN && !defined __USE_XOPEN2K)
  tty.c_lflag &= ~(XCASE);
#endif
#ifdef __USE_MISC
  tty.c_lflag |= ECHOCTL | ECHOKE;
  tty.c_lflag &= ~(ECHOPRT | FLUSHO | PENDIN | EXTPROC);
#endif

//  tty.c_cflag = 03004774;
  tty.c_cflag |= CLOCAL | CSTOPB | CREAD | PARENB | CS8;
  tty.c_cflag &= ~(PARODD | HUPCL);
#ifdef __USE_MISC
  tty.c_cflag &= ~(ADDRB);
#endif

  ::tcsetattr(serialPortFD, TCSANOW, &tty);
}

void SerialPort::closeSerial()
{
  close(serialPortFD);
}

ssize_t SerialPort::readSerial(void* buffer, size_t dataSize)
{
  ssize_t readSize = read(serialPortFD, buffer, dataSize);
  LOGD("SerialPort::readSerial: Read %ld data bytes of %ld\n", readSize, dataSize);
  for (int bufferIndex = 0; (readSize > 0) && (bufferIndex < readSize); bufferIndex++)
  {
    LOGD("SerialPort::readSerial: 0x%02x\n", static_cast<uint8_t*>(buffer)[bufferIndex] & 0xFF);
  }
  return readSize;
}

ssize_t SerialPort::writeSerial(const void* buffer, size_t dataSize)
{
  ssize_t writeSize = write(serialPortFD, buffer, dataSize);
  LOGD("SerialPort::writeSerial: Write %ld data bytes of %ld\n", writeSize, dataSize);
  for (int bufferIndex = 0; (writeSize > 0) && (bufferIndex < writeSize); bufferIndex++)
  {
    LOGD("SerialPort::writeSerial: 0x%02x\n", static_cast<const uint8_t*>(buffer)[bufferIndex] & 0xFF);
  }
  return writeSize;
}
// LCOV_EXCL_STOP
