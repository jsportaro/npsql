# NQP Protocol

_npsql_ communicates with clients via the application layer message based _npsql_ Query Protocol or, redundently, _nqp protocol_. Currently, TCP/IP is used as the transport layer protocol.  

Note: TCP/IP isn't nessessarily a requirement of the nqp spec.  Unix sockets or Window's named pipes are equality implementable transport protocols.  However, for the sake of simplicity TCP/IP is used.

The _nqp_ protocol is based on a half-duplex exchange of descrete messages.  Messages can be broadly broken into two message categories: Query Messages and Session Messages.  Session messages control the lifetime of the connection between client and server.  Query messages allow clients to submit queries and receive results.

Both message categories follow the same given basic message format:

| Field Name   | Size               |
|--------------|--------------------|
| Message Type | 1 byte             |
| Payload Size | 2 bytes            |
| Payload      | Payload Size bytes |

The protocol is composed of the following messages.  As new features are added to the protocol
this list might grow.

## Table of Messages and their categories  

### Session Messages
| Name         | Message Type | Description                                | Direction* |
|--------------|--------------|--------------------------------------------|------------|
| Hello        | 0x01         | Starts a session                           | C2S        |
| Welcome      | 0x02         | Acknowledges the session has been accepted | S2C        |
| Sorry        | 0x03         | The session could not be accepted          | S2C        |
| Goodbye      | 0x04         | End the session                            | C2S        |
| ComeBackSoon | 0x05         | Acknowledges the session has ended         | S2C        |

### Query Messages
| Name             | Message Type | Description                            | Direction* |
|------------------|--------------|----------------------------------------|------------|
| Query            | 0x06         | Submits query(s)                       | C2S        |
| ColumnDefinition | 0x07         | Columns for following RowSet           | S2C        |
| RowSet           | 0x08         | Rows resulting from a SELECT           | S2C        |
| Completed        | 0x09         | A SQL statement has finished executing | S2C        |
| Ready            | 0x0A         | The query(s) have finished executing   | S2C        |

\* C2S = Client to Server; S2C = Server to client
Message Flows
## Session Messages

### Starting a Session - saying Hello!
When clients want to connect to nqp, they'll first open a transport layer connection to the server.  Currently, only TCP/IP is supported.  After successfully initializing the transport layer, Clients send a Hello message to the server. If the server is able to accept the Client then it sends back a Welcome message.  Otherwise, a Sorry message is sent.  Clients recieving a Sorry message can try again in the future to connect.

### Ending a session - saying Goodbye!
When the Client has decided to end it's session, it sends a Goodbye message to the server.  In response, the server will send back a ComeBackSoon message and close the transport layer connection.  

### Detailed Session Message Formats

#### Hello

| Field Name       | Size     | Description                                                             |
|------------------|----------|-------------------------------------------------------------------------|
| Client UUID      | 16 bytes | What the client would like to refer to itself by.  Useful for debugging |

#### Welcome

| Field Name       | Size     | Description                                                             |
|------------------|----------|-------------------------------------------------------------------------|
| Max Message Size | 2 bytes  | The maximum size in bytes the server will send as a message             |


#### Sorry

Besides the header, message contains no content.  Used to indicate a session was not able to begin.

#### Goodbye

Besides the header, message contains no content.  Used to indicate a client is ending the session.

## Query Messages

### Asking a question
When clients want to perform action on npsql, they'll first open a session as described in the previous section.  If they receive a Welcome, they'll be able to send Query messages.  Query messages can contain zero or more SQL statements all executing in the same transaction.  For each SQL statement submitted, they'll be an associated Completed message.  In cases of successful SELECT statements, the client will also get a ColumnDefinition message and possibly, if rows were returned, a number of RowSet messages.  npsql will be optimistic about the results of all SQL statements in a Query message.  Meaning,it will assume they're all going to execute successfully.  If a statement fails to execute successfully, the transaction will be rolled back and the Completed message will detail the statement causing the error.  Keep in mind, this implies that if the first statement is a sucessful SELECT and the second is an unsuccessul INSERT then the client still could've seen the results of the SELECT.  When all statements have been executed either successfully or unsuccessfully a Ready message will be sent to the client indicating it's ready for the next Query message.  The implication being that only one Query message can be "in-flight" at a given time. 

### Detailed Query Message Formats

#### Query

To accomodate SQL statements longer than the max message size, each Query message payload starts with a _Continue_ byte.  If the byte is set to 1, the SQL text expands to another Query message.  If it is set to zero, the full text of the SQL was either contained in one message; or, the text spanned multiple Query messages and this is the last of that series.

One per SQL statement

| Field Name       | Size                   | Description                                        |
|------------------|------------------------|----------------------------------------------------|
| SQL String       | Length given by message header | UTF8 encoded SQL string                            |

#### ColumnDefinition

One per returned column

| Field Name         | Size                     | Description                            |
|--------------------|--------------------------|----------------------------------------|
| Column Name Length | 2 bytes                  | The length in bytes of the Column Name |
| Column Name        | Column Name length bytes | UTF8 encoded column name               |
| Type               | 1 byte                   | One of npsql's data types              |
| Length             | 2 bytes                  | Length in bytes of the column          |

The Type column can be interpreted with the below table

| Type Name | Length (bytes) | Type enum value |
|-----------|----------------|-----------------|
| int       | 4              | 0x01            |
| char      | N              | 0x02            |

#### RowSet

RowSet messages will follow the ordering and schema of the most recent ColumnDefinition message.  RowSet messages will therefor always be homogeneous.  As a result, there isn't a hard and fast format for RowSet messages as they're entirly defined the most recent ColumnDefinition message.

#### Completed

| Field Name     | Size                 | Description                    |
|----------------|----------------------|--------------------------------|
| Result         | 1 byte               | One of npsql's result types    |
| Message Length | 2 bytes              | Length in bytes of the Message |
| Message        | Message Length bytes | UTF8 encoded message           |

The Result column can be iterpreted with the below table

| Result Name | Result enum value | Description                                 |
|-------------|-------------------|---------------------------------------------|
| Success     | 1                 | Statement was able to run to completion     |
| Fail        | 2                 | Statement was not able to run to completion |

#### Ready

Besides the header, message contains no content.  Used to indicate the client may send a new Query message

## Example nqp Protocol message exchange

#### Hello 

The client, after opening a TCP/IP connection, send the Hello message.  

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| 00000000   | 01 | 10 | 00 | 8b | a6 | b9 | 7c | fd | 6e | 3e | 45 | 94 | 56 | 90 | 82 | 98 | 
| 00000010   | d4 | 96 | a2 |    |    |    |    |    |    |    |    |    |    |    |    |    |

The client requests the size of a message to be no longer than 1024 bytes and that it'd like to be called 7cb9a68b-6efd-453e-9456-908298d496a2.

#### Welcome

The server accepts the session by sending the Welcome message 

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A | B | C | D | E | F |
|------------|----|----|----|----|----|----|----|----|----|----|---|---|---|---|---|---|
| 00000000   | 02 | 02 | 00 | 00 | 04 |    |    |    |    |    |   |   |   |   |   |   |

#### Query

After recieving the Welcome message the Client is free to send Query messages.  In this example, the client issues the following statements:

```SQL

select * from addresses;
insert into people (name, age) values ('joe', 36)

```

It does so by sending the following

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 06 | 4E | 00 | 73 | 65 | 6C | 65 | 63 | 74 | 20 | 2A | 20 | 66 | 72 |6F  | 6D |
|  00000010  | 20 | 61 | 64 | 64 | 72 | 65 | 73 | 73 | 65 | 73 | 3B | 5C | 72 | 5C |6E  | 69 |
|  00000020  | 6E | 73 | 65 | 72 | 74 | 20 | 69 | 6E | 74 | 6F | 20 | 70 | 65 | 6F |70  | 6C |
|  00000030  | 65 | 20 | 28 | 6E | 61 | 6D | 65 | 2C | 20 | 61 | 67 | 65 | 29 | 20 |76  | 61 |
|  00000040  | 6C | 75 | 65 | 73 | 20 | 28 | 27 | 6A | 6F | 65 | 27 | 2C | 20 | 33 |36  | 29 |
|  00000050  | 3B |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    | 

#### ColumnDefinition

Given that the schema indeed supports the SQL statements, the server will first send back the following ColumnDefinition

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| 00000000   | 07 | 29 | 00 | 0A | 00 | 73 | 74 | 72 | 65 | 65 | 74 | 6E | 61 | 6D | 65 | 02 | 
| 00000010   | 19 | 00 | 04 | 00 | 63 | 69 | 74 | 79 | 02 | 0A | 00 | 05 | 00 | 73 | 74 | 61 | 
| 00000020   | 74 | 65 | 01 | 04 | 00 | 03 | 00 | 7A | 69 | 70 | 01 | 04 |    |    |    |    | 

Here we see that the first SQL statement will return rows with the following schema 

| Column Name | Type | Length |
|-------------|------|--------|
| Streetname  | char | 25     |
| city        | char | 10     |
| state       | int  |        |
| zip         | int  |        |

#### RowSet

Immediately following the ColumnDefinition message, the server will send a set of RowSet messages.  In our example, for the sake of brevity, the server will send only one RowSet message containing the entire contents of the addresses table as shown below

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| 00000000   | 08 | 00 | 54 | 00 | 32 | 31 | 20 | 57 | 69 | 6E | 64 | 73 | 6F | 72 | 20 | 4C |
| 00000010   | 61 | 6E | 65 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 4C | 69 | 74 |
| 00000020   | 69 | 74 | 7A | 00 | 00 | 00 | 00 | 26 | 00 | 00 | 00 | 87 | 44 | 00 | 00 | 31 |
| 00000030   | 38 | 30 | 20 | 4D | 69 | 64 | 64 | 6C | 65 | 73 | 65 | 78 | 20 | 41 | 76 | 65 |
| 00000040   | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 00 | 50 | 69 | 73 | 63 | 61 | 74 | 61 | 77 |
| 00000050   | 61 | 79 | 1E | 00 | 00 | 00 | 96 | 22 |    |    |    |    |    |    |    |    |

Which, after deserializing, results in the following table:

| Streetname        | city       | state | zip   |
|-------------------|------------|-------|-------|
| 21 Windsor Lane   | Lititz     | 28    | 17543 |
| 180 Middlesex Ave | Piscataway | 30    | 8854  |

#### Completed

As all the rows have been returned, the next message will be Completed

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 09 | C0 | 00 | 01 | 09 | 00 | 43 | 6F | 6D | 70 | 6C | 65 | 74 | 65 | 64 |    |

#### Completed... again

Currently, insert statements do not return rows.  So, the next message will be another similarly looking Completed

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 09 | C0 | 00 | 01 | 09 | 00 | 43 | 6F | 6D | 70 | 6C | 65 | 74 | 65 | 64 |    |

Note: The message is purely illustrative.  During normal operation the server is free to return any message message.  The import take away is a string will be returned with the prescribed format.

#### Ready

Finally, the server will send a Ready message indicating the client to send more Query messages, a Goodbye message, or to simply sit idly.

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 0A | 00 | 00 |    |    |    |    |    |    |    |    |    |    |    |    |    |

#### Goodbye

In this example, the client will simply close the session with a Goodbye message

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 04 | 00 | 00 |    |    |    |    |    |    |    |    |    |    |    |    |    |


#### ComeBackSoon

The server will return a ComeBackSoon and close the TCP/IP connection.

| Offset (h) | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | A  | B  | C  | D  | E  | F  |
|------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
|  00000000  | 05 | 00 | 00 |    |    |    |    |    |    |    |    |    |    |    |    |    |
