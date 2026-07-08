# Ticket Booking System (Train Booking) - HLD Revision Notes

## 1. Scope

### In Scope

-   Search trains
-   View seat availability
-   Book ticket
-   Cancel ticket
-   Booking status
-   General & Tatkal quota

### Out of Scope

-   Authentication/User Service
-   Payment Gateway (external)
-   Notification Service

------------------------------------------------------------------------

# 2. Functional Requirements

-   Search trains by source, destination and date.
-   View seat availability by class and quota.
-   Book tickets.
-   Confirm booking after payment.
-   Cancel booking.
-   View booking status.
-   Support General and Tatkal quotas.

------------------------------------------------------------------------

# 3. Non Functional Requirements

-   High Availability
-   Horizontal Scalability
-   Strong Consistency for seat allocation
-   Low latency (especially search)
-   Durability
-   Reliability & Fault Tolerance

------------------------------------------------------------------------

# 4. Capacity Estimation (Example)

-   DAU: \~20 Million
-   Peak Concurrent Users: \~2 Million
-   Search QPS: 15K--20K
-   Availability QPS: 15K--20K
-   Booking QPS: 500--1000
-   \~15K trains/day

------------------------------------------------------------------------

# 5. APIs

GET /trains - source - destination - date

GET /availability - trainId - date - class - quota

POST /bookings - userId - passenger details - class - quota

POST /payment/callback

GET /booking/{id}

DELETE /booking/{id}

------------------------------------------------------------------------

# 6. Data Model

Train - train_id - train_number - train_name

Station - station_id - station_code

TrainRoute - train_id - station_id - sequence

Coach - coach_id - train_id - class

Seat - seat_id - coach_id - berth_type

Booking - booking_id - user_id - status - payment_status

Passenger - passenger_id - booking_id - seat_id

Inventory - train_id - journey_date - class - quota - available_count

SeatInventory - train_id - journey_date - seat_id - status -
booking_id - lock_expiry

------------------------------------------------------------------------

# 7. Why Inventory + SeatInventory?

Inventory - Fast availability lookup - Stores only count - Read
optimized

SeatInventory - Stores actual seat status - Used during booking - Write
optimized

Tradeoff: - Inventory alone cannot tell which seat to assign. -
SeatInventory alone makes availability expensive (COUNT on every
search).

------------------------------------------------------------------------

# 8. Booking Flow

``` text
Search
    |
Availability
    |
BEGIN TRANSACTION
    |
SELECT ... FOR UPDATE
    |
Seat -> LOCKED
Inventory--
Create Pending Booking
COMMIT
    |
Payment
    |
+----------------------+
| Success              |
|  Seat -> BOOKED      |
|  Booking CONFIRMED   |
+----------------------+
        OR
+----------------------+
| Failure/Timeout      |
| Seat -> AVAILABLE    |
| Inventory++          |
| Booking FAILED       |
+----------------------+
```

------------------------------------------------------------------------

# 9. Database Lock vs Seat Lock

## Database Row Lock

Purpose: - Prevent concurrent modification

Duration: - Milliseconds

Invisible to user

Released after COMMIT

## Seat Lock

Purpose: - Reserve seat during payment

Duration: - 5--10 minutes

Stored in DB: - status=LOCKED - lock_expiry

Visible to application

------------------------------------------------------------------------

# 10. Why Not Keep DB Lock During Payment?

If payment takes 3 minutes:

-   Row remains locked.
-   Other users block.
-   Thousands of waiting transactions.
-   Increased memory, lock contention, transaction metadata.

Correct approach:

``` text
Short DB Transaction
        |
Seat -> LOCKED
Commit
        |
User Pays
```

------------------------------------------------------------------------

# 11. High Level Architecture

``` text
                Client
                   |
             Load Balancer
                   |
      +------------+-------------+
      |                          |
 Search Service          Booking Service
      |                          |
   Cache                 Rate Limiter
      |                          |
      |                  Admission Counter
      |                          |
      |                    Message Queue
      |                          |
      |                   Booking Workers
      |                          |
      +------------+-------------+
                   |
              PostgreSQL
```

------------------------------------------------------------------------

# 12. Why Each Component?

Load Balancer - Distribute traffic

Cache - Train metadata - Station list - Routes - Schedules

Rate Limiter - Prevent bots/spam

Admission Counter - Reject requests once seats exhausted

Message Queue - Smooth Tatkal spikes

Workers - Control DB write rate

PostgreSQL - Source of truth

------------------------------------------------------------------------

# 13. Cache Tradeoff

Cache: - Train metadata - Routes - Station data

Don't cache (initially): - Seat inventory - Seat status

Reason: - Highly dynamic - Strong consistency required

If introduced: - DB remains source of truth. - Redis updated
asynchronously. - Booking always validates against DB.

------------------------------------------------------------------------

# 14. Tatkal Optimizations

Problem: - Huge spike in booking requests.

Solutions: - Rate limiting - Admission control - Queue - Worker pool

Tradeoff: - Some queued users may eventually get "Sold Out".

------------------------------------------------------------------------

# 15. Extensions

Waitlist/RAC - Promote RAC when confirmed seat cancels. - Promote WL to
RAC.

Cancellation - status=CANCELLED - Refund via Refund Service

Idempotency - Use payment transaction id/idempotency key.

Sharding - Prefer (train_id + journey_date) - Avoid user_id

Read Replicas - Search -\> Replica - Booking -\> Primary

Monitoring - Queue length - Booking latency - Search latency - DB CPU -
Failed bookings - Lock expiry

Disaster Recovery - Replication - Backups - Promote replica on failure

------------------------------------------------------------------------

# Key Interview Tradeoffs

  ------------------------------------------------------------------------
  Decision                     Why            Tradeoff
  ---------------------------- -------------- ----------------------------
  Inventory + SeatInventory    Fast reads +   Extra synchronization
                               accurate seat  
                               allocation     

  DB Lock                      Prevent double Must be short-lived
                               booking        

  Seat Lock                    Allow payment  Need cleanup job
                               time           

  Queue                        Smooth spikes  Increased latency

  Rate Limiter                 Prevent abuse  Some legitimate requests
                                              rejected

  Cache Metadata               Reduce DB load Cache invalidation

  Don't Cache Inventory        Strong         More DB reads
  Initially                    consistency    

  Shard by Train               Local seat     Uneven shard sizes possible
                               allocation     
  ------------------------------------------------------------------------

------------------------------------------------------------------------

# Golden Interview Lines

-   Database is the source of truth.
-   Use short DB transactions.
-   Convert DB lock into application-level seat lock.
-   Never hold DB locks during payment.
-   Add components only after identifying a bottleneck.
-   Separate read-heavy and write-heavy workloads.
