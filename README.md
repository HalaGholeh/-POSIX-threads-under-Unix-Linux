# -POSIX-threads-under-Unix-Linux
# Supermarket Product Shelving Simulation

## Project Overview

This project simulates the behavior of supermarket employees who are responsible for placing products on shelves. The simulation involves both multi-processing and multi-threading to handle the dynamic environment of a supermarket, including customer arrivals and product shelving activities.

## Features

- **Product Management**: The supermarket sells `p` products, each with a defined shelf amount and storage amount.
- **Shelving Teams**: Multiple shelving teams, each with a team manager and several employees, are responsible for restocking shelves when product levels drop below a threshold.
- **Customer Arrivals**: Customers arrive at the supermarket randomly within a user-defined range and select random items in random quantities.
- **Simulation End Conditions**: The simulation ends if the storage area is out of stock or the simulation runs for a user-defined amount of time.

## Implementation Details

- **Multi-Processing and Multi-Threading**: Customers are represented as processes, and shelving teams are also processes. Within each shelving team, the manager and employees are threads.
- **Interprocess Communication (IPC)**: Utilizes message queues, semaphores, and shared memory to manage communication and synchronization between processes and threads.
- **User-Defined Configuration**: A text file is used to define all configurable parameters such as product list, shelf quantities, storage quantities, and time ranges.
- **Graphics**: Uses OpenGL for a simple and elegant visual representation of the simulation.

## Getting Started

### Prerequisites

- **C Compiler**: Ensure you have `gcc` or any other C compiler installed.
- **OpenGL**: Install OpenGL libraries for graphics.
- **IPC Libraries**: Ensure you have necessary libraries for IPC mechanisms.

