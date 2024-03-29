# ParallelContainers
Parallel high performance C++ containers, including a **set** class and a **map** class.

These containers distribute the inserting, finding and erasing operations to multiple containers to decrease the chance of waiting. This makes better use of multi-core CPU power.

## Sample performance test results, using OpenMP for multi-threading.
>> CPU: Intel Core i7 3740QM
<br> 1600000 integer items were inserted, searched and erased
<br> 8 parallel threads

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


## STL set (std::set)<br/>
![STL set](https://github.com/huxia1124/ParallelContainers/blob/master/SampleResults/stl_set.png?v2)

## Concurrent set (Concurrency::concurrent_unordered_set)<br/>
![Concurrent set](https://github.com/huxia1124/ParallelContainers/blob/master/SampleResults/concurrent_set.png?v2)

## High Performance set (CSTXHashSet)<br/>
![High Performance set](https://github.com/huxia1124/ParallelContainers/blob/master/SampleResults/high_performance_set.png?v2)
 

> The above results were measured under the following configuration:
>> CPU: Intel Core i7 3740QM
<br> 12800000 integer items were inserted, searched and erased
<br> 8 parallel threads
