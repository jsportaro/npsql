using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;

namespace NpSql.Nqp
{
    internal class MessageReader
    {
        const int HeaderLength = 3;

        private short bytesRead = 0;
        private byte[] payload;

        public NqpMessageType MessageType { get; private set; }

        public short Size { get; private set; }
        public bool ReadToEnd { get => payload == null || bytesRead == payload.Length; }
        public bool IsAtBodyStart { get => bytesRead == 0; }

        public MessageReader(Stream stream)
        {
            var header = new byte[HeaderLength];

            stream.Read(header, 0, HeaderLength);

            MessageType = (NqpMessageType)header[0];
            Size = BitConverter.ToInt16(header, 1);

            if (Size > 0)
            {
                payload = new byte[Size];
                stream.Read(payload, 0, Size);
            }
        }

        public short ReadShort()
        {
            if (ReadToEnd)
            {
                throw new ProtocolViolationException();
            }

            var s = BitConverter.ToInt16(payload, bytesRead);

            bytesRead += sizeof(short);

            return s;
        }

        public int ReadInt()
        {
            if (ReadToEnd)
            {
                throw new ProtocolViolationException();
            }

            var i = BitConverter.ToInt32(payload, bytesRead);

            bytesRead += sizeof(int);

            return i;
        }

        public byte ReadByte()
        {
            if (ReadToEnd)
            {
                throw new ProtocolViolationException();
            }

            var b = payload[bytesRead];

            bytesRead += sizeof(byte);

            return b;
        }

        public string ReadString(short length)
        {
            var preReadBytes = bytesRead;

            bytesRead += length;

            return Encoding.UTF8.GetString(payload.Skip(preReadBytes).Take(length).ToArray());
        }
    }
}
