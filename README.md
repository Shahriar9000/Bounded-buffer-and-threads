# Bounded-buffer-and-threads

Overview:
  This project was done to understand the behavior of correctly coordinating the producers and consumers and their depositing and retrieving activities.

The bounded-buffer problems (aka the producer-consumer problem) is a classic example of concurrent access to a shared resource. A bounded buffer lets multiple producers and multiple consumers share a single buffer. Producers write data to the buffer and consumers read data from the buffer.

Requirements:

Producers must block if the buffer is full.
Consumers must block if the buffer is empty.
