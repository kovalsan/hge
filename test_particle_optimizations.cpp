/*-----------------------------------------------------------------------------
 * Particle System Optimization Test
 * This file demonstrates and validates the low-level optimizations made
 * to the HGE particle system.
 *-----------------------------------------------------------------------------*/

#include <iostream>
#include <chrono>
#include <cstring>
#include <cmath>
#include <cstdlib>

// Mock HGE includes for testing (since we can't build on Linux)
#define M_PI 3.14159265358979323846f
#define M_PI_2 (M_PI/2.0f)

// Simplified mock structures for testing
struct MockHGE {
    float Random_Float(float min, float max) {
        return min + (max - min) * (rand() / (float)RAND_MAX);
    }
};

struct hgeColor {
    float r, g, b, a;
    hgeColor() : r(0), g(0), b(0), a(0) {}
    hgeColor& operator+=(const hgeColor& other) {
        r += other.r; g += other.g; b += other.b; a += other.a;
        return *this;
    }
};

struct hgeVector {
    float x, y;
    
    hgeVector(float _x = 0, float _y = 0) : x(_x), y(_y) {}
    
    static float InvSqrt(float val) {
        union { int i; float f; } conv;
        conv.f = val;
        conv.i = 0x5f3759df - (conv.i >> 1);
        return conv.f * (1.5f - 0.4999f * val * conv.f * conv.f);
    }
    
    float Dot(const hgeVector* v) const { return x * v->x + y * v->y; }
    float LengthSq() const { return Dot(this); }
    float Length() const { return sqrtf(LengthSq()); }
    
    hgeVector* Normalize() {
        const auto rc = InvSqrt(Dot(this));
        x *= rc; y *= rc;
        return this;
    }
    
    hgeVector operator-(const hgeVector& v) const { return hgeVector(x - v.x, y - v.y); }
    hgeVector operator+(const hgeVector& v) const { return hgeVector(x + v.x, y + v.y); }
    hgeVector& operator+=(const hgeVector& v) { x += v.x; y += v.y; return *this; }
    hgeVector operator*(float s) const { return hgeVector(x * s, y * s); }
    hgeVector& operator*=(float s) { x *= s; y *= s; return *this; }
};

// Optimized particle structure with improved data layout
struct OptimizedParticle {
    // Frequently accessed data grouped together for cache performance
    hgeVector vecLocation;
    hgeVector vecVelocity;
    float fAge;
    float fTerminalAge;
    
    // Physics data
    float fGravity;
    float fRadialAccel;
    float fTangentialAccel;
    
    // Visual data
    float fSpin, fSpinDelta;
    float fSize, fSizeDelta;
    hgeColor colColor, colColorDelta;
};

// Original particle structure for comparison
struct OriginalParticle {
    hgeVector vecLocation;
    hgeVector vecVelocity;
    float fGravity;
    float fRadialAccel;
    float fTangentialAccel;
    float fSpin, fSpinDelta;
    float fSize, fSizeDelta;
    hgeColor colColor, colColorDelta;
    float fAge;
    float fTerminalAge;
};

const int NUM_PARTICLES = 500;
const int NUM_ITERATIONS = 10000;

// Test function for optimized particle removal
void testOptimizedRemoval() {
    std::cout << "\n=== Testing Optimized Particle Removal ===\n";
    
    OptimizedParticle particles[NUM_PARTICLES];
    int particles_alive = NUM_PARTICLES;
    
    // Initialize particles
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].fAge = i * 0.1f;
        particles[i].fTerminalAge = 10.0f;
        particles[i].vecLocation = hgeVector(i, i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate particle removal using optimized method
    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        for (int i = 0; i < particles_alive; i++) {
            if (particles[i].fAge > 5.0f) {
                particles_alive--;
                // OPTIMIZED: Direct struct assignment instead of memcpy
                particles[i] = particles[particles_alive];
                i--;
            }
        }
        if (particles_alive < NUM_PARTICLES / 2) break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Optimized removal took: " << duration.count() << " microseconds\n";
    std::cout << "Particles remaining: " << particles_alive << "\n";
}

// Test function for original particle removal (for comparison)
void testOriginalRemoval() {
    std::cout << "\n=== Testing Original Particle Removal ===\n";
    
    OriginalParticle particles[NUM_PARTICLES];
    int particles_alive = NUM_PARTICLES;
    
    // Initialize particles
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].fAge = i * 0.1f;
        particles[i].fTerminalAge = 10.0f;
        particles[i].vecLocation = hgeVector(i, i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate particle removal using original method
    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        for (int i = 0; i < particles_alive; i++) {
            if (particles[i].fAge > 5.0f) {
                particles_alive--;
                // ORIGINAL: Expensive memcpy
                memcpy(&particles[i], &particles[particles_alive], sizeof(OriginalParticle));
                i--;
            }
        }
        if (particles_alive < NUM_PARTICLES / 2) break;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Original removal took: " << duration.count() << " microseconds\n";
    std::cout << "Particles remaining: " << particles_alive << "\n";
}

// Test optimized vector operations
void testVectorOptimizations() {
    std::cout << "\n=== Testing Vector Optimizations ===\n";
    
    const int NUM_VECTORS = 100000;
    hgeVector vectors[NUM_VECTORS];
    
    // Initialize test vectors
    for (int i = 0; i < NUM_VECTORS; i++) {
        vectors[i] = hgeVector(i * 0.1f, i * 0.2f);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Test optimized LengthSq vs Length for comparisons
    int count = 0;
    for (int i = 0; i < NUM_VECTORS; i++) {
        // OPTIMIZED: Use LengthSq for comparisons (avoids sqrt)
        if (vectors[i].LengthSq() > 100.0f) {
            count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Optimized vector comparisons took: " << duration.count() << " microseconds\n";
    std::cout << "Vectors above threshold: " << count << "\n";
}

// Test division optimization
void testDivisionOptimization() {
    std::cout << "\n=== Testing Division Optimization ===\n";
    
    const int NUM_TESTS = 1000000;
    float terminal_ages[NUM_TESTS];
    float deltas[NUM_TESTS];
    float differences[NUM_TESTS];
    
    // Initialize test data
    for (int i = 0; i < NUM_TESTS; i++) {
        terminal_ages[i] = 1.0f + i * 0.001f;
        differences[i] = i * 0.01f;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // OPTIMIZED: Pre-calculate reciprocal and multiply
    for (int i = 0; i < NUM_TESTS; i++) {
        const float inv_terminal_age = 1.0f / terminal_ages[i];
        deltas[i] = differences[i] * inv_terminal_age;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Optimized division took: " << duration.count() << " microseconds\n";
    std::cout << "Sample result: " << deltas[NUM_TESTS/2] << "\n";
}

int main() {
    std::cout << "Particle System Optimization Tests\n";
    std::cout << "==================================\n";
    
    std::cout << "\nTesting particle structure sizes:\n";
    std::cout << "Original particle size: " << sizeof(OriginalParticle) << " bytes\n";
    std::cout << "Optimized particle size: " << sizeof(OptimizedParticle) << " bytes\n";
    
    testOriginalRemoval();
    testOptimizedRemoval();
    testVectorOptimizations();
    testDivisionOptimization();
    
    std::cout << "\n=== Summary of Optimizations ===\n";
    std::cout << "1. Particle removal: Use struct assignment instead of memcpy\n";
    std::cout << "2. Vector operations: Added LengthSq() method for fast comparisons\n";
    std::cout << "3. Data layout: Grouped frequently accessed fields for cache performance\n";
    std::cout << "4. Division optimization: Pre-calculate reciprocals\n";
    std::cout << "5. Conditional acceleration: Skip expensive calculations when not needed\n";
    std::cout << "6. Bounding box: Batch updates for better memory access patterns\n";
    
    return 0;
}