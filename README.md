# STL Monotonic Wedge

## Lemire-Fenn algorithm for rolling min/max

Computing the minimum and maximum values in a "rolling" range --- such as the last second of an audio signal --- is a common problem in DSP, particularly in "peak finding" problems which often arise in transient detection and compressor design.  They can also be used to observe trends in gradually-changing values.

The Lemire algorithm allows for computing the rolling minimum and maximum values in amortized constant time per sample, regardless of range.  The algorithm here incorporates enhancements proposed by Ethan Fenn to lower the worst-case complexity per sample from **O(N)** to **O(log2(N)).**  My implementation here lowers that _very slightly_ further to **O(J)**, where **J=log2(N-J)**.

The algorithm works by maintaining a "monotonic wedge", comprising those values which compare greater (or less) than all values following them and ending with the latest value.  Each time a new value is added to the wedge, any values which are not greater (or less) than the latest value are removed from the end of the wedge, after which the new value is appended.

_(Think of it like gazing at a mountain range from a great distance.  If a new mountain were to appear, it would obscure the foothills behind it --- but not the higher peaks beyond.)_

The "front" (oldest) value of the wedge is always the greatest (or least) of all.  Typically values beyond a certain age are removed (via `pop_front` or similar) in order to evaluate the largest (or smallest) value in a finite range.  Even if this is not done, the algorithm will update in amortized linear time---only the worst-case time per sample will increase!

Read the original Lemire paper [here](https://arxiv.org/abs/cs/0610046).


### Code

Only `mono_wedge.h` is necessary to utilize this algorithm in a C++ application.

The implementation here conforms to the C++/STL programming style and may be used with `std::deque` and `std::vector` templates in addition to others satisfying the requirements (below).


```python
min_wedge_update (wedge, value)
max_wedge_update (wedge, value)
mono_wedge_update(wedge, value, comp)
```

Call these functions with an initally empty container in order to maintain a monotonic wedge of minimum or maximum values.  The first (oldest) element in the container will always be the minimum (or maximum) with subsequent values gradually increasing (or decreasing) until the latest.

If using `mono_wedge_update`, supplying a "less" function as `comp` produces a monotonically-increasing "min-wedge", while supplying a "greater" function results in a monotonically-decreasing "max-wedge".

**Complexity:**  N updates to an initially empty container will take **O(N)** time.  The worst case for a single update is slightly less than **O(log2(N))** time.

The container must fulfill the requirements below.  `std::vector` and `std::deque` work well.


```python
min_wedge_search (begin, end, value)
max_wedge_search (begin, end, value)
mono_wedge_search(begin, end, value, comp)
```

These functions behave similar to `std::lower_bound`, returning the first element which does not satisfy `comp(value, element)`.

**Complexity:**  Slightly less than **O(log2(N))** time in the worst case.  Uses a combination of linear and binary search in order to facilitate amortized constant-time execution of `wedge_update` routines.

The iterators must fulfill the requirements below.  `std::vector` and `std::deque` work well.


### Requirements

`wedge_search` functions may be used with any range of random access iterators, including those provided by `std::vector` and `std::deque`.

`wedge_update` functions require that the wedge class produces random access iterators from its `begin()` and `end()` methods, and supports appending elements via a `push_back` method.  `std::vector` and `std::deque` satisfy these requirements.


## Future Work

DSP applications may prefer to use a fixed-size ringbuffer as the underlying container for a wedge of bounded size.  The documentation should be updated with a recommendation for an STL-compliant ringbuffer implementation.

In addition to the `<algorithm>`-inspired functions here, it may be convenient to implement a wedge container type.  This could behave similar to `std::queue`, which is internally implemented using `std::deque`.

I may or may not pursue these enhancements myself, and welcome pull requests.


## License

This code is made available under the MIT license, and the underlying algorithm is unencumbered by patents at the time of writing.

See the LICENSE file for full copy.


## Credits

The original algorithm was designed by Daniel Lemire:  [See here](https://github.com/lemire/runningmaxmin) for his implementation [or here](https://arxiv.org/abs/cs/0610046) for the paper.

The enhanced algorithm was proposed on the music-dsp mailing list by Ethan Fenn and implemented here by Evan Balster.
