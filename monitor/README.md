# monitor

Each `monitor.*` file here implements a ring buffer that is safe to use when there are multiple producer threads and multiple consumer threads, using a *monitor* (a *mutex* + *condition variables*).

## Language comparison

|        | Error Handling             | Memory (Heap) Management                | Resource Clean-Up Support |
| ------ | -------------------------- | --------------------------------------- | ------------------------- |
| C      | Return code, `errno`       | Manual                                  | None                      |
| C++    | Exception                  | Manual, RAII                            | RAII                      |
| Java   | Exception                  | Garbage collection                      | `finally`                 |
| Python | Exception                  | Reference counting with cycle detection | `finally`, `with`         |
| D      | Exception                  | Garbage collection                      | RAII, `finally`, `scope`  |
| Rust   | `Result`, `panic!`         | RAII                                    | RAII                      |
| Go     | `error`, `panic`/`recover` | Garbage collection                      | `defer`                   |
