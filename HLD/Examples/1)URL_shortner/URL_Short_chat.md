# URL Shortener HLD - Interview Notes

# 1. Requirement Gathering

## Functional Requirements

1. User should be able to create a short URL.

2. User should be able to redirect using the short URL.

3. Support custom aliases.
   Example:

   ```
   short.ly/openai
   ```

4. Support URL expiration.

5. Support analytics (click count, traffic stats).

---

## Non Functional Requirements

### Availability

* Highly available system.
* Target: 99.9% - 99.99%.

### Scalability

* Support millions of users.
* Scale horizontally as traffic grows.

### Latency

* Redirect should be very fast.
* Target:

  ```
  <100ms ideal
  <200ms acceptable
  ```

### Durability

* Created URLs should never be lost.

### Consistency

* URL should be usable immediately after creation.

### Reliability

* Avoid single points of failure.

### Security

* Rate limiting.
* Spam URL detection.
* Malicious URL prevention.

### Cost

* Avoid overengineering.
* Start simple and scale when needed.

---

# 2. Capacity Estimation

Assumptions:

```
DAU = 10 Million
```

Assume:

```
5 Million URLs created/day
```

Average URL record size:

```
1 KB
```

Storage:

```
5M * 1KB
= 5GB/day
```

Yearly:

```
5GB * 365
≈ 1.8TB/year
```

5 Years:

```
≈ 9TB
```

---

## Redirect Traffic

Assume:

```
80% URLs accessed
```

```
4M URLs/day
```

Each URL:

```
100 accesses
```

Redirects:

```
4M * 100
= 400M redirects/day
```

Average QPS:

```
400M / 86400
≈ 4600 QPS
```

Peak QPS:

```
≈ 25K QPS
```

---

## Write Traffic

```
5M URLs/day
```

Average:

```
≈ 58 writes/sec
```

Peak:

```
≈ 300 writes/sec
```

---

## Key Observation

```
Read QPS ≈ 25K
Write QPS ≈ 300
```

System is heavily read-dominated.

This drives the architecture decisions.

---

# 3. API Design

## Create URL

```http
POST /shorten
```

Request:

```json
{
  "longUrl": "https://google.com",
  "alias": "google"
}
```

Response:

```json
{
  "shortUrl": "short.ly/google"
}
```

---

## Redirect

```http
GET /g8
```

Response:

```http
302 Redirect
Location: https://google.com
```

---

## Analytics

```http
GET /analytics/g8
```

---

# 4. Short URL Generation

## Option 1 - Auto Increment + Base62

Flow:

```
Insert URL
     ↓
DB generates ID
     ↓
Convert ID to Base62
     ↓
Store mapping
```

Example:

```
1000 -> g8
```

### Advantages

* Simple.
* Guaranteed uniqueness.
* No collision handling.
* Easy lookup.

### Drawbacks

* Centralized ID generation.
* Predictable URLs.
* Multi-region scaling is harder.

---

## Option 2 - Hash Original URL

Example:

```
MD5(URL)
```

### Advantages

* Distributed.
* No central coordinator.

### Drawbacks

* Hash collisions.
* Collision resolution required.
* Same URL always gets same short URL.

Rejected because collisions complicate the design.

---

## Option 3 - Random String

Example:

```
AbX91Kq
```

### Advantages

* Fully distributed.
* Hard to predict.

### Drawbacks

* Collision checks required.
* Extra DB lookup for uniqueness.

Rejected due to collision management.

---

## Option 4 - Snowflake IDs

### Advantages

* Distributed ID generation.
* No collisions.
* Massive scale.

### Drawbacks

* More complex.
* Machine IDs and clock synchronization needed.

Useful at very large scale.

---

## Chosen Solution

```
Auto Increment + Base62
```

Reason:

```
300 writes/sec
```

does not justify distributed ID generation.

---

# 5. Database Selection

## Access Pattern

Primary query:

```sql
SELECT long_url
FROM urls
WHERE short_code = ?
```

This is basically:

```
short_code
      ↓
long_url
```

Simple key-value lookup.

---

## PostgreSQL Chosen

Reasons:

* Supports current scale.
* Supports analytics metadata.
* Supports expiration.
* Supports user ownership.
* Supports indexing.

Schema:

```sql
urls
------------------------------
id BIGINT PRIMARY KEY
short_code VARCHAR UNIQUE
long_url TEXT
user_id BIGINT
created_at TIMESTAMP
expiry_at TIMESTAMP
```

Indexes:

```sql
UNIQUE(short_code)
INDEX(user_id)
```

---

# 6. Caching Layer

Problem:

```
25K read QPS
```

Repeated DB lookups.

Solution:

```
Redis
```

Flow:

```
User
 ↓
Redis
 ↓
Postgres (on miss)
```

Benefits:

* Reduced DB load.
* Lower latency.

---

# 7. CDN

Problem:

Viral URL:

```
500K QPS
```

Redis and application servers become bottlenecks.

Solution:

```
CDN
```

Flow:

```
User
 ↓
CDN
 ↓
Application
```

CDN caches redirect responses.

Benefits:

* Offloads backend.
* Handles viral traffic.

---

# 8. High Availability

## Problem

Postgres is a SPOF.

If it dies:

* Cannot create URLs.
* Cannot resolve cache misses.
* Cannot generate IDs.

---

## Solution

```
Primary
   |
Replicas
```

Writes:

```
Primary
```

Reads:

```
Replicas
```

Failover:

```
Replica promoted to Primary
```

---

# 9. Analytics Design

## Bad Approach

Every click:

```sql
UPDATE click_count
```

on Postgres.

Problem:

```
Too many writes.
```

---

## Better Approach

Redis Counters

Flow:

```
Redirect
   ↓
Redis INCR
   ↓
Periodic Batch Flush
   ↓
Postgres
```

Benefits:

* Extremely fast.
* Analytics removed from critical path.

Tradeoff:

```
Small amount of click data can be lost if Redis crashes.
```

---

## Future Scaling

For guaranteed delivery:

```
Redirect
   ↓
Kafka
   ↓
Analytics Consumer
   ↓
Analytics DB
```

---

# 10. Sharding

Not needed initially.

Reason:

```
9TB over 5 years
300 writes/sec
```

can be handled by a single database.

---

## When To Shard

* Storage becomes too large.
* Write throughput becomes bottleneck.

---

## Bad Sharding

By year:

```
2024 -> DB1
2025 -> DB2
```

Problem:

Need to search multiple shards.

---

## Good Sharding

Hash by short_code.

```
hash(short_code)
```

Example:

```
hash(g8)%4
```

Benefits:

* Uniform distribution.
* Single shard lookup.

---

# 11. Consistent Hashing

Problem:

```
hash(key)%N
```

Adding shards moves almost all data.

---

## Solution

Consistent Hashing

Benefits:

* Minimal data movement.
* Easy shard expansion.
* Better scalability.

---

# 12. Final Architecture

```text
                User
                  |
                  |
                CDN
                  |
          Load Balancer
                  |
             URL Service
              /       \
             /         \
         Redis      PostgreSQL
                        |
                  Read Replicas

Analytics

Redirect
    |
Redis Counter
    |
Batch Flush
    |
Postgres
```

---

# Why This Design Works

Functional Requirements:

✅ Create URL

✅ Redirect URL

✅ Custom Alias

✅ Expiration

✅ Analytics

---

Non Functional Requirements:

✅ High Availability

* Replicas + Failover

✅ Scalability

* Redis
* CDN
* Sharding

✅ Low Latency

* Redis
* CDN

✅ Durability

* PostgreSQL

✅ Consistency

* Immediate URL availability

✅ Reliability

* No major SPOF after replication

✅ Cost Effective

* Simple PostgreSQL solution first.
* Scale only when metrics justify it.

---

# Biggest HLD Lesson

Always do:

```
Requirements
    ↓
Capacity Estimation
    ↓
Architecture
```

Do NOT start with:

Kafka
Snowflake
Cassandra
Sharding

unless your capacity estimates justify them.
