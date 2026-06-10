Requirements

***Functional***:

Create short URLs
Redirect to original URLs
Optional custom aliases
Analytics

***Non-functional***:

High availability
Low latency redirects
Durable storage
Scalable to millions of URLs

-------------------

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


10M users ~ 50% make urls 

Capacity Estimation

5M URLs/day
1 KB per URL

= 5 GB/

5 GB × 365
= 1825 GB
≈ 1.8 TB


***Read Traffic***
5M urls/Day
80% acessed

4M * 100 times in a day  -> 400M redirects

400M / 86400 seconds
4629 ~ 5K reads/sec on avg

Peak Load = 3 to 5 times * avg
5 * 5K = 25K reads per second


***Write Traffic***

5M urls/Day
5M / 86400 ~ 58 Writes per second  ~ 60 WPS

Peak = 60 * 5 = 300 WPS


Read QPS  = 25,000
Write QPS = 300


***URLs***

if we use base62 encoding, 7 * 1 byte char

67^7 combinations = Trillions

-------------------

High Level Components


1) user makes url
POST /shorten
{
    "longUrl": "https://google.com",
    "alias": "google"
}
Response:
{
    "shortUrl": "short.ly/google"
}

2) Clicks a URL (Redirect)
GET /abc123

Response:
302 Redirect
Location: https://google.com


Simple Arch 

          User
            |
            |
      Load Balancer
            |
            |
    URL Shortener Service
            |
            |
         Database
    



***Which approach ***

1) Auto incr and base62

Tradeoff - 
1) DB becomes central part for id generation (SERIAL id DB)
2) URL becomes predictable as sequential
3) in case of multi DB its issue

Easy to implement

whatever number convert to BAse 62,  0-9, a-z, A-Z
Speacial chars require escape , / @

Indexing id is easier

id, short_code, long_url
number, its_base62, long



<!-- Works -->  in this 300 QPS writes so dooable no need for fancy



2) Random String Generation

Collison checking required, 

No central coodinator

3) Snowflake IDs + Base62 


Flow:

Timestamp
Machine ID
Sequence Number

↓

Generate unique 64-bit ID.

↓

Convert to Base62.



I'll use Auto Increment ID + Base62 initially because it's simple, collision-free, and our write traffic is only 300 QPS. If scale grows significantly, we can move to a distributed ID generator like Snowflake.



If interviewer asks:

Why not use random strings instead of Auto Increment + Base62?

Answer:

Random strings work and are used in production. However they require collision handling through uniqueness checks. Auto-increment/Base62 guarantees uniqueness without collision checks and provides deterministic O(1) generation. In distributed systems, instead of a database auto-increment counter, we typically use a distributed ID generator such as Snowflake or allocate ID ranges per region, then encode the resulting unique ID using Base62.





***REdis***

Traffic not random 

So most will go in Redis


***ARchitecture***

GET /g8
     |
     |
 Redis Lookup
     |
Hit?
 / \
Y   N
|    \
|     Postgres
|        |
|     Populate Cache
|        |
 ---------
     |
302 Redirect



-------

what if hot request and redis is getting hammered ?


CDN serve 


                 User
                   |
                   |
                 CDN
                   |
             Cache Miss
                   |
             Load Balancer
                   |
             App Servers
                   |
                Redis
                   |
              PostgreSQL


----------

URL Creation
User
 ↓
Load Balancer
 ↓
App Server
 ↓
Generate Base62
 ↓
PostgreSQL



Redirect
User
 ↓
CDN
 ↓
Redis
 ↓
PostgreSQL
 ↓
302 Redirect