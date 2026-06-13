# Rate Limiter - High Level Design (SDE-2 Interview Notes)

# 1. Problem Statement

Design a distributed rate limiter that can:

* Enforce request limits on incoming traffic.
* Allow requests within configured limits.
* Reject requests exceeding configured limits.
* Support millions of users and high throughput.
* Add minimal latency to request processing.

---

# 2. Functional Requirements

## Mandatory

### FR1. Enforce Rate Limits

Allow or reject requests based on configured limits.

Example:

* Free User → 100 requests/minute
* Premium User → 1000 requests/minute

---

### FR2. Fast Decision Making

For every request, return:

* ALLOW
* REJECT

before the request reaches backend services.

---

### FR3. Support Multiple Rate Limit Dimensions

Rate limiting can be applied on:

* User ID
* API Key
* IP Address
* Organization/Tenant

---

### FR4. Configurable Limits

Limits should be changeable without deployment.

Example:

* Free → 100/min
* Premium → 1000/min
* Enterprise → 10000/min

---

### FR5. Distributed Enforcement

Requests can hit different gateway instances.

The limit must still be enforced globally.

---

### FR6. Quota Information

Return metadata:

* Remaining quota
* Total quota
* Reset time

---

# 3. Non Functional Requirements

### NFR1. Low Latency

Rate limiter sits in request path.

Target:

* P99 < 10ms

---

### NFR2. High Throughput

Should support:

* 100K+ QPS initially
* Scale to millions of requests/sec

---

### NFR3. High Availability

Target:

* 99.999% availability

---

### NFR4. Horizontal Scalability

Should scale by adding more nodes.

---

### NFR5. Memory Efficient

Must support millions of active users.

---

### NFR6. Controlled Inaccuracy is Acceptable

Slight limit violations are acceptable if they significantly improve:

* Availability
* Scalability
* Latency

---

# 4. Capacity Estimation

Assumptions:

DAU = 10 Million

Average Requests/User/Day = 100

Total Requests Per Day:

10M × 100

= 1 Billion Requests/Day

---

Average QPS:

1B / 86400

≈ 11,500 QPS

≈ 12K QPS

---

Peak QPS

Assume:

10x traffic spike

Peak:

≈ 120K QPS

Round:

100K QPS

---

# 5. Algorithm Selection

Several algorithms are available.

Interviewers expect discussion of tradeoffs.

---

## Option 1: Fixed Window

Example:

Limit = 100 requests/minute

Windows:

12:00-12:01
12:01-12:02

Store:

count

Only one counter per user.

### Pros

* Very simple
* O(1) memory
* O(1) computation
* Easy Redis implementation

### Cons

Boundary burst issue.

Example:

100 requests at 12:00:59

100 requests at 12:01:01

Result:

200 requests in 2 seconds

while policy says:

100 requests/minute

### Verdict

Very cheap but inaccurate.

---

## Option 2: Sliding Window Log

Store timestamp of every request.

Example:

[10,20,35,42,50]

For each request:

1. Remove expired timestamps.
2. Check queue size.
3. Insert current timestamp.

### Pros

* Most accurate
* True rolling window

### Cons

High memory usage.

Example:

100 req/min

Need to store:

100 timestamps/user

Millions of users make this expensive.

### Verdict

Excellent accuracy but poor scalability.

---

## Option 3: Sliding Window Counter

Store:

* Current window count
* Previous window count

Approximate rolling window using weighted counts.

### Pros

* Better accuracy than Fixed Window
* Much lower memory than Sliding Log

### Cons

Approximation
Not perfectly accurate

### Verdict

Good middle ground.

---

## Option 4: Token Bucket

Store:

* Available Tokens
* Last Refill Timestamp

Tokens are replenished over time.

Request consumes one token.

If token available:

ALLOW

Else:

REJECT

### Example

Bucket Capacity = 100

Refill Rate = 10 tokens/sec

User idle for 10 seconds.

Bucket becomes:

100 tokens

User suddenly sends:

50 requests

Allowed.

---

### Pros

#### O(1) Memory

Need only:

* Token count
* Timestamp

#### O(1) Computation

Simple refill formula.

#### Supports Bursts

Short bursts are often desirable.

Users should not be punished for temporary spikes.

#### Highly Scalable

Works well with Redis.

#### Industry Standard

Used by many API gateways and cloud providers.

---

### Cons

Not perfectly accurate rolling-window enforcement.

A small amount of approximation exists.

---

### Why Choose Token Bucket?

Our requirements:

* Very low latency
* High throughput
* Millions of users
* Memory efficiency
* Controlled burst support

Token Bucket satisfies all requirements while keeping implementation simple.

This provides the best tradeoff.

---

## Option 5: Leaky Bucket

Incoming requests enter a queue.

Requests leave at fixed rate.

Example:

Leak Rate = 10/sec

100 requests arrive instantly.

Output:

10/sec
10/sec
10/sec

---

### Pros

Smooth traffic.

Protects backend systems.

---

### Cons

Does not naturally support bursts.

Requests may experience delays.

---

### Verdict

More suitable for traffic shaping than API rate limiting.

---

# Final Algorithm Choice

Chosen:

Token Bucket

Reason:

Best balance of:

* Accuracy
* Memory
* Throughput
* Simplicity
* Burst Support

---

# 6. Placement of Rate Limiter

Possible Options:

### Option 1: Inside Every Service

Pros:

* Easy local implementation

Cons:

* Duplicate logic
* Hard to manage
* Wastes resources before rejection

Rejected.

---

### Option 2: Dedicated Rate Limiter Service

Pros:

* Centralized

Cons:

* Additional network hop
* Increased latency

Possible but unnecessary.

---

### Option 3: API Gateway

Pros:

* First entry point
* Rejects requests early
* Protects backend systems
* Centralized policy enforcement

Chosen Approach.

---

# 7. Architecture

Request Flow:

Client

↓

ALB

↓

API Gateway Fleet

↓

Redis Cluster

↓

Allow / Reject

↓

Backend Services

---

# 8. Why Redis?

Options Considered

## Database

Pros

* Durable

Cons

* Too slow
* Cannot handle required throughput

Rejected.

---

## In-Memory Storage in Gateway

Pros

* Extremely fast

Cons

* Different gateways maintain different counters

Limit violations occur.

Rejected.

---

## Redis

Pros

* In-memory
* Very low latency
* Atomic operations
* Supports expiration
* Handles large throughput

Chosen.

---

# 9. Redis Data Model

Key:

rate_limit:user123

Value:

tokens = 75

last_refill_timestamp = 1718199000

Stored as Redis Hash.

---

# 10. Atomicity Problem

Naive Flow:

GET tokens

GET timestamp

Calculate

SET tokens

Race condition possible.

Example:

Tokens = 1

Gateway A reads 1

Gateway B reads 1

Both allow.

Limit violated.

---

# Solution: Redis Lua Script

Perform:

1. Read
2. Refill
3. Consume
4. Update
5. Return Decision

inside Redis.

Entire operation becomes atomic.

---

# 11. Configuration Management

Runtime State:

Redis

Configuration:

MySQL/Postgres

Stores:

* User plans
* Limits
* Refill rates

Gateway caches configuration locally.

Request path avoids DB lookups.

---

# 12. High Availability

Problem:

Single Redis is SPOF.

---

Solution

Primary

↓

Replica

Automatic failover.

---

# 13. Scaling Redis

Current:

100K QPS

Future:

Millions of QPS

Single Redis becomes bottleneck.

---

Solution:

Redis Cluster

Shard users across nodes.

Example:

user1 → shard1

user2 → shard2

user3 → shard3

---

# 14. Consistent Hashing

Normal Hashing:

hash(key) % N

Adding node changes mapping for almost all keys.

---

Consistent Hashing:

Only small subset moves.

Preferred approach.

---

# 15. Failure Strategy

## Fail Open

Redis unavailable.

Allow requests.

Pros:

Application remains available.

Cons:

Rate limiting bypassed.

---

## Fail Closed

Redis unavailable.

Reject requests.

Pros:

Backend protected.

Cons:

Entire service unavailable.

---

Decision depends on business requirements.

Most consumer applications choose Fail Open.

---

# 16. Hot Key Problem

Problem:

Popular API key receives massive traffic.

All requests hit one Redis key.

Single shard overloaded.

---

Solutions

### Option 1

Accept.

Often sufficient.

---

### Option 2

Split counter into multiple shards.

More complex.

---

### Option 3

Token Leasing

Redis grants token batches to gateways.

Gateway spends tokens locally.

Reduces Redis load significantly.

Used at very high scale.

---

# 17. Multi-Region Discussion

Option A

Global Redis

Pros:

Accurate limits.

Cons:

High latency.

---

Option B

Regional Redis

Pros:

Fast.

Cons:

Small limit violations possible.

---

Most systems prefer regional deployment and tolerate slight inaccuracies.

---

# 18. Monitoring

Track:

* Allowed Requests
* Rejected Requests
* Redis Latency
* Redis Errors
* Hot Keys
* Failovers
* Token Consumption Rate

---

# Final Architecture Diagram

+----------------------+
| Config Service       |
+----------+-----------+
|
v
+----------------------+
| MySQL / PostgreSQL   |
+----------------------+

```
       |
       v
```

+----------------------+
| Client Applications  |
+----------+-----------+
|
v
+----------------------+
| ALB / Load Balancer  |
+----------+-----------+
|
v
+----------------------+
| API Gateway Fleet    |
| (Local Config Cache) |
+----------+-----------+
|
v
+----------------------+
| Redis Cluster        |
| Token Buckets        |
+----------+-----------+
|
Allow / Reject
|
v
+----------------------+
| Backend Services     |
+----------------------+
