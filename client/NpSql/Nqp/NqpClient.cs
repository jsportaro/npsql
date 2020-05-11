using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace NpSql.Nqp
{
    internal sealed class NqpClient : IDisposable
    {
        private bool disposed = false;
        private TcpClient client;
        private IPAddress[] addresses;
        private int port;
        private NetworkStream stream;

        private NetworkStream Stream 
        {
            get
            {
                if (stream == null)
                {
                    string hostname = string.Empty;

                    if (addresses != null)
                        client.Connect(addresses, port);
                    else
                        client.Connect(hostname, port);

                    stream = client.GetStream();
                }

                return stream;
            }
            set
            {
                stream = value;
            }
        }


        internal Guid ConnectionId { get; private set; }
        internal short MaxMessageSize { get; private set; }
        internal bool HasActiveReader { get; private set; }

        internal NqpClient(IPAddress[] addresses, int port)
        {
            client = new TcpClient();

            this.addresses = addresses;
            this.port = port;
        }

        internal NqpClient(string hostName, int port)
        {
            client = new TcpClient();

            this.port = port;
        }

        internal bool Hello()
        {
            DisposeGuard();
            
            SendHello();
            var message = new MessageReader(Stream);

            if (message.MessageType == NqpMessageType.Welcome)
            {
                MaxMessageSize = message.ReadShort();

                return true;
            }
            else if (message.MessageType == NqpMessageType.Sorry)
            {
                return false;
            }
            else
            {
                throw new ProtocolViolationException($"All the sudden {Enum.GetName(typeof(NqpMessageType), message.MessageType)}");
            }
        }

        private void SendHello()
        {
            var session_id = Guid.NewGuid();
            var id_bytes = session_id.ToByteArray();
            var hello = new List<byte>();

            var helloLength = BitConverter.GetBytes((short)id_bytes.Length);

            hello.Add((byte)NqpMessageType.Hello);
            hello.AddRange(helloLength);
            hello.AddRange(id_bytes);

            Stream.Write(hello.ToArray(), 0, hello.Count);

            ConnectionId = session_id;
        }

        internal QueryResults Query(string sql)
        {
            DisposeGuard();

            var sqlBytes = Encoding.UTF8.GetBytes(sql);
            var queryMessageHeader = new List<byte>();

            queryMessageHeader.Add((byte)NqpMessageType.Query);
            queryMessageHeader.AddRange(BitConverter.GetBytes((short)sqlBytes.Length));
            queryMessageHeader.AddRange(sqlBytes);

            Stream.Write(queryMessageHeader.ToArray(), 0, queryMessageHeader.Count);

            return new QueryResults(stream).ProcessNextMessage();
        }

        internal void Goodbye()
        {
            DisposeGuard();
            SendGoodbye();
        }

        private void SendGoodbye()
        {
            byte[] goodbye = new byte[] { (byte)NqpMessageType.Goodbye, 0x00, 0x00 };
            Stream.Write(goodbye, 0, goodbye.Length);
        }

        public void Dispose()
        {
            if (Stream != null)
            {
                Stream.Close();
                Stream = null;
            }

            if (client != null)
            {
                if (client.Connected)
                {
                    client.Close();
                }
                client = null;
            }

            disposed = true;
        }

        private void DisposeGuard()
        {
            if (disposed)
                throw new ObjectDisposedException(nameof(NqpClient));
        }
    }
}
