# WhatsApp System Design (SDE-2 HLD)

# 1. Requirements

## Functional Requirements

1. User should be able to send messages to another user.
2. User should be able to create/join groups and send messages to groups.
3. User should be able to see read receipts.
4. Messages should be synchronized across all devices of a user.
5. Offline users should receive messages when they come online.
6. At-least-once delivery guarantee.
7. Message ordering should be preserved within a conversation/group.

---

## Non-Functional Requirements

1. High availability (99.99%+).
2. Horizontal scalability.
3. Low latency (<100ms delivery).
4. Reliable message delivery.
5. Durability (messages should not be lost).
6. Fault tolerance.
7. End-to-end encryption (out of scope for current design).

---

# 2. Capacity Estimation

## Assumptions

* Registered Users = 2 Billion
* DAU = 1 Billion
* Messages/User/Day = 50

## Message Creation

Messages/day:

50B messages/day

Average QPS:

50B / 86400 ≈ 578K msg/sec

Peak QPS (5x):

≈ 3M msg/sec

---

## Delivery Events

Assumptions:

* 20% messages are group messages
* Average group size = 50

Direct Messages:

40B deliveries/day

Group Messages:

10B × 50 ≈ 500B deliveries/day

Total:

≈ 540B delivery events/day

Peak Delivery QPS:

≈ 30M deliveries/sec

---

## Concurrent Connections

Assumption:

15% of DAU online simultaneously

1B × 15%

≈ 150M concurrent users

Assuming:

1M WebSocket connections/server

Required:

≈ 150 servers

With headroom:

≈ 200-250 connection servers

---

# 3. Core Architecture

Client
|
Load Balancer
|
Connection Server
|
Message Service
|
+--> Message DB
|
+--> Kafka
|
+--> Fanout Service
|
+--> Delivery Service
|
+--> Presence Service
|
+--> Read Receipt Service
|
+--> Sync Service

Redis:

* Connection Registry
* Presence Data
* Device Mapping

---

# 4. Component Responsibilities

## Connection Server

Responsibilities:

* Maintain WebSocket connections
* Heartbeats
* Presence updates
* Push messages to connected clients

Not Responsible For:

* Persistence
* Ordering
* Retry Logic
* Delivery Guarantees

### Why Stateless?

Pros:

* Easy horizontal scaling
* Easy replacement during failures
* No data loss on restart

Cons:

* Requires external state store (Redis)

---

## Message Service

Responsibilities:

* Accept send-message requests
* Generate Message IDs
* Generate sequence numbers
* Persist messages
* Publish events to Kafka
* Acknowledge sender

---

# 5. Message Flow (1-to-1)

## Step 1

A sends message to Connection Server.

## Step 2

Connection Server forwards request to Message Service.

## Step 3

Message Service:

* Generates Message ID
* Generates Conversation Sequence Number
* Persists message

## Step 4

Acknowledges sender.

## Step 5

Publishes delivery event to Kafka.

## Step 6

Delivery Worker consumes event.

## Step 7

Redis lookup:

user -> connection server

## Step 8

If online:

Deliver immediately.

If offline:

Store and deliver later through Sync Service.

---

# Why Kafka?

## Without Kafka

Message Service directly performs delivery.

Problems:

* Tight coupling
* Backpressure impacts writes
* Poor scalability

## With Kafka

Pros:

* Decouples write and delivery paths
* Handles traffic spikes
* Independent scaling
* Retry support

Cons:

* Additional infrastructure
* Eventual consistency

Decision:

Use Kafka.

---

# 6. Connection Registry

Store:

user_id -> server_id

For multi-device:

user_id ->
[
device1 -> server42,
device2 -> server17,
device3 -> server90
]

Stored in Redis.

---

## Why Redis?

Pros:

* Extremely low latency
* TTL support
* In-memory lookups
* Horizontal scalability

Cons:

* Memory cost
* Operational complexity

Decision:

Use Redis.

---

# 7. Offline Messaging

Messages are persisted before delivery.

When user is offline:

Message remains in DB.

When user reconnects:

Sync Service fetches pending messages.

---

## Why Not Store Offline Messages in Kafka?

Pros:

* Simpler architecture

Cons:

* Kafka is not user state storage
* Long retention is expensive
* Hard to query pending messages

Decision:

Store in Message DB.

---

# 8. Message Ordering

## Problem

Network delays and retries can reorder messages.

Example:

M2 arrives before M1.

---

## Incorrect Solution

Timestamps

Problems:

* Clock skew
* Arrival time != Send time

---

## Correct Solution

### Conversation Sequence Number

Conversation(A,B)

M1 -> seq=1

M2 -> seq=2

M3 -> seq=3

---

### Kafka Partitioning

Partition Key:

conversation_id

All messages of same conversation go to same partition.

Kafka guarantees ordering within a partition.

---

## Why Not Global Ordering?

Not required.

Only conversation-level ordering matters.

---

# 9. Group Messaging

Group:

A, B, C, D, E

A sends message.

Message Service:

* Persists once
* Publishes group event

Fanout Service:

* Retrieves group members
* Creates recipient deliveries
* Delivers to online users
* Stores for offline users

---

# 10. Fanout Strategies

## Fanout-On-Write

Message stored once.

Inbox entries created for all recipients.

Example:

Inbox(B, msg500)

Inbox(C, msg500)

Inbox(D, msg500)

Pros:

* Fast reads
* Easy unread counts
* Fast offline sync

Cons:

* More writes
* More storage

---

## Fanout-On-Read

Store message once.

Compute recipients at read time.

Pros:

* Less storage
* Less write amplification

Cons:

* Expensive reads
* Complex unread calculations

---

## Decision

Small Groups:

Fanout-On-Write

Large Groups:

Fanout-On-Read

Hybrid strategy.

---

# 11. Read Receipts

State Machine:

SENT

↓

DELIVERED

↓

READ

---

## Delivered

Only after receiver device sends ACK.

Why?

Push to connection server != successful delivery.

---

## Read

Client emits READ event after user opens message.

Read Receipt Service:

Receives event

↓

Kafka

↓

Sender notified

---

# 12. Multi-Device Sync

User:

Phone

Laptop

Tablet

Message delivered to all devices.

Read on Phone:

READ event generated

↓

State updated

↓

Sync to Laptop and Tablet

---

## User-Level vs Device-Level Read State

### User-Level

Pros:

* Simpler
* Less storage

Cons:

* Less granular

Decision:

User-Level.

---

# 13. Presence Service

Stores:

* Online
* Offline
* Last Seen

---

## Heartbeats

Every ~30 seconds:

Client -> Connection Server -> Presence Service

Redis TTL refreshed.

---

## Offline Detection

TTL expires.

User marked offline.

Last Seen stored.

---

## Typing Indicators

Typing events are:

* Ephemeral
* Not persisted

Pros:

* Cheap
* Low latency

Cons:

* May occasionally be lost

Decision:

Acceptable.

---

# 14. Reliability

Requirement:

At-Least-Once Delivery

---

## Failure Scenario

Message delivered.

ACK lost.

Worker retries.

Duplicate arrives.

---

## Idempotency

Every message has:

msg_id

Client ignores duplicates.

Pros:

* Simple
* Industry standard

Cons:

* Requires deduplication logic

---

## Exactly Once vs At-Least Once

### Exactly Once

Pros:

* No duplicates

Cons:

* Expensive
* Distributed transactions
* Operational complexity

### At-Least Once

Pros:

* Reliable
* Simpler
* Widely adopted

Cons:

* Duplicates possible

Decision:

At-Least Once + Idempotency

---

# 15. Retry Strategy

Use Exponential Backoff:

1 sec

5 sec

30 sec

5 min

---

## Why Not Constant Retry?

Problems:

* Retry storms
* Cascading failures

Decision:

Exponential Backoff

---

# 16. Dead Letter Queue

Messages failing repeatedly:

Move to DLQ.

Pros:

* Debugging
* Isolation of bad events

Cons:

* Operational overhead

Decision:

Use DLQ

---

# 17. Database Sharding

## Bad

Single Database

Problems:

* Scalability limits
* Hotspots

---

## Better

Shard by:

conversation_id

or

group_id

Example:

hash(conversation_id) % N

---

## Why Not User ID?

Ordering becomes difficult.

Messages of same conversation may land on different shards.

Decision:

Shard by conversation/group.

---

# 18. Large Group Problem

Group:

100,000 members

Fanout-On-Write:

100,000 inbox writes per message.

Can become bottleneck.

---

## Solution

Hybrid Strategy

Small Groups:

Fanout-On-Write

Large Groups:

Fanout-On-Read

Potentially use sub-sharding.

---

# 19. Availability

## Kafka

Replication Factor = 3

---

## Redis

Primary + Replica

---

## Database

Primary + Replicas

Cross-region backups

---

## Connection Servers

Stateless

Horizontally scalable

---

# Final Design Decisions

| Problem             | Decision                          |
| ------------------- | --------------------------------- |
| Messaging Transport | WebSockets                        |
| Connection Registry | Redis                             |
| Async Communication | Kafka                             |
| Offline Storage     | Message DB                        |
| Ordering            | Sequence Number + Kafka Partition |
| Delivery Guarantee  | At-Least Once                     |
| Duplicate Handling  | Idempotency                       |
| Presence            | Redis TTL                         |
| Typing Indicator    | Ephemeral Events                  |
| Group Strategy      | Hybrid Fanout                     |
| Sharding            | Conversation ID                   |
| Read Receipts       | Event Driven                      |
| Multi Device        | User-Level Sync                   |
| Availability        | Replication + Stateless Services  |
