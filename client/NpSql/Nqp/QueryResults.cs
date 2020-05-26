using System;
using System.Collections.Generic;
using System.Drawing;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace NpSql.Nqp
{
    internal class QueryResults
    {
        private MessageReader currentReader;
        private List<NpSqlColumnDefinition> columnsSchema = new List<NpSqlColumnDefinition>();
        private NetworkStream stream;
        private object[] rowValues;
        private bool receivedCompleted = false;
        internal bool HasRows { get; private set; }

        internal int FieldCount { get => columnsSchema.Count; }

        internal IEnumerable<NpSqlColumnDefinition> Columns { get { return columnsSchema; } }

        public QueryResults(NetworkStream stream)
        {
            this.stream = stream;
        }

        public bool NextRow()
         {
            if (receivedCompleted)
            {
                return !receivedCompleted;
            }

            switch (currentReader.MessageType)
            {
                case NqpMessageType.RowSet:
                    if (currentReader.ReadToEnd)
                    {
                        ProcessNextMessage();
                    }
                    
                    if (!receivedCompleted)
                    {
                        for (int i = 0; i < columnsSchema.Count; i++)
                        {
                            var column = columnsSchema[i];

                            switch (column.Type)
                            {
                                case NqpTypes.Char:
                                    rowValues[i] = currentReader.ReadString(column.Size);
                                    break;
                                case NqpTypes.Int:
                                    rowValues[i] = currentReader.ReadInt();
                                    break;
                            }
                        }
                    }
                    break;
                case NqpMessageType.Completed:
                    HandleCompleted();
                    break;
                default:
                    break;
            }

            return !receivedCompleted;
        }

        public QueryResults ProcessNextMessage()
        {
            currentReader = new MessageReader(stream);

            var messageType = currentReader.MessageType;

            switch (messageType)
            {
                case NqpMessageType.ColumnDefinition:
                    ParseColumnDefinitions();
                    break;
                case NqpMessageType.RowSet:
                    HasRows = true;
                    break;
                case NqpMessageType.Completed:
                    HandleCompleted();
                    break;
            }

            return this;
        }

        private void HandleCompleted()
        {
            var status = (CompletedStatus)currentReader.ReadByte();
            var length = currentReader.ReadShort();
            var message = currentReader.ReadString(length);

            if (status == CompletedStatus.Fail)
            {
                throw new NpSqlException($"FAILURE: {message} - tranaction as been rolled back.");
            }

            receivedCompleted = true;
        }

        public bool NextResult()
        {
            ProcessNextMessage();

            return currentReader.MessageType != NqpMessageType.Ready;
        }

        public T GetValue<T>(int ordinal)
        {
            return (T)rowValues[ordinal];
        }

        private void ParseColumnDefinitions()
        {
            short offset = 0;
            columnsSchema.Clear();
            receivedCompleted = false;
            while (!currentReader.ReadToEnd)
            {
                var type = (NqpTypes)currentReader.ReadByte();
                var length = currentReader.ReadShort();
                var nameLength = currentReader.ReadShort();
                var name = currentReader.ReadString(nameLength);


                columnsSchema.Add(new NpSqlColumnDefinition
                {
                    Name = name,
                    Offset = offset,
                    Size = length,
                    Type = type
                });

                offset += length;
            }

            rowValues = new object[columnsSchema.Count];

            currentReader = new MessageReader(stream);
        }
    }
}
