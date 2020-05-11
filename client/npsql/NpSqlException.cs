using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Text;

namespace NpSql
{
    public class NpSqlException : Exception
    {
        public NpSqlException()
        {
        }

        public NpSqlException(string message) : base(message)
        {
        }

        public NpSqlException(string message, Exception innerException) : base(message, innerException)
        {
        }

        protected NpSqlException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
        }
    }
}
