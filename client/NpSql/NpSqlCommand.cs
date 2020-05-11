using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Text;

namespace NpSql
{
    class NpSqlCommand : IDbCommand
    {
        private bool disposed = false;
        private NpSqlConnection connection;

        public string CommandText { get; set; }
        public int CommandTimeout { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        public CommandType CommandType { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        public IDbConnection Connection { get => connection; set => connection = value as NpSqlConnection; }
        public IDataParameterCollection Parameters => throw new NotImplementedException();
        public IDbTransaction Transaction { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }
        public UpdateRowSource UpdatedRowSource { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public NpSqlCommand(NpSqlConnection connection)
        {
            Connection = connection;
        }

        public void Cancel()
        {
            throw new NotImplementedException();
        }

        public IDbDataParameter CreateParameter()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public int ExecuteNonQuery()
        {
            QueryResults results = RunQuery();

            throw new NotImplementedException();
        }

        public IDataReader ExecuteReader()
        {
            return new NpSqlDataReader(RunQuery());
        }

        public IDataReader ExecuteReader(CommandBehavior behavior)
        {
            throw new NotImplementedException();
        }

        public object ExecuteScalar()
        {
            throw new NotImplementedException();
        }

        public void Prepare()
        {
            throw new NotImplementedException();
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
            }

            //  Unmanaged things here

            disposed = true;
        }

        private QueryResults RunQuery()
        {
            if (connection.Client.HasActiveReader)
            {
                throw new InvalidOperationException("Already has an active DataReader.  Close that first.");
            }

            return connection.Client.Query(CommandText);
        }

        private void DisposeGuard()
        {
            if (disposed)
                throw new ObjectDisposedException(nameof(NpSqlConnection));
        }
    }
}
