using System;
using System.Collections.Generic;
using System.Text;

namespace NpSql.Nqp
{
    [Flags]
    internal enum NqpMessageType : byte
    {
        //  Session Management
        Hello            = 1,
        Welcome          = 2,
        Sorry            = 3,
        Goodbye          = 4,
        ComeBackSoon     = 5,
                           
        //  Query Handling
        Query            = 6,
        ColumnDefinition = 7,
        RowSet           = 8,
        Completed        = 9,
        Ready            = 10
    }
}
