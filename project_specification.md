# Smart Food Vending Machine Server with MySQL

## Overview

Implement the software that controls an embedded self-service food vending machine.
Customers connect through TCP and interact with the machine using a text protocol.
The machine stores persistent information in a MySQL database. Inventory, products, orders, and historical data must survive server restarts.
The server is the main focus. Telnet or Netcat may be used as clients.

---

# General Requirements

* Support multiple simultaneous TCP clients.
* Keep the machine responsive under load.
* Persist important data in MySQL.
* Recover its state after restart.
* Continue operating indefinitely.
* Allow future expansion.

---

# System Components

* TCP Server
* Client Session
* Vending Machine Controller
* Product Service
* Inventory Service
* Payment Service
* Food Preparation Service
* Notification Service
* Sensor Service
* Statistics Service
* Logging Service
* Database Service
* Administration Interface

---

# Database

Tables:

## products

| Column           | Type    |
| ---------------- | ------- |
| id               | INT     |
| name             | VARCHAR |
| price            | DECIMAL |
| preparation_time | INT     |
| enabled          | BOOLEAN |

---

## inventory

| Column     | Type |
| ---------- | ---- |
| product_id | INT  |
| quantity   | INT  |

---

## orders

| Column      | Type     |
| ----------- | -------- |
| id          | INT      |
| state       | VARCHAR  |
| total_price | DECIMAL  |

---

## order_items

| Column      | Type     |
| ----------- | -------- |
| order_id    | INT      |
| product_id  | INT      |
| unit_price  | DECIMAL  |

---

## order_events

| Column    | Type     |
| --------- | -------- |
| id        | INT      |
| order_id  | INT      |
| event     | VARCHAR  |
| timestamp | DATETIME |

---

## statistics

| Column | Type    |
| ------ | ------- |
| key    | VARCHAR |
| value  | BIGINT  |

---

# Phase 1 — Basic Connectivity

Commands:

### LIST

Displays available products.
Example:

```text
LIST
```

Response:

```text
ID  Product   Price
1   PIZZA     20
2   BURGER    15
3   SODA      5
```

Products are loaded from MySQL.

---

### QUIT
Disconnects the client.

---

# Phase 2 — Customer Session Flow

Supported commands:

### SELECT
```text
SELECT BURGER 1 3
```

Selection can be by name or ID

---

### CONFIRM
Finalizes the order and start the payment flow.

---

### STATUS
Returns the order state.

Example:

```text
STATUS
```

Response:

```text
Preparing
```

Order state changes are stored in MySQL.

---

# Phase 3 — Concurrent Customers
The machine supports multiple clients simultaneously.

Requirements:

* Inventory values must remain consistent.
* Stock cannot become negative.
* Orders are independent.
* Simultaneous purchases are allowed.

Example:

Initial stock:

```text
PIZZA 1
```

Two customers purchase pizza simultaneously.
Only one should succeed.

---

# Phase 4 — Food Preparation

Preparation times:

| Product | Time      |
| ------- | --------- |
| Burger  | 8 s       |
| Pizza   | 15 s      |
| Soda    | Immediate |

Example:

```text
Preparing...
```

Later:

```text
Ready.
```

Order state changes are recorded.
Order history is stored.

---

# Phase 5 — Order Cancellation

Command:

```text
CANCEL <order_id>
```

Requirements:

* Restore inventory.
* Update database records.
* Register cancellation events.
* Completed orders cannot be canceled.

---

# Phase 6 — Payment Authorization

Payment approval takes approximately five seconds.
Payment results are saved.

---

# Phase 7 — Oven Queue

The machine contains a single oven.
Orders wait in line.
Example:

```text
Order 10
Order 11
Order 12
```

Execution order must be preserved.
Order transitions are recorded in the database.

---

# Phase 8 — Sensors

Simulated sensors:

### Temperature

Every second.
Example:

```text
TEMP 42C
```

---

### Door

Every five seconds.
Example:

```text
DOOR CLOSED
```

---

### Inventory Monitor

Every thirty seconds.
When stock becomes low:

```text
LOW STOCK BURGER
```

Sensor events are stored.

---

# Phase 9 — Event Subscription

Command:

```text
WATCH
```

Receive asynchronous notifications.

Examples:

```text
ORDER_READY 32
```

```text
LOW_STOCK BURGER
```

```text
TEMP 43C
```

---

# Phase 10 — Administration Interface

Commands:

### SHOW INVENTORY

---

### SHOW ORDERS

---

### SHOW PRODUCT

---

### ADD

Example:

```text
ADD BURGER 10
```

Updates inventory.

---

### REMOVE

Example:

```text
REMOVE BURGER 3
```

---

### CREATE_PRODUCT

Example:

```text
CREATE_PRODUCT HOTDOG 12 6
```

Creates a product in MySQL.

---

### DISABLE_PRODUCT

Example:

```text
DISABLE_PRODUCT HOTDOG
```

---

### SHUTDOWN

Gracefully terminates the machine.

---

# Phase 11 — Persistent Logging

Store events:

* client connections
* order creation
* payment approval
* preparation completed
* cancellation
* low stock alerts

---

# Phase 12 — Statistics

Maintain:

* total orders
* canceled orders
* average preparation time
* most sold product
* revenue

Command:

```text
STATS
```

Example:

```text
Orders: 153
Revenue: 4280
Most Sold: BURGER
```

---

# Phase 13 — Historical Queries

Command:

```text
ORDER_HISTORY 42
```

Example:

```text
CREATED
PAYMENT_APPROVED
PREPARING
READY
```

---

Command:

```text
TOP_PRODUCTS
```

Example:

```text
BURGER 120
PIZZA 90
HOTDOG 40
```

---

Command:

```text
REVENUE
```

Example:

```text
Today: 2500
Month: 42700
```

---

# Phase 15 — Fault Simulation

Random failures:

### Payment Failure

```text
Payment rejected.
```

---

### Oven Failure

```text
OVEN ERROR
```

Current order is canceled.

---

### Temperature Alarm

```text
TEMPERATURE ALARM
```

Preparation pauses until temperature returns to normal.
All events are persisted.

---

# Stretch Goals

## Persistence Recovery

After restart:

* active orders are restored
* inventory is restored
* statistics are restored

---

## Monitoring Dashboard

Display:

* active orders
* temperature
* inventory
* alarms
* revenue

---

## Fleet Management

Multiple vending machines connected to a central server.

---

## Recipe Versioning

Allow changing recipes over time without affecting historical orders.
