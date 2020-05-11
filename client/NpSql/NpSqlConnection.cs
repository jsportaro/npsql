using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Data;
using System.Net;
using System.Text;

namespace NpSql
{
    public class NpSqlConnection : IDbConnection, IDisposable
    {
        private readonly ConnectionString connectionString;
        private bool disposed = false;

        public string ConnectionString
        {
            get => connectionString.BaseString;
            set => throw new NotImplementedException();
        }

        public int ConnectionTimeout => connectionString.ConnectionTimeout;
        public string Database { get; private set; }
        public ConnectionState State { get; private set; }
        public Guid ConnectionId { get => Client.ConnectionId; }
        internal NqpClient Client { get; private set; }

        public NpSqlConnection(string connectionString)
        {
            this.connectionString = new ConnectionString(connectionString);
            State = ConnectionState.Closed;
            Database = "unknown";
        }

        public IDbTransaction BeginTransaction()
        {
            DisposeGuard();

            throw new NotImplementedException();
        }

        public IDbTransaction BeginTransaction(IsolationLevel il)
        {
            DisposeGuard();

            throw new NotImplementedException();
        }

        public void ChangeDatabase(string databaseName)
        {
            DisposeGuard();

            throw new NotSupportedException("gpsql only suppors one database.");
        }

        public void Close()
        {
            DisposeGuard();

            DisposeClient();
        }

        public IDbCommand CreateCommand()
        {
            DisposeGuard();

            return new NpSqlCommand(this);
        }


        public void Open()
        {
            DisposeGuard();

            IPAddress hostAddress;

            if (!IPAddress.TryParse(connectionString.Host, out hostAddress))
            {
                Client = new NqpClient(Dns.GetHostAddresses(connectionString.Host), connectionString.Port);
            }
            else
            {
                Client = new NqpClient(connectionString.Host, connectionString.Port);
            }

            if (Client.Hello())
            {
                State = ConnectionState.Open;
            }
            else
            {
                throw new NpSqlException($"When connecting to {connectionString.Host}:{connectionString.Port}");
            }
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                DisposeClient();
            }

            //  Unmanaged things here

            disposed = true;
        }

        private void DisposeClient()
        {
            if (Client != null)
            {
                Client.Goodbye();
                Client.Dispose();
                Client = null;

                State = ConnectionState.Closed;
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }


        private void DisposeGuard()
        {
            if (disposed)
                throw new ObjectDisposedException(nameof(NpSqlConnection));
        }
    }
}
