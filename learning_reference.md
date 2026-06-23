# Smart Food Vending Machine Server — Learning Reference

This document maps each phase to the concepts naturally encountered during implementation.

---

# Phase 1 — Basic Connectivity

## ASIO

* io_context
* acceptor
* socket
* asynchronous reads
* asynchronous writes
* session lifetime
* handler chains

## MySQL

* database connection
* SQL queries
* SELECT
* result sets

---

# Phase 2 — Customer Session Flow

## ASIO

* state machines
* composed workflows

## MySQL

* INSERT
* UPDATE
* transactions

---

# Phase 3 — Concurrent Customers

## ASIO

* multiple io_context threads
* synchronization
* execution ordering

## MySQL

* transactions
* isolation
* consistency
* row locking
* lost updates

---

# Phase 4 — Food Preparation

## ASIO

* timers
* delayed operations

## MySQL

* state persistence
* event history

---

# Phase 5 — Order Cancellation

## ASIO

* cancellation
* error handling

## MySQL

* rollback concepts
* compensating updates

---

# Phase 6 — Payment Authorization

## ASIO

* worker threads
* task delegation
* posting results

## MySQL

* recording payment results

---

# Phase 7 — Oven Queue

## ASIO

* queues
* producer-consumer patterns

## MySQL

* ordered processing
* state transitions

---

# Phase 8 — Sensors

## ASIO

* recurring timers
* long-running services

## MySQL

* periodic inserts
* telemetry tables

---

# Phase 9 — Event Subscription

## ASIO

* publish-subscribe
* fan-out communication

## MySQL

* event persistence

---

# Phase 10 — Administration Interface

## ASIO

* multiple client roles
* graceful shutdown

## MySQL

* CRUD operations
* INSERT
* UPDATE
* DELETE

---

# Phase 11 — Persistent Logging

## ASIO

* background services
* communication between execution contexts

## MySQL

* batch inserts
* file vs database logging

---

# Phase 12 — Refill Vehicle

## ASIO

* multiple clients
* protocol design

## MySQL

* inventory updates

---

# Phase 13 — Statistics

## ASIO

* shared state
* synchronization

## MySQL

* aggregate functions
* COUNT
* SUM
* AVG
* GROUP BY

---

# Phase 14 — Historical Queries

## ASIO

* request-response workflows

## MySQL

* JOIN
* ORDER BY
* LIMIT
* indexes
* query optimization

---

# Phase 15 — Fault Simulation

## ASIO

* failure recovery
* cancellation
* robustness

## MySQL

* consistency after failures
* transactional thinking

---

# Cross-Phase ASIO Concepts

* executors
* strands
* post()
* dispatch()
* defer()
* work guards
* lifetime management
* handler composition
* thread safety
* graceful shutdown

---

# Cross-Phase MySQL Concepts

## Schema Design

* primary keys
* foreign keys
* normalization

---

## Queries

* SELECT
* INSERT
* UPDATE
* DELETE

---

## Transactions

* COMMIT
* ROLLBACK
* ACID properties

---

## Constraints

* UNIQUE
* NOT NULL
* FOREIGN KEY

---

## Performance

* indexes
* query plans
* optimization

---

## Aggregation

* COUNT
* SUM
* AVG
* GROUP BY

---

## Relationships

* one-to-many
* many-to-one

---

## Advanced Topics

* connection pools
* prepared statements
* stored procedures
* views
* triggers
* optimistic locking
* pessimistic locking

---

# Suggested Learning Order

### ASIO

1. Sockets and sessions
2. Handler chains
3. Timers
4. Multiple threads
5. Synchronization
6. Task delegation
7. Long-running services
8. Publish-subscribe
9. Graceful shutdown
10. Advanced executors
11. Coroutines

### MySQL

1. Schema creation
2. SELECT
3. INSERT
4. UPDATE
5. DELETE
6. Transactions
7. Foreign keys
8. Joins
9. Aggregates
10. Indexes
11. Query optimization
12. Connection pools

