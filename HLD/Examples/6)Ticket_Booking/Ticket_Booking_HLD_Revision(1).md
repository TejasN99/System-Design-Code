# Ticket Booking System (Train / IRCTC) - HLD Revision Notes

# 1. Scope

Assume external services exist: - Authentication/User Service - Payment
Service - Notification Service

We design only the Ticket Booking Service.

------------------------------------------------------------------------

# 2. Functional Requirements

-   Search trains by source, destination and date.
-   View seat availability by class and quota.
-   Book a ticket.
-   Cancel a booking.
-   View booking status.
-   Support General and Tatkal quota.
-   Integrate with payment service.

------------------------------------------------------------------------

# 3. Non Functional Requirements

-   High Availability
-   Horizontal Scalability
-   Strong Consistency (no double booking)
-   Low latency for search
-   Durable bookings
-   Reliability & Fault tolerance

------------------------------------------------------------------------

# 4. Capacity Assumptions

  Metric                  Value
  ----------------------- --------------
  DAU                     \~20 Million
  Peak Concurrent Users   \~2 Million
  Search QPS              15K-20K
  Availability QPS        15K-25K
  Booking QPS             500-1000
  Trains/day              \~15K

------------------------------------------------------------------------

# 5. APIs

GET /trains GET /availability POST /bookings POST /payments/callback GET
/bookings/{id} DELETE /bookings/{id}

------------------------------------------------------------------------

# 6. Data Model

Train Station TrainRoute Coach Seat Booking Passenger Inventory
SeatInventory

Inventory stores aggregated counts.

SeatInventory stores actual seat allocation.

------------------------------------------------------------------------

# 7. Inventory vs SeatInventory

Inventory - Fast availability lookup - Stores count only - Optimized for
reads

SeatInventory - Stores actual seat state - AVAILABLE - LOCKED - BOOKED -
Used while allocating seats

Tradeoff: - Inventory =\> O(1) reads - SeatInventory =\> actual
correctness

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
User Pays
   |
Success?
 /      \
Yes      No
 |        |
BOOKED   AVAILABLE
          Inventory++
```

------------------------------------------------------------------------

# 9. DB Lock vs Seat Lock

Database Row Lock - Exists only inside DB - Duration: milliseconds -
Prevents concurrent updates - Released after COMMIT

Seat Lock - Business concept - Duration: 5-10 minutes - Stored in
SeatInventory - Prevents others from reserving while payment happens

Never keep DB transaction open during payment.

------------------------------------------------------------------------

# 10. Why Not Keep DB Lock During Payment?

Problems: - Other users block - Long running transactions - Lock
contention - Low throughput - Database resources wasted

Correct approach: - Short DB transaction - Convert AVAILABLE -\>
LOCKED - Commit immediately - Payment happens outside transaction

------------------------------------------------------------------------

# 11. High Level Architecture

``` text
                Client
                   |
             Load Balancer
                   |
        +----------+-----------+
        |                      |
 Search Service        Booking Service
        |                      |
      Cache               PostgreSQL
```

------------------------------------------------------------------------

# 12. Scaling for Tatkal

Problem: Hundreds of thousands of booking requests hit DB
simultaneously.

Solution:

``` text
Client
   |
Load Balancer
   |
Rate Limiter
   |
Booking Service
   |
Admission Counter
   |
Message Queue
   |
Booking Workers
   |
PostgreSQL
```

Role of components:

Load Balancer - Distributes traffic

Rate Limiter - Stops abuse/bots

Admission Counter - Rejects requests when seats exhausted

Queue - Smooths traffic spikes

Workers - Control DB write rate

Database - Source of truth

------------------------------------------------------------------------

# 13. Cache Tradeoffs

Cache: - Train metadata - Station list - Routes - Schedules

Do NOT initially cache: - Seat inventory - Seat status - Booking status

Reason: Highly dynamic and consistency is critical.

If later needed: Use asynchronous cache updates. Database remains source
of truth.

------------------------------------------------------------------------

# 14. Queue Tradeoffs

Pros - Absorb traffic spikes - Protect DB - Smooth processing

Cons - Users may wait - Some requests processed after seats sold out

------------------------------------------------------------------------

# 15. Important Tradeoffs

Inventory Count vs Counting Seats

Inventory + Fast - Extra synchronization

SeatInventory Count + Always correct - Slow COUNT queries

------------------------------------------------------------------------

DB Lock vs Seat Lock

DB Lock + Strong consistency - Cannot keep for minutes

Seat Lock + User gets reservation - Needs expiry cleanup

------------------------------------------------------------------------

Cache Availability

Yes + Faster reads - Stale data

No + Correct - More DB reads

------------------------------------------------------------------------

Queue vs Direct DB

Queue + Protects DB + Handles spikes - Added complexity

Direct DB + Simpler - Doesn't scale for Tatkal

------------------------------------------------------------------------

# 16. Extensions

-   RAC
-   Waitlist
-   Cancellation
-   Refund Service
-   Read Replicas
-   Sharding by train_id + journey_date
-   Monitoring
-   Disaster Recovery
-   Idempotency
-   Audit logs

------------------------------------------------------------------------

# 17. Golden Interview Principles

1.  Don't add technology without identifying bottleneck.
2.  DB is source of truth.
3.  Keep transactions short.
4.  Protect the database.
5.  Every component must solve a problem.
6.  State tradeoffs.
7.  Explain WHY before WHAT.
