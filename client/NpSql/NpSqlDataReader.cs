using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Data;
using System.Text;

namespace NpSql
{
    public class NpSqlDataReader : IDataReader
    {
        private NpSqlCommand command;
        private NpSqlConnection connection;
        private bool disposed = false;
        private QueryResults queryResults;

        public object this[int i] => throw new NotImplementedException();

        public object this[string name] => throw new NotImplementedException();

        public int Depth => throw new NotImplementedException();

        public bool IsClosed => throw new NotImplementedException();

        public int RecordsAffected
        {
            get
            {
                return 0;
            }
        }

        public int FieldCount => queryResults.FieldCount;

        internal NpSqlDataReader(NpSqlCommand command, NpSqlConnection connection)
        {
            this.command = command;
            this.connection = connection;
        }

        internal NpSqlDataReader(QueryResults queryResults)
        {
            this.queryResults = queryResults;
        }

        public void Close()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public bool GetBoolean(int i)
        {
            throw new NotSupportedException();
        }

        public byte GetByte(int i)
        {
            throw new NotSupportedException();
        }

        public long GetBytes(int i, long fieldOffset, byte[] buffer, int bufferoffset, int length)
        {
            throw new NotSupportedException();
        }

        public char GetChar(int i)
        {
            throw new NotImplementedException();
        }

        public long GetChars(int i, long fieldoffset, char[] buffer, int bufferoffset, int length)
        {

            throw new NotImplementedException();
        }

        public IDataReader GetData(int i)
        {
            throw new NotSupportedException();
        }

        public string GetDataTypeName(int i)
        {
            throw new NotSupportedException();
        }

        public DateTime GetDateTime(int i)
        {
            throw new NotSupportedException();
        }

        public decimal GetDecimal(int i)
        {
            throw new NotSupportedException();
        }

        public double GetDouble(int i)
        {
            throw new NotSupportedException();
        }

        public Type GetFieldType(int i)
        {
            throw new NotSupportedException();
        }

        public float GetFloat(int i)
        {
            throw new NotSupportedException();
        }

        public Guid GetGuid(int i)
        {
            throw new NotSupportedException();
        }

        public short GetInt16(int i)
        {
            throw new NotSupportedException();
        }

        public int GetInt32(int i)
        {
            return queryResults.GetValue<int>(i);
        }

        public long GetInt64(int i)
        {
            throw new NotSupportedException();
        }

        public string GetName(int i)
        {
            throw new NotImplementedException();
        }

        public int GetOrdinal(string name)
        {
            throw new NotImplementedException();
        }

        public DataTable GetSchemaTable()
        {
            throw new NotSupportedException();
        }

        public string GetString(int i)
        {
            return queryResults.GetValue<string>(i).TrimEnd('\0');
        }

        public object GetValue(int i)
        {
            throw new NotImplementedException();
        }

        public int GetValues(object[] values)
        {
            throw new NotImplementedException();
        }

        public bool IsDBNull(int i)
        {
            throw new NotImplementedException();
        }

        public bool NextResult()
        {
            return queryResults.NextResult();
        }

        public bool Read()
        {
            return queryResults.NextRow();
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

        private void DisposeGuard()
        {
            if (disposed)
                throw new ObjectDisposedException(nameof(NpSqlConnection));
        }
    }
}
