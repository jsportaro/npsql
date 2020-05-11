using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal class NqpHeader
    {
        internal NqpMessageType MessageType { get; set; }

        internal int MessageSize { get; set; }
    }
}
