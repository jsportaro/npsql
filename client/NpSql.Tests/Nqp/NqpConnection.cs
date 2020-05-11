using Microsoft.VisualStudio.TestPlatform.ObjectModel.Client;
using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace NpSql.Tests.Nqp
{
    public delegate void ConnectionClosedEventHandler(object sender, EventArgs e);

    internal class NqpConnection
    {
        const int HeaderSize = 3;
        private TcpClient connection;
        private NetworkStream stream;
        private bool stop = false;
        private Task processorTask;
        private TestNpServer server;
        private static Random random = new Random();
        private byte[] requestBytes;

        public string LastQuery { get; private set; }

        public Guid ConnectionId { get; private set; }

        public bool ClientConnected { get; private set; }

        public NqpConnection(TcpClient clientConnection, TestNpServer server)
        {
            this.connection = clientConnection;
            this.server     = server;
        }

        public event ConnectionClosedEventHandler OnConnectionClosed;

        public void Start()
        {
            processorTask =  Task.Run(() => {
                stream = connection.GetStream();

                while (!stop && connection.Connected)
                {
                    if (stream.DataAvailable)
                    {
                        ServiceRequests();
                    }

                    Thread.Sleep(500);
                }

                OnConnectionClosed?.Invoke(this, null);
            });
        }

        public void Stop()
        {
            stop = true;

            if (processorTask != null)
            {
                // Wait for termination
                processorTask.Wait();
            }
        }

        private void ServiceRequests()
        {
            byte[] bytes = new byte[HeaderSize];

            // Read can return anything from 0 to numBytesToRead. 
            // This method blocks until at least one byte is read.
            if (stream.Read(bytes, 0, HeaderSize) != HeaderSize)
            {
                throw new InvalidOperationException("Header was the wrong size");
            }

            var gqpMessageType = (NqpMessageType)bytes[0];
            var requestSize = BitConverter.ToInt16(bytes, 1);
            var requestBytes = new byte[requestSize];
                
            if (requestSize > 0 && stream.Read(requestBytes, 0, requestSize) != requestSize)
            {
                throw new InvalidOperationException("Request was the wrong size");
            }

            switch (gqpMessageType)
            {
                case NqpMessageType.Hello:
                    HandleHello(stream, requestBytes);
                    break;
                case NqpMessageType.Query:
                    HandleQuery(requestBytes, requestSize);
                    break;
                case NqpMessageType.Goodbye:
                    KillClient();
                    ClientConnected = false;
                    break;
            }
        }

        private void HandleQuery(byte[] requestBytes, short requestSize)
        {
            LastQuery = Encoding.UTF8.GetString(requestBytes);

            foreach (var querySetup in server.Setup.QueryResults)
            {
                SendColumnDefinition(querySetup);
                SendRowSet(querySetup);
                SendComplete();
            }

            SendReady();
        }

        private void SendReady()
        {
            var ready = new byte[] { 0x0A, 0x00, 0x00 };

            stream.Write(ready, 0, ready.Length);
        }

        private void SendComplete()
        {
            var completed = new byte[] { 0x09, 0x03, 0x00, 0x01, 0x00, 0x00 };

            stream.Write(completed, 0, completed.Length);
        }

        private void SendRowSet(QuerySetup querySetup)
        {
            if (querySetup.RowCount > 0)
            {
                var rowBytesAvailable = server.Setup.MaxMessageSize - HeaderSize;
                var rowsPerMessage = Math.Floor((double)rowBytesAvailable / querySetup.RowLength);

                var rowsToSend = querySetup.RowCount;

                var yieldSetEnum = querySetup.YieldSets.GetEnumerator();
                yieldSetEnum.MoveNext();
                var currentYieldSet = yieldSetEnum.Current;
                var yieldSetRowsToSend = currentYieldSet.RowCount;

                var s = 0;
                while (rowsToSend > 0)
                {
                    var rowSet = new List<byte>(new byte[] { 0x08, 0x00, 0x00 });

                    for (int i = 0; i < rowsPerMessage && rowsToSend > 0; i++, rowsToSend--, yieldSetRowsToSend--)
                    {
                        if (yieldSetRowsToSend == 0)
                        {
                            yieldSetEnum.MoveNext();
                            currentYieldSet = yieldSetEnum.Current;
                            yieldSetRowsToSend = currentYieldSet.RowCount;
                        }

                        for (int ordinal = 0; ordinal < querySetup.Schema.Count(); ordinal++)
                        {
                            var column = querySetup.Schema[ordinal];

                            if (column.type == NqpTypes.Int)
                            {
                                rowSet.AddRange(currentYieldSet.GetInt(ordinal));
                            }
                            else if (column.type == NqpTypes.Char)
                            {
                                rowSet.AddRange(currentYieldSet.GetChar(column.length, ordinal));
                            }
                        }

                        s++;
                    }

                    var length = BitConverter.GetBytes((short)rowSet.Count - 3);
                    var bytes = rowSet.ToArray();

                    bytes[1] = length[0];
                    bytes[2] = length[1];

                    stream.Write(bytes, 0, bytes.Length);
                }
            }
        }

        private void SendColumnDefinition(QuerySetup querySetup)
        {
            List<byte> columnDefinition = new List<byte>();

            columnDefinition.AddRange(new byte[] { 0x07, 0x00, 0x00 });

            foreach (var column in querySetup.Schema)
            {
                columnDefinition.Add((byte)column.type);
                columnDefinition.AddRange(BitConverter.GetBytes((short)column.length));
                columnDefinition.AddRange(BitConverter.GetBytes((short)column.name.Length));
                columnDefinition.AddRange(Encoding.UTF8.GetBytes(column.name));
            }

            var length = BitConverter.GetBytes((short)columnDefinition.Count - 3);
            var bytes = columnDefinition.ToArray();

            bytes[1] = length[0];
            bytes[2] = length[1];

            stream.Write(bytes, 0, bytes.Length);
        }

        private void HandleHello(Stream stream, byte[] requestBytes)
        {
            ConnectionId = new Guid(requestBytes);

            var maxMessageSizeBytes = BitConverter.GetBytes(server.Setup.MaxMessageSize);

            if (server.Setup.SaySorry)
            {
                var sorry = new byte[] { (byte)NqpMessageType.Sorry, 0, 0 };

                stream.Write(sorry, 0, sorry.Length);

                ClientConnected = false;
            }
            else
            {
                var message = new List<byte>();
                var body = new List<byte>();

                body.AddRange(maxMessageSizeBytes);

                message.Add((byte)NqpMessageType.Welcome);
                message.AddRange(BitConverter.GetBytes((short)maxMessageSizeBytes.Length));
                message.AddRange(body);
                stream.Write(message.ToArray(), 0, message.Count);

                ClientConnected = true;
            }
        }

        private void KillClient()
        {
            stop = true;

            if (stream != null)
            {
                stream.Close();
                stream.Dispose();
                stream = null;
            }

            if (connection != null)
            {
                connection.Dispose();
                connection = null;
            }
        }

        
        public static byte [] RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            var fullLength = new string(Enumerable.Repeat(chars, length)
              .Select(s => s[random.Next(s.Length)]).ToArray());
            var subLength = random.Next(length);
            var stringBytes = new List<byte>(Encoding.UTF8.GetBytes(fullLength.Substring(0, subLength)));
            var padding = Enumerable.Repeat((byte)0, length - subLength);
            var finalProduct = stringBytes.Concat(padding).ToArray();
            var peakAString = Encoding.UTF8.GetString(finalProduct);

            return finalProduct;
        }
    }
}
