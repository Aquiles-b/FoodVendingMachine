# Food Vending Machine (in progress)

A terminal-based vending machine server in C++20. Each TCP connection is an independent session where clients browse products, build an order, and go through a payment flow. Built to explore ASIO and MySQL integration.

**Stack:** C++20 · Standalone ASIO · MySQL Connector/C++ · GNU Make · Docker

## Project Structure

```text
food_vending_machine/
  src/         Source files
  include/     Header files
sql/           Database initialization scripts
docker-compose.yaml
```

## Setup

Start MySQL with Docker:

```bash
docker compose up -d
```

This runs the scripts in `sql/` automatically.

> Credentials are hardcoded in `food_vending_machine/src/client_session.cpp`. 

Build:

```bash
cd food_vending_machine && make
```

## Running

```bash
./food_vending   # from food_vending_machine/
```

Listens on port `6666`. Connect with `telnet localhost 6666`.

## Commands

| Command               | Description                                         |
| --------------------- | --------------------------------------------------- |
| `LIST`                | Show available products with IDs and prices.        |
| `SELECT <id/name...>` | Add products to the order by ID or name.            |
| `CONFIRM`             | Confirm the order and start the payment flow.       |
| `QUIT`                | End the session.                                    |

**Payment flow:** after `CONFIRM`, select a payment method by number (or `0` to cancel). Then enter the payment amount when prompted. Payment is processed asynchronously, the result is pushed back to the client.

## Architecture

| Component          | Responsibility                                        |
| ------------------ | ----------------------------------------------------- |
| `ClientSession`    | Manages TCP connection and async I/O.                 |
| `FoodVmController` | Parses commands and drives application logic.         |
| `FoodVmOrder`      | Holds the current order and tracks its state.         |
| `FoodVmPayment`    | Simulates async payment processing via ASIO timers.   |
| `FoodVmDatabase`   | Encapsulates all MySQL operations.                    |
| `FoodVmNotification` | Generic callback wrapper for async event delivery. |

