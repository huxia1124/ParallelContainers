# ParallelContainers
Parallel high performance C++ containers, including a **set** class and a **map** class.

## Sample performance test results, using OpenMP for multi-threading.
> on Intel® Core™ i7-3740QM Processor

```
---------- STL set (std::set) ----------
Insertion:              1600000 items!
Insertion:       681ms elapsed.
Found:                  1600000 items.
Searching:       1710ms elapsed.
Erasing:         637ms elapsed.
Now:                    0 items!

---------- Concurrent set (Concurrency::concurrent_unordered_set) ----------
Insertion:              1600000 items!
Insertion:       133ms elapsed.
Found:                  1600000 items.
Searching:       117ms elapsed.
Erasing:         2065ms elapsed.
Now:                    0 items!

---------- High Performance set ----------
Insertion:              1600000 items!
Insertion:       136ms elapsed.
Found:                  1600000 items.
Searching:       268ms elapsed.
Erasing:         164ms elapsed.
Now:                    0 items!
```
