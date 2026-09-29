# Particle Timings — Chapter 29

The companion program for Chapter 29 of *Learning C++ by Building Games*, The
Data Locality Pattern. It's a console program, all in one `main.cpp`, and every
piece of it that the chapter shows matches this file exactly.

It uses Chapter 14's `Particle` and `moveParticle` (taking a reference rather
than a pointer), and times the same work done to particles kept in memory in
different ways, printing how long each took:

- **How far away is the data?** The time to fetch one number from memory, for
  chains of numbers from 16 KB to 256 MB, each number saying where the next
  one is, in a shuffled order.
- **Vectors, lists, and pointers.** A million particles moved on by one frame,
  kept in a vector, a `std::list`, a vector of pointers, and a vector of
  `std::unique_ptr`s, each as made and after sorting.
- **Structures and arrays.** The particles as an array of structures and as a
  structure of arrays (`ParticleArrays`), moving only their positions, and
  doing everything.
- **Hot and cold.** A `FatParticle`, with a cold `ParticleHistory` inside,
  against lean particles with their histories kept in a vector of their own.
- **Two kinds of particle.** Falling and floating particles, moved through
  virtual calls, a `switch` on an `enum class`, and a vector for each kind,
  with the kinds mixed or in blocks.

Every test starts from a copy of the same million particles, runs 15 times,
and keeps its fastest time. The chapter shows one run's results from an Intel
Core i7-12700F. Yours will differ; the shape won't.

## To run

Open `Particle Timings.slnx`, choosing **Trust and Continue** if Visual Studio
asks. Switch the configuration on the toolbar from **Debug** to **Release**,
and press **Ctrl+F5** (Start Without Debugging). It takes about fifteen
seconds. Timings from a Debug build, or with the debugger attached, don't
compare, as the chapter explains.

To try a hundred thousand particles, change `COUNT` to `100000`.

It uses the `SDL3` folder beside it in `SDL3 Projects`, for SDL's clock and
random numbers, and `SDL3.dll` is already in the project folder. It opens no
window.
