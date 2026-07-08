Availability
Scalability
Latency
Durability
Consistency
Reliability
Security
Cost


A good HLD interview for 3+ YOE usually looks like:

1. Requirements Gathering
2. Capacity Estimation
3. High Level Components
4. Core Data Model
5. Deep Dive into Critical Components
6. Scaling Bottlenecks
7. Reliability & Monitoring
8. Tradeoffs




| Setup                              | Approx Reads/sec       | Approx Writes/sec      |
|------------------------------------|------------------------|------------------------|
| Single MySQL Instance              | 5K - 20K              | 1K - 10K              |
| MySQL + Proper Indexes             | 10K - 50K+            | 2K - 20K              |
| High-End MySQL Server (NVMe SSDs)  | 50K - 200K+           | 10K - 50K+            |
| MySQL with Read Replicas           | 100K+ combined reads  | Same as primary       |
| MySQL Sharded Across Nodes         | Millions+             | Hundreds of thousands+|
| PostgreSQL (powerful single node)  | 10K - 100K+           | 5K - 50K+             |
| Cassandra Cluster                  | 100K+                 | 100K+                 |
| DynamoDB                           | Millions+             | Millions+             |
| Bigtable / ScyllaDB / CockroachDB  | Hundreds of thousands | Hundreds of thousands |
| Redis                              | 100K - 1M+ ops/sec    | 100K - 1M+ ops/sec    |
| Kafka Broker                       | 100K - 1M+ msgs/sec   | 100K - 1M+ msgs/sec   |
| Snowflake ID Service               | Millions IDs/sec      | Millions IDs/sec      |



Capacity Estimations

Metric	    Small	Medium	Large
DAU	        100K	1M	    10M+
Write QPS	100	    1K	    10K
Read QPS	1K	    10K	    100K
Storage	    GBs	    TBs	    PBs


100K DAU  -> small
1M DAU    -> medium
10M DAU   -> large
100M DAU  -> internet scale




+-------+--------------+-------------------+--------------------+-------------------+------------------+
| Nines | Availability | Downtime / Year   | Downtime / Month   | Downtime / Week   | Downtime / Day   |
+-------+--------------+-------------------+--------------------+-------------------+------------------+
| 1     | 90%          | 37 days           | 73 hours           | 17 hours          | 2.4 hours        |
| 2     | 99%          | 3.7 days          | 7.3 hours          | 1.7 hours         | 14 minutes       |
| 3     | 99.9%        | 9 hours           | 44 minutes         | 10 minutes        | 1.4 minutes      |
| 4     | 99.99%       | 53 minutes        | 4.4 minutes        | 1 minute          | 8.6 seconds      |
| 5     | 99.999%      | 5.3 minutes       | 26 seconds         | 6 seconds         | 860 ms           |
| 6     | 99.9999%     | 32 seconds        | 2.6 seconds        | 600 ms            | 86 ms            |
| 7     | 99.99999%    | 3.2 seconds       | 260 ms             | 60 ms             | 8.6 ms           |
| 8     | 99.999999%   | 320 ms            | 26 ms              | 6 ms              | 860 μs           |
| 9     | 99.9999999%  | 32 ms             | 2.6 ms             | 600 μs            | 86 μs            |
| 10    | 99.99999999% | 3.2 ms            | 260 μs             | 60 μs             | 8.6 μs           |
+-------+--------------+-------------------+--------------------+-------------------+------------------+




Requirements
Capacity Estimation
APIs
Data Model
High-Level Architecture
Core Flow
Concurrency
Scaling
Trade-offs
Extensions
Interviewer's Follow-up Questions