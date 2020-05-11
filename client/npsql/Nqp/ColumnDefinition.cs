using System;
using System.Collections.Generic;
using System.Text;

namespace NpSql.Nqp
{
    internal class ColumnDefinition
    {
        public string Name { get; set; }
        public NqpTypes Type { get; set; }
        public short Size { get; set; }
        public short Offset { get; set; }
    }

    internal class ColumnDefinition<T> : ColumnDefinition
    {
        public T CurrentValue;
    }
}
