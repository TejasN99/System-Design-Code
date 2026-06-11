# Notification System HLD (3+ YOE Interview Revision Notes)

# 1. Problem Statement

Design a scalable notification system that supports:

* Topic subscriptions
* Push notifications
* In-App notifications
* Email notifications
* Multiple devices per user
* Notification history
* Read/Unread status
* High scalability
* At-least-once delivery

---

# 2. Requirements Discussion

## Functional Requirements

1. Users can subscribe/unsubscribe to topics.
2. Support Push, In-App, Email, SMS channels.
3. Notifications should be delivered to all registered devices.
4. Users can configure notification preferences.
5. Users can view notification history.
6. Support Read/Unread status.
7. Support broadcast notifications.

## Non Functional Requirements

1. High Availability.
2. High Scalability.
3. Near Real-Time Delivery.
4. At-Least-Once Delivery.
5. Durable notification storage.
6. Eventual Consistency is acceptable.

---

# 3. Capacity Estimation

## Assumptions

DAU = 10 Million

Subscribed Users = 50%

Active Subscribers:

5M users

Notifications/User/Day:

50

Devices/User:

2

Notification Size:

1 KB

Retention:

30 Days

---

## Notification Deliveries

5M × 50 × 2

= 500 Million Deliveries/Day

---

## Average Throughput

500M / 86400

≈ 6K Writes/Sec

Peak (5x Burst)

≈ 30K Writes/Sec

---

## Storage

500M × 1KB

= 500 GB/Day

30 Days Retention

≈ 15 TB

---

# 4. Core Concept: Fanout

## What is Fanout?

One notification event being distributed to many users.

Example:

Cricket Update

↓

5M Subscribers

↓

5M Deliveries

---

# 5. Fanout-On-Write vs Fanout-On-Read

## Fanout-On-Write

When event arrives:

Immediately create notification records for all subscribers.

### Pros

* Fast Reads
* Simple Notification Center
* Low Read Latency

### Cons

* Huge Write Amplification
* Expensive for massive topics

---

## Fanout-On-Read

Store event once.

Generate notifications when user opens notification center.

### Pros

* Minimal Writes
* Storage Efficient

### Cons

* Expensive Reads
* Complex Query Logic

---

## Choice

For Notification System:

Use Fanout-On-Write

Reason:

Users read notifications frequently and expect low latency.

---

# 6. High Level Architecture

Producer Services

↓

Kafka

↓

Fanout Workers

↓

Notification DB

↓

Delivery Queue

↓

Push / Email / SMS Workers

↓

External Providers

---

# 7. Why Kafka?

Without Kafka:

Producer waits for millions of notifications.

High latency.

Poor reliability.

---

## Kafka Benefits

### Decoupling

Producer does not care about delivery.

### Reliability

Messages survive worker crashes.

### Scalability

Add more consumers.

### Traffic Smoothing

Kafka absorbs traffic spikes.

### At-Least-Once Delivery

Messages are not lost.

---

# 8. Subscriber Management

Notification Service owns subscriptions.

## Topic Table

Topic

* topic_id
* topic_name

---

## Subscription Table

Subscription

* user_id
* topic_id

---

## Preference Table

UserPreference

* user_id
* topic_id
* push_enabled
* email_enabled
* sms_enabled

---

# Why Notification Service Owns This?

Alternative:

Every service manages subscribers.

Problems:

* Duplicate logic
* Difficult preference management
* Hard to scale

Centralized ownership is cleaner.

---

# 9. Large Topic Fanout

Example:

Cricket Topic

5M Subscribers

Do NOT:

Fetch all subscribers and process together.

---

## Better

Chunk Subscribers

Example:

Chunk Size = 10K

5M / 10K

= 500 Fanout Jobs

Benefits:

* Parallel Processing
* Better Retries
* Better Fault Isolation
* Horizontal Scalability

---

# 10. Device Management

Notifications are sent to devices, not users.

## Device Table

Device

* device_id
* user_id
* device_token
* platform
* status

---

Example

User

↓

Phone

Tablet

Laptop

All receive notifications.

---

# 11. Notification Storage Design

## Option A

Store notification text for every user.

Huge duplication.

---

## Option B (Chosen)

Notification

* notification_id
* title
* message

UserNotification

* user_id
* notification_id
* read_status

### Why Better?

* Saves storage
* Message stored once
* Cleaner schema

---

# 12. Delivery Pipeline

Fanout Worker

↓

Delivery Queue

↓

Push Worker

Email Worker

SMS Worker

---

# Why Separate Queue?

Fanout decides:

WHO should receive.

Delivery decides:

HOW to deliver.

Benefits:

* Independent scaling
* Retry isolation
* Better fault tolerance

---

# 13. At-Least-Once Delivery Problem

Worker receives message.

↓

Sends Push.

↓

Crashes before Kafka offset commit.

Kafka redelivers.

↓

Duplicate Notification

---

# Solution: Idempotency

Every notification has:

notification_id

Before sending:

Check if already delivered.

If yes:

Skip.

Else:

Send.

---

## Redis Optimization

Store:

delivery:<notification_id>:<device_id>

Benefits:

* Fast lookup
* Prevent duplicate delivery

---

# 14. Notification History

## Query Pattern

Get notifications for user.

Therefore partition by:

user_id

NOT notification_id.

---

## Why?

Most common query:

Get notifications for a user.

Partitioning should follow access patterns.

---

# 15. Read/Unread Status

UserNotification

* user_id
* notification_id
* read_status

Update status when user reads notification.

---

## Unread Count

Do NOT calculate using COUNT(*) repeatedly.

Use Redis Counter.

Example:

unread_count:user123

INCR on new notification.

DECR on read.

---

# 16. Retry Strategy

Failures happen:

* FCM down
* SMTP down
* Network issues

---

## Retry Queue

Failed Messages

↓

Retry Queue

---

## Exponential Backoff

Retry after:

1 Minute

5 Minutes

15 Minutes

1 Hour

---

## Dead Letter Queue (DLQ)

After max retries:

Move to DLQ.

Benefits:

* Debugging
* Prevent infinite retries

---

# 17. Multi-Channel Delivery

Separate Workers

* Push Worker
* Email Worker
* SMS Worker

Benefits:

* Independent scaling
* Isolation
* Different retry strategies

---

# 18. Availability Strategy

Kafka

* Replication Factor = 3

Database

* Primary
* Read Replicas

Redis

* Redis Cluster

No single point of failure.

---

# 19. Tradeoffs Summary

## Fanout-On-Write

Pros

* Fast Reads

Cons

* Heavy Writes

---

## Fanout-On-Read

Pros

* Low Writes

Cons

* Slow Reads

---

## Kafka

Pros

* Decoupling
* Reliability
* Scalability

Cons

* Operational Complexity
* Duplicate Processing

---

## At-Least-Once Delivery

Pros

* No Message Loss

Cons

* Requires Idempotency

---

# 20. Interview Summary

Final Design Decisions

✓ Kafka for decoupling and durability

✓ Fanout-On-Write

✓ Chunked fanout processing

✓ Notification stored once

✓ UserNotification mapping table

✓ Separate delivery queue

✓ Push/Email/SMS workers

✓ Redis for unread counts

✓ Redis for idempotency

✓ Retry Queue + DLQ

✓ Partition by user_id

✓ Multi-device support

This design handles large-scale notification delivery while maintaining low read latency, high availability, and horizontal scalability.




# Appendix: Final Architecture Diagram

```text
                    +----------------------+
                    |   Producer Services  |
                    |----------------------|
                    | Video Service        |
                    | Cricket Service      |
                    | Order Service        |
                    | Comment Service      |
                    | Marketing Service    |
                    +----------+-----------+
                               |
                               | Notification Event
                               v
                    +----------------------+
                    |        Kafka         |
                    |  Notification Topic  |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |   Fanout Workers      |
                    +----------+-----------+
                               |
              +----------------+----------------+
              |                                 |
              v                                 v
  +----------------------+       +----------------------+
  | Subscription Store   |       |    Device Store      |
  |----------------------|       |----------------------|
  | Topic                |       | device_id            |
  | Subscription         |       | user_id              |
  | User Preferences     |       | device_token         |
  +----------------------+       | platform             |
                                 +----------------------+
                                              |
                                              |
                               +--------------+-------------+
                               |
                               v
                    +----------------------+
                    | Notification Service |
                    +----------+-----------+
                               |
                               |
                               v
                    +----------------------+
                    | Notification DB      |
                    |----------------------|
                    | Notification         |
                    | UserNotification     |
                    +----------+-----------+
                               |
                               |
                               v
                    +----------------------+
                    |    Delivery Queue    |
                    |       (Kafka)        |
                    +----------+-----------+
                               |
         +---------------------+---------------------+
         |                     |                     |
         v                     v                     v
+----------------+   +----------------+   +----------------+
|  Push Worker   |   | Email Worker   |   |  SMS Worker    |
+--------+-------+   +--------+-------+   +--------+-------+
         |                    |                    |
         v                    v                    v
+----------------+  +----------------+  +----------------+
| FCM / APNS     |  | SMTP Provider  |  | SMS Provider   |
+----------------+  +----------------+  +----------------+

                     +----------------------+
                     |        Redis         |
                     |----------------------|
                     | Unread Counters      |
                     | Idempotency Keys     |
                     | Hot Subscription     |
                     | Cache                |
                     +----------------------+
```

## End-to-End Flow

1. Producer service generates a notification event.
2. Event is published to Kafka.
3. Fanout workers consume the event.
4. Fanout workers fetch subscribers and user preferences.
5. Subscribers are processed in chunks (e.g., 10K users per chunk).
6. Device information is fetched for each user.
7. Notification metadata is stored in Notification DB.
8. Delivery jobs are pushed to Delivery Queue.
9. Push/Email/SMS workers consume delivery jobs.
10. Workers send notifications via external providers.
11. Redis is used for:

    * Idempotency checks
    * Unread notification counts
    * Hot subscription caching
12. Failed deliveries go to Retry Queue and eventually DLQ after max retries.

```

### Key Design Decisions

- Kafka for durability and decoupling.
- Fanout-on-write for low-latency reads.
- Chunk-based fanout for large subscriber groups.
- Notification stored once, mapped via UserNotification.
- Separate delivery pipeline for Push/Email/SMS.
- Redis for performance-critical operations.
- Retry Queue + DLQ for fault tolerance.
- Partition UserNotification by user_id for efficient notification-center reads.
```
