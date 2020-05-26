using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Data;
using System.Text;

namespace NpSql
{
    public class NpSqlColumnDefinition
    {
        public string Name { get; set; }
        public NqpTypes Type { get; set; }
        public short Size { get; set; }
        public short Offset { get; set; }
    }

    public class NpSqlColumnDefinition<T> : NpSqlColumnDefinition
    {
        public T CurrentValue;
    }
}
