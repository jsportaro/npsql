using NpSql;
using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal class NqpParser
    {
        private readonly Stream stream;


        internal NqpParser(Stream stream)
        {
            this.stream = stream;
        }

        internal object GetResponse()
        {
            Parse();


            return null;
        }

        private void Parse()
        {
            if (!stream.CanRead)
                throw new InvalidOperationException("Couldn't read from stream");

            var header = GetHeader();
        }

        private NqpHeader GetHeader()
        {
            //  Every message must be at least 5 bytes or death
            var headerByteCount = 5;
            var headerBytes = new byte[headerByteCount];

            if (headerByteCount != stream.Read(headerBytes, 0, headerBytes.Length))
            {
                throw new InvalidOperationException("Message must be at least 5 bytes");
            }

            return new NqpHeader()
            {
                MessageType = (NqpMessageType)headerBytes[0],
                MessageSize = BitConverter.ToInt32(headerBytes, 1)
            };
        }
    }
}
