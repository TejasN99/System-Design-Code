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