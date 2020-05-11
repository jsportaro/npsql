using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace NpSql.Tests.Nqp
{
    internal sealed class TestNpServer : IDisposable
    {
        private List<NqpConnection> connections = new List<NqpConnection>();
        private bool keepListening = true;
        private readonly IPEndPoint endpoint;
        private TcpListener tcpListener;
        private Thread requestThread;

        public bool ClientConnected { get; set; }
        public string ConnectionString { get => $"Host=localhost;Port={endpoint.Port}"; }
        public Setup Setup { get; } = new Setup();

        public TestNpServer()
        {

            endpoint           = new IPEndPoint(IPAddress.Any, 0);
            tcpListener        = new TcpListener(endpoint);
            requestThread      = new Thread(new ThreadStart(requestLoop)) { IsBackground = true };
            requestThread.Name = "NPQ Endpoint Listener";
            ClientConnected    = false;

            tcpListener.Start();

            endpoint = (IPEndPoint)tcpListener.LocalEndpoint;
            requestThread.Start();
        }

        public bool HasClientDisconnected(Guid clientId, TimeSpan timeout)
        {
            return false;
        }

        public void connectionWasClosed(object connection, EventArgs eventArgs)
        {
            var closedConnection = (NqpConnection)connection;

            connections.Remove(closedConnection);
        }

        public void Dispose()
        {
            var connectionsToStop = new List<NqpConnection>(connections.ToArray());

            foreach (var connection in connectionsToStop)
            {
                connection.Stop();
            }

            connections.Clear();

            if (requestThread != null)
            {
                keepListening = false;
                requestThread.Join();
            }

            if (tcpListener != null)
            {
                tcpListener.Stop();
                tcpListener = null;
            }
        }

        public string GetLastQuery(Guid connectionId)
        {
            return connections.First(c => c.ConnectionId == connectionId).LastQuery;
        }

        private void requestLoop()
        {
            while (keepListening)
            {
                if (tcpListener.Pending())
                {
                    var connection = new NqpConnection(tcpListener.AcceptTcpClient(), this);

                    connection.OnConnectionClosed += connectionWasClosed;
                    connections.Add(connection);
                    connection.Start();
                }
                else
                {
                    Thread.Sleep(500);  // Give the CPU a break
                }
            }
        }
    }
}
