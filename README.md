# Jolly-Jip Restaurant — Financial Information & POS System

A modular, terminal-based Point-of-Sale (POS) and Financial Information System (FIS) developed in C. The application simulates full restaurant floor operations, including dynamic combo/package ordering, discount authorization pipelines, payment settlement, end-of-day sales auditing, and flat-file persistence.

---

## Key Architectural Features

- **Transaction & Order Processing**:
  - Multi-tier package routing (Solo, Barkada, Family) and à la carte individual orders.
  - Interactive table ordering supporting up to 50 unique line items per guest check.
- **Discount & Settlement Engine**:
  - Automatic subtotal volume discounts (2% between ₱500.00 and ₱2,000.00).
  - Statutory concessionaire deductions: Senior Citizen (20%) and Person with Disability / PWD (5%).
  - Enforced tender validation loop to prevent underpayment before receipt commitment.
- **Flat-File Storage & Auditing**:
  - `menu.txt`: Dynamic menu database preserving spaces via tokenized underscore parsing.
  - `sales_history.txt`: Append-only transaction log storing delimited receipt bills.
  - Daily Total Sales reconciliation: Filter transactions by date (`MM/DD/YYYY`) to audit gross billing, discounts, and net intake.
- **File Maintenance (CRUD)**:
  - Add new menu records with automated duplicate item-code rejection.
  - Real-time price and description updates.
  - Tabular catalog display with fixed-width formatting.

---

## Data Schema & Structures

The application is structured using three core composite types:

| Struct | Purpose | Attributes |
| :--- | :--- | :--- |
| `MenuItem` | Master catalog definition | `itemCode`, `itemDescription`, `unitPrice`, `itemType` |
| `OrderItem` | Line item in active cart | `itemCode`, `itemDescription`, `quantity`, `unitPrice`, `amount` |
| `Bill` | Full settlement record | `date`, `tableNo`, `items[]`, `totalBill`, `discount`, `netBill`, `amountTendered`, `change` |

---

## Getting Started

### Prerequisites
- GCC, Clang, or MinGW compiler supporting **C99** or later.

### Installation & Build

1. Clone the repository:
   ```bash
   git clone [https://github.com/](https://github.com/)<YOUR-USERNAME>/jolly-jip-restaurant-fis.git
   cd jolly-jip-restaurant-fis
