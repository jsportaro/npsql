using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;

namespace NpSql.Nqp
{
    [Flags]
    internal enum MessageStatus
    {
        IsLastMessage = 1,
        ContainsColumnDefs = 2,
    }

    internal class QueryResultParser
    {
        private BinaryReader reader;
        private NetworkStream stream;
        private List<ColumnDefinition> columns = new List<ColumnDefinition>();
        private int totalSize;
        private MessageStatus status;

        public string QueryText { get; private set; }

        public bool LastMessage { get; private set; }

        public string Message { get; private set; }

        public QueryResultParser(short maxMessageSize, string sql, NetworkStream stream)
        {
            QueryText = sql;
            this.stream = stream;
            reader = new BinaryReader(stream);
            ReadHeader();
        }

        private void ReadHeader()
        {
            var b = reader.ReadByte();
            if (b != (byte)NqpMessageType.Hello)
            {
                return;
            }

            totalSize = reader.ReadInt16();

            GetMessageIfExists();

            status = (MessageStatus)reader.ReadByte();
            ParseColumnDataIfExists(status);
            CheckIfLastMessage();
        }

        public bool Read()
        {
            if (!columns.Any())
            {
                throw new InvalidDataException("Query response did not have expected column definitions");
            }

            //if (!stream.DataAvailable)
            //{
            //    return false;
            //}

            //foreach (var column in columns)
            //{
            //    switch (column.Type)
            //    {
            //        case GqpTypes.Int:
            //            ((ColumnDefinition<int>)column).CurrentValue = reader.ReadInt32();
            //            break;
            //        case GqpTypes.Char:
            //            var value = ParseCString();
            //            for (int i = 0; i < column.Size - value.Length - 1; i++)
            //            {
            //                reader.ReadByte();
            //            }
            //            ((ColumnDefinition<string>)column).CurrentValue = value;
            //            break;
            //    }
            //}

            return true;
        }

        private void CheckIfLastMessage()
        {
            

            if ((status & MessageStatus.IsLastMessage) == MessageStatus.IsLastMessage)
            {
                LastMessage = true;
            }
        }

        private void ParseColumnDataIfExists(MessageStatus status)
        {
            if (columns.Any())
            {
                throw new InvalidDataException("Query response can only have one column definition");
            }

            if ((status & MessageStatus.ContainsColumnDefs) == MessageStatus.ContainsColumnDefs)
            {
                var columnCount = reader.ReadInt16();
                for (int i = 0; i < columnCount; i++)
                {
                    AddNextColumn();
                }
            }
        }

        private void AddNextColumn()
        {
            var name = ParseCString();
            var type = (NqpTypes)reader.ReadByte();
            var size = reader.ReadInt16();

            switch (type)
            {
                case NqpTypes.Char:
                    columns.Add(new ColumnDefinition<string>
                    {
                        Name = name,
                        Size = size,
                        Type = type
                    });
                    break;
                case NqpTypes.Int:
                    columns.Add(new ColumnDefinition<int>
                    {
                        Name = name,
                        Size = size,
                        Type = type
                    });
                    break;
            }
            
        }

        private string ParseCString()
        {
            List<byte> nameBytes = new List<byte>();
            var b = reader.ReadByte();

            while (b != '\0')
            {
                nameBytes.Add(b);
                b = reader.ReadByte();
            }

            var name = Encoding.ASCII.GetString(nameBytes.ToArray());
            return name;
        }

        private void GetMessageIfExists()
        {
            short messageSize = reader.ReadInt16();

            if (messageSize == 0)
                return;

            var msgBytes = new byte[messageSize];
            reader.Read(msgBytes, 0, messageSize);
            Message = Encoding.UTF8.GetString(msgBytes);
        }
    }
}
