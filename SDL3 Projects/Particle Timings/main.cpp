/*
    Particle Timings
    The Chapter 29 project from Learning C++ by Building Games

    Chapter 14's particles, timed. First, how long it takes to fetch one
    number from memory, depending on how much memory is in play. Then
    the same work, moving a million particles on by one frame, done to
    particles kept in memory in different ways. The console shows how
    long each took.

    Build it in Release, and run it without the debugger (Ctrl+F5). A
    Debug build is slower, and slower by different amounts for different
    code, so its times don't compare.
*/

#include <SDL3/SDL.h>

#include <algorithm>    // std::sort
#include <functional>   // std::function, for the work to be timed
#include <iomanip>      // std::setw and std::setprecision
#include <iostream>     // std::cout
#include <list>         // std::list, a linked list
#include <memory>       // std::unique_ptr
#include <string>       // std::string and std::to_string
#include <utility>      // std::swap
#include <vector>       // std::vector

// The space the particles bounce around in, and the fountain's physics,
// all from Chapter 14
const int   WINDOW_W     = 800;
const int   WINDOW_H     = 600;
const float GRAVITY      = 600.0f;   // pixels per second, per second
const float FLOOR_BOUNCE = 0.6f;     // speed kept after hitting the floor

// The tests
const int   COUNT = 1000000;         // a million particles in each test
const float DELTA = 1.0f / 60.0f;    // one frame, at 60 frames a second
const int   TRIES = 15;              // how many times each test runs

// Chapter 14's particle: a colored square with a position and a velocity
struct Particle
{
    float x;           // the top-left corner, in pixels
    float y;
    float velX;        // the velocity, in pixels per second
    float velY;
    float size;        // the width and height, in pixels
    SDL_Color color;
    float life;        // seconds left before it fades away
};

// A random number from low up to high, as in Chapter 14
float randomBetween(float low, float high)
{
    return low + SDL_randf() * (high - low);
}

// A particle somewhere in the window, flying in a random direction, like
// one from Chapter 14's fountain after it has been running a while
Particle randomParticle()
{
    Particle p{};
    p.size = randomBetween(8.0f, 16.0f);
    p.x = randomBetween(0.0f, WINDOW_W - p.size);
    p.y = randomBetween(0.0f, WINDOW_H - p.size);
    p.velX = randomBetween(-400.0f, 400.0f);
    p.velY = randomBetween(-400.0f, 400.0f);
    p.color = { 200, 150, 100, 255 };
    p.life = 3.0f;
    return p;
}

// Chapter 14's moveParticle, taking a reference rather than a pointer, so
// it can move a particle wherever it's kept
void moveParticle(Particle& p, float delta)
{
    p.velY += GRAVITY * delta;
    p.life -= delta;

    p.x += p.velX * delta;
    p.y += p.velY * delta;

    // The left and right walls
    if (p.x < 0.0f)
    {
        p.x = 0.0f;
        p.velX = -p.velX;
    }
    else if (p.x + p.size > WINDOW_W)
    {
        p.x = WINDOW_W - p.size;
        p.velX = -p.velX;
    }

    // The top and bottom walls
    if (p.y < 0.0f)
    {
        p.y = 0.0f;
        p.velY = -p.velY;
    }
    else if (p.y + p.size > WINDOW_H)
    {
        p.y = WINDOW_H - p.size;
        p.velY = -p.velY * FLOOR_BOUNCE;
    }
}

// How long some work takes, in milliseconds. It does the work several
// times and keeps the fastest, because anything slower was held up by
// something else the computer was doing
double timeIt(const std::function<void()>& work, int tries = TRIES)
{
    double fastest = 1.0e9;
    for (int i = 0; i < tries; i++)
    {
        Uint64 start = SDL_GetTicksNS();
        work();
        double ms = (SDL_GetTicksNS() - start) / 1.0e6;
        if (ms < fastest)
            fastest = ms;
    }
    return fastest;
}

// One line of the results: what was timed, and how long it took, in
// milliseconds unless it says otherwise
void report(const std::string& what, double time,
            const std::string& unit = "ms")
{
    std::cout << "  " << std::left << std::setw(40) << what << std::right
              << std::setw(7) << time << " " << unit << "\n";
}

// ---- How far away is the data? ---------------------------------------------

// The average time to fetch one number from memory, in nanoseconds, when
// the numbers in play fill this many kilobytes. Each number says where
// the next one is, in a random order, so the processor can't see where
// it's going next, and has to wait for each number to arrive
double timeOneFetch(int kilobytes, int& total)
{
    int count = kilobytes * 256;   // 256 ints to a kilobyte, 4 bytes each

    // Every position, in a random order: walk down from the end, swapping
    // each one with a random one at or before it
    std::vector<int> order(count);
    for (int i = 0; i < count; i++)
        order[i] = i;
    for (int i = count - 1; i > 0; i--)
        std::swap(order[i], order[SDL_rand(i + 1)]);

    // Link them into one big loop, in that order
    std::vector<int> next(count);
    for (int i = 0; i + 1 < count; i++)
        next[order[i]] = order[i + 1];
    next[order[count - 1]] = order[0];

    const int STEPS = 10000000;
    double ms = timeIt([&next, &total]()
    {
        int at = 0;
        for (int step = 0; step < STEPS; step++)
            at = next[at];
        total += at;   // use the answer, so the compiler can't skip the work
    }, 3);

    return ms * 1.0e6 / STEPS;   // milliseconds to nanoseconds, per fetch
}

// ---- Vectors, lists, and pointers --------------------------------------------

// The particles themselves, side by side in one block
double timeVector(std::vector<Particle>& particles)
{
    return timeIt([&particles]()
    {
        for (Particle& p : particles)
            moveParticle(p, DELTA);
    });
}

// A linked list: every particle in a node of its own, allocated alone,
// with pointers to the nodes on either side
double timeList(std::list<Particle>& particles)
{
    return timeIt([&particles]()
    {
        for (Particle& p : particles)
            moveParticle(p, DELTA);
    });
}

// Chapter 14's way: a vector of pointers, each to a particle made with new
double timePointers(std::vector<Particle*>& particles)
{
    return timeIt([&particles]()
    {
        for (Particle* p : particles)
            moveParticle(*p, DELTA);
    });
}

// The same, with smart pointers
double timeUniquePointers(std::vector<std::unique_ptr<Particle>>& particles)
{
    return timeIt([&particles]()
    {
        for (std::unique_ptr<Particle>& p : particles)
            moveParticle(*p, DELTA);
    });
}

// For sorting particles by how far across the window they are, which
// scrambles the order they're visited in, as a real game's comings and
// goings do over time
bool isLeftOf(const Particle& a, const Particle& b)
{
    return a.x < b.x;
}

// ---- Structures and arrays -----------------------------------------------------

// The same particles, kept as a structure of arrays: one array for each
// member of Particle, so particle i is x[i], y[i], and so on
struct ParticleArrays
{
    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> velX;
    std::vector<float> velY;
    std::vector<float> size;
    std::vector<SDL_Color> color;
    std::vector<float> life;
};

// Copy each particle's members into the arrays
ParticleArrays toArrays(const std::vector<Particle>& particles)
{
    ParticleArrays arrays;
    for (const Particle& p : particles)
    {
        arrays.x.push_back(p.x);
        arrays.y.push_back(p.y);
        arrays.velX.push_back(p.velX);
        arrays.velY.push_back(p.velY);
        arrays.size.push_back(p.size);
        arrays.color.push_back(p.color);
        arrays.life.push_back(p.life);
    }
    return arrays;
}

// Only the positions, from the velocities: the particles as structures
double timeStructPositions(std::vector<Particle>& particles)
{
    return timeIt([&particles]()
    {
        for (Particle& p : particles)
        {
            p.x += p.velX * DELTA;
            p.y += p.velY * DELTA;
        }
    });
}

// Only the positions, from the velocities: the particles as arrays
double timeArrayPositions(ParticleArrays& a)
{
    return timeIt([&a]()
    {
        for (size_t i = 0; i < a.x.size(); i++)
        {
            a.x[i] += a.velX[i] * DELTA;
            a.y[i] += a.velY[i] * DELTA;
        }
    });
}

// Everything moveParticle does, done to the arrays
void moveArrays(ParticleArrays& a, float delta)
{
    for (size_t i = 0; i < a.x.size(); i++)
    {
        a.velY[i] += GRAVITY * delta;
        a.life[i] -= delta;
        a.x[i] += a.velX[i] * delta;
        a.y[i] += a.velY[i] * delta;

        if (a.x[i] < 0.0f)
        {
            a.x[i] = 0.0f;
            a.velX[i] = -a.velX[i];
        }
        else if (a.x[i] + a.size[i] > WINDOW_W)
        {
            a.x[i] = WINDOW_W - a.size[i];
            a.velX[i] = -a.velX[i];
        }

        if (a.y[i] < 0.0f)
        {
            a.y[i] = 0.0f;
            a.velY[i] = -a.velY[i];
        }
        else if (a.y[i] + a.size[i] > WINDOW_H)
        {
            a.y[i] = WINDOW_H - a.size[i];
            a.velY[i] = -a.velY[i] * FLOOR_BOUNCE;
        }
    }
}

double timeArrays(ParticleArrays& a)
{
    return timeIt([&a]()
    {
        moveArrays(a, DELTA);
    });
}

// ---- Hot and cold ----------------------------------------------------------------

// What the game hardly ever reads about a particle: where and when it was
// born, which burst it came from, and a label for the debugger
struct ParticleHistory
{
    float bornX;
    float bornY;
    float bornAt;      // seconds after the game started
    int burst;
    std::string label;
};

// A particle that carries its history around with it
struct FatParticle
{
    Particle particle;          // read every frame
    ParticleHistory history;    // read almost never
};

double timeFat(std::vector<FatParticle>& particles)
{
    return timeIt([&particles]()
    {
        for (FatParticle& f : particles)
            moveParticle(f.particle, DELTA);
    });
}

// ---- Two kinds of particle --------------------------------------------------------

// A bubble's move: no gravity, and it rises or sinks slowly
void floatParticle(Particle& p, float delta)
{
    p.life -= delta;
    p.x += p.velX * delta;
    p.y += p.velY * 0.25f * delta;
}

// A particle that moves itself, through a virtual function, as Chapter
// 28's inputs decide for their runners
class MovingParticle
{
public:
    virtual ~MovingParticle() = default;
    virtual void move(float delta) = 0;

    Particle particle;
};

// One that falls, like the fountain's
class FallingParticle : public MovingParticle
{
public:
    void move(float delta) override
    {
        moveParticle(particle, delta);
    }
};

// One that floats, like a bubble
class FloatingParticle : public MovingParticle
{
public:
    void move(float delta) override
    {
        floatParticle(particle, delta);
    }
};

// The two kinds, for a switch to choose between
enum class Kind
{
    Falling,
    Floating
};

// A particle of either kind, made on the heap, which moves itself
std::unique_ptr<MovingParticle> makeMoving(const Particle& p, Kind kind)
{
    std::unique_ptr<MovingParticle> moving;
    if (kind == Kind::Falling)
        moving = std::make_unique<FallingParticle>();
    else
        moving = std::make_unique<FloatingParticle>();
    moving->particle = p;
    return moving;
}

double timeVirtual(std::vector<std::unique_ptr<MovingParticle>>& particles)
{
    return timeIt([&particles]()
    {
        for (std::unique_ptr<MovingParticle>& p : particles)
            p->move(DELTA);
    });
}

// The other way to have two kinds: say which kind each one is, and switch
struct KindOfParticle
{
    Particle particle;
    Kind kind;
};

double timeSwitch(std::vector<KindOfParticle>& particles)
{
    return timeIt([&particles]()
    {
        for (KindOfParticle& k : particles)
        {
            switch (k.kind)
            {
            case Kind::Falling:
                moveParticle(k.particle, DELTA);
                break;
            case Kind::Floating:
                floatParticle(k.particle, DELTA);
                break;
            }
        }
    });
}

// Or keep each kind in a vector of its own, and never ask
double timeTwoVectors(std::vector<Particle>& falling,
                      std::vector<Particle>& floating)
{
    return timeIt([&falling, &floating]()
    {
        for (Particle& p : falling)
            moveParticle(p, DELTA);
        for (Particle& p : floating)
            floatParticle(p, DELTA);
    });
}

int main()
{
    SDL_srand(29);   // the same random numbers every time it runs
    std::cout << std::fixed << std::setprecision(2);

    // How far away is the data?
    std::cout << "Fetching one number from memory, with this much in play:\n";
    int total = 0;
    for (int kilobytes : { 16, 64, 256, 1024, 4096, 16384, 65536, 262144 })
    {
        std::string amount = std::to_string(kilobytes) + " KB";
        if (kilobytes >= 1024)
            amount = std::to_string(kilobytes / 1024) + " MB";
        report(amount, timeOneFetch(kilobytes, total), "ns");
    }

    // A million particles, made once. Every test starts from a copy of
    // these, so they all do exactly the same work
    std::vector<Particle> start;
    for (int i = 0; i < COUNT; i++)
        start.push_back(randomParticle());

    std::cout << "\nMoving " << COUNT << " particles on by one frame:\n";
    std::vector<Particle> particles = start;
    report("a vector of particles", timeVector(particles));

    std::vector<Particle> sorted = start;
    std::sort(sorted.begin(), sorted.end(), isLeftOf);
    report("a vector of particles, sorted", timeVector(sorted));

    std::list<Particle> list(start.begin(), start.end());
    report("a list of particles", timeList(list));

    std::list<Particle> sortedList(start.begin(), start.end());
    sortedList.sort(isLeftOf);
    report("a list of particles, sorted", timeList(sortedList));

    std::vector<Particle*> pointers;
    for (const Particle& p : start)
        pointers.push_back(new Particle(p));
    report("a vector of pointers", timePointers(pointers));
    for (Particle* p : pointers)
        delete p;

    std::vector<Particle*> sortedPointers;
    for (const Particle& p : start)
        sortedPointers.push_back(new Particle(p));
    std::sort(sortedPointers.begin(), sortedPointers.end(),
              [](const Particle* a, const Particle* b)
              {
                  return isLeftOf(*a, *b);
              });
    report("a vector of pointers, sorted", timePointers(sortedPointers));
    for (Particle* p : sortedPointers)
        delete p;

    std::vector<std::unique_ptr<Particle>> owners;
    for (const Particle& p : start)
        owners.push_back(std::make_unique<Particle>(p));
    std::sort(owners.begin(), owners.end(),
              [](const std::unique_ptr<Particle>& a,
                 const std::unique_ptr<Particle>& b)
              {
                  return isLeftOf(*a, *b);
              });
    report("a vector of unique_ptrs, sorted", timeUniquePointers(owners));

    // Structures, and arrays
    std::cout << "\nStructures and arrays:\n";
    std::vector<Particle> structs = start;
    report("positions only, as structures", timeStructPositions(structs));
    ParticleArrays arrays = toArrays(start);
    report("positions only, as arrays", timeArrayPositions(arrays));

    structs = start;
    report("everything, as structures", timeVector(structs));
    arrays = toArrays(start);
    report("everything, as arrays", timeArrays(arrays));

    // Hot and cold
    std::cout << "\nHot and cold (a Particle is " << sizeof(Particle)
              << " bytes, a FatParticle " << sizeof(FatParticle) << "):\n";
    std::vector<FatParticle> fat(COUNT);
    for (int i = 0; i < COUNT; i++)
        fat[i].particle = start[i];
    report("fat particles, history inside", timeFat(fat));

    std::vector<Particle> hot = start;
    std::vector<ParticleHistory> cold(COUNT);
    report("particles, with histories kept apart", timeVector(hot));

    // Two kinds of particle: a coin toss decides each one's kind
    std::vector<Kind> kinds;
    for (int i = 0; i < COUNT; i++)
        kinds.push_back(SDL_rand(2) == 0 ? Kind::Falling : Kind::Floating);

    // Each kind in a vector of its own, and the kinds mixed, as the coins
    // fell, in a vector that says which kind each one is
    std::vector<Particle> falling;
    std::vector<Particle> floating;
    std::vector<KindOfParticle> mixedKinds;
    for (int i = 0; i < COUNT; i++)
    {
        if (kinds[i] == Kind::Falling)
            falling.push_back(start[i]);
        else
            floating.push_back(start[i]);
        mixedKinds.push_back({ start[i], kinds[i] });
    }

    // The same, in blocks: every falling one, then every floating one
    std::vector<KindOfParticle> blockKinds;
    for (const Particle& p : falling)
        blockKinds.push_back({ p, Kind::Falling });
    for (const Particle& p : floating)
        blockKinds.push_back({ p, Kind::Floating });

    // And three vectors of particles that move themselves: in blocks, mixed,
    // and mixed and then sorted, each made in a loop of its own
    std::vector<std::unique_ptr<MovingParticle>> blocks;
    for (const KindOfParticle& k : blockKinds)
        blocks.push_back(makeMoving(k.particle, k.kind));

    std::vector<std::unique_ptr<MovingParticle>> mixed;
    for (const KindOfParticle& k : mixedKinds)
        mixed.push_back(makeMoving(k.particle, k.kind));

    std::vector<std::unique_ptr<MovingParticle>> mixedSorted;
    for (const KindOfParticle& k : mixedKinds)
        mixedSorted.push_back(makeMoving(k.particle, k.kind));
    std::sort(mixedSorted.begin(), mixedSorted.end(),
              [](const std::unique_ptr<MovingParticle>& a,
                 const std::unique_ptr<MovingParticle>& b)
              {
                  return isLeftOf(a->particle, b->particle);
              });

    std::cout << "\nTwo kinds of particle, half falling, half floating:\n";
    report("two vectors, one for each kind", timeTwoVectors(falling, floating));
    report("switch, the kinds in blocks", timeSwitch(blockKinds));
    report("switch, the kinds mixed", timeSwitch(mixedKinds));
    report("virtual, the kinds in blocks", timeVirtual(blocks));
    report("virtual, the kinds mixed", timeVirtual(mixed));
    report("virtual, the kinds mixed, sorted", timeVirtual(mixedSorted));

    std::cout << "\n(All the fetches' answers, so none could be skipped: "
              << total << ")\n";
    return 0;
}
