using System;
using System.Collections.Generic;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal interface IYieldSet
    {
        int RowCount { get; }

        byte[] GetInt(int ordinal);

        byte[] GetChar(int length, int ordinal);
    }
}
