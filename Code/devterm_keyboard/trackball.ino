/*
 * ClockworkPi DevTerm Trackball
 */
#include "keys_io_map.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <USBComposite.h>

#include "trackball.h"
#include "math.h"

// Choose the type of filter (add a `_` to the #define you're not using):
// - FIR uses more memory and takes longer to run, but you can tweak the FILTER_SIZE
// - IIR is faster (has less coefficients), but the coefficients are pre-calculated (via scipy)
#define _USE_FIR
#define USE_IIR

// Enable debug messages via serial
#define _DEBUG_TRACKBALL

// Source: https://github.com/dangpzanco/dsp
#include <dsp_filters.h>

// Simple trackball pin direction counter
enum TrackballPin : uint8_t
{
    PIN_LEFT,
    PIN_RIGHT,
    PIN_UP,
    PIN_DOWN,
    PIN_NUM,
};
static uint8_t direction_counter[PIN_NUM] = {0};

// Mouse and Wheel sensitivity values
static const float MOUSE_SENSITIVITY = 10.0f;
static const float WHEEL_SENSITIVITY = 0.25f;

#ifdef USE_IIR
// Infinite Impulse Response (IIR) Filter
// Filter design (https://docs.scipy.org/doc/scipy/reference/signal.html):
// Low-pass Butterworth filter [b, a = scipy.signal.butter(N=2, Wn=0.1)]
static const int8_t IIR_SIZE = 3;
static float iir_coeffs_b[IIR_SIZE] = {0.020083365564211232, 0.040166731128422464, 0.020083365564211232};
static float iir_coeffs_a[IIR_SIZE] = {1.0, -1.5610180758007182, 0.6413515380575631};
IIR iir_x, iir_y;
#endif

#ifdef USE_FIR
// FIR Filter
static const int8_t FILTER_SIZE = 10;
static float fir_coeffs[FILTER_SIZE];
FIR fir_x, fir_y;

static void init_fir()
{
    // Moving Average Finite Impulse Response (FIR) Filter:
    // - Smooths out corners (the trackball normally only moves in 90 degree angles)
    // - Filters out noisy data (avoids glitchy movements)
    // - Adds delay to the movement. To tweak this:
    //    1. Change the FILTER_SIZE (delay is proportional to the size, use millis() to measure time)
    //    2. Redesign the filter with the desired delay
    for (int8_t i = 0; i < FILTER_SIZE; i++)
        fir_coeffs[i] = 1.0f / FILTER_SIZE;
}
#endif

#ifdef DEBUG_TRACKBALL
static uint32_t time = millis();

// Useful debug function
template <typename T>
void print_vec(DEVTERM *dv, T *vec, uint32_t size, bool newline = true)
{
    for (int8_t i = 0; i < size - 1; i++)
    {
        dv->_Serial->print(vec[i]);
        dv->_Serial->print(",");
    }
    if (newline)
    {
        dv->_Serial->println(vec[size - 1]);
    }
    else
    {
        dv->_Serial->print(vec[size - 1]);
        dv->_Serial->print(",");
    }
}
#endif

template <TrackballPin PIN>
static void interrupt()
{
    // Count the number of times the trackball rolls towards a certain direction
    // (when the corresponding PIN changes its value). This part of the code should be minimal,
    // so that the next interrupts are not blocked from happening.
    direction_counter[PIN] += 1;
}

static float position_scale(float x)
{
    // Exponential scaling of the mouse movement:
    // - Small values remain small (precise movement)
    // - Slightly larger values get much larger (fast movement)
    // This function may be tweaked further, but it's good enough for now.
    return MOUSE_SENSITIVITY * sign(x) * std::exp(std::abs(x) / std::sqrt(MOUSE_SENSITIVITY));
}

void trackball_task(DEVTERM *dv)
{

#ifdef DEBUG_TRACKBALL
    // Measure elapsed time
    uint32_t elapsed = millis() - time;
    time += elapsed;

    // Send raw data via serial (CSV format)
    dv->_Serial->print(elapsed);
    dv->_Serial->print(",");
    print_vec(dv, direction_counter, PIN_NUM);
#endif


    // Stop interrupts from happening. Don't forget to re-enable them!
    noInterrupts();

    // Calculate x and y positions
    float x = direction_counter[PIN_RIGHT] - direction_counter[PIN_LEFT];
    float y = direction_counter[PIN_DOWN] - direction_counter[PIN_UP];

    // Clear counters
    // memset(direction_counter, 0, sizeof(direction_counter));
    std::fill(std::begin(direction_counter), std::end(direction_counter), 0);

    // Re-enable interrupts (Mouse.move needs interrupts)
    interrupts();

    // Non-linear scaling
    x = position_scale(x);
    y = position_scale(y);

    // Wheel rolls with the (reverse) vertical axis (no filter needed)
    int8_t w = clamp<int8_t>(-y * WHEEL_SENSITIVITY);

    // Filter x and y
#ifdef USE_FIR
    x = fir_filt(&fir_x, x);
    y = fir_filt(&fir_y, y);
#endif
#ifdef USE_IIR
    x = iir_filt(&iir_x, x);
    y = iir_filt(&iir_y, y);
#endif

    // Move Trackball (either Mouse or Wheel)
    switch (dv->state->moveTrackball())
    {
    case TrackballMode::Mouse:
    {
        // Move mouse
        while ((int)x != 0 || (int)y != 0)
        {
            // Only 8bit values are allowed,
            // so clamp and execute move() multiple times
            int8_t x_byte = clamp<int8_t>(x);
            int8_t y_byte = clamp<int8_t>(y);

            // Move mouse with values in the range [-128, 127]
            dv->Mouse->move(x_byte, y_byte, 0);

            // Decrement the original value, stop if done
            x -= x_byte;
            y -= y_byte;
        }

        break;
    }
    case TrackballMode::Wheel:
    {
        if (w != 0)
        {
            // Only scroll the wheel [move cursor by (0,0)]
            dv->Mouse->move(0, 0, w);
            dv->state->setScrolled();
        }

        break;
    }
    }
}

void trackball_init(DEVTERM *dv)
{
    // Enable trackball pins
    pinMode(LEFT_PIN, INPUT);
    pinMode(UP_PIN, INPUT);
    pinMode(RIGHT_PIN, INPUT);
    pinMode(DOWN_PIN, INPUT);

    // Initialize filters
#ifdef USE_FIR
    init_fir();
    fir_init(&fir_x, FILTER_SIZE, fir_coeffs);
    fir_init(&fir_y, FILTER_SIZE, fir_coeffs);
#endif
#ifdef USE_IIR
    iir_init(&iir_x, IIR_SIZE, iir_coeffs_b, IIR_SIZE, iir_coeffs_a);
    iir_init(&iir_y, IIR_SIZE, iir_coeffs_b, IIR_SIZE, iir_coeffs_a);
#endif

    // Run interrupt function when corresponding PIN changes its value
    attachInterrupt(LEFT_PIN, &interrupt<PIN_LEFT>, ExtIntTriggerMode::CHANGE);
    attachInterrupt(RIGHT_PIN, &interrupt<PIN_RIGHT>, ExtIntTriggerMode::CHANGE);
    attachInterrupt(UP_PIN, &interrupt<PIN_UP>, ExtIntTriggerMode::CHANGE);
    attachInterrupt(DOWN_PIN, &interrupt<PIN_DOWN>, ExtIntTriggerMode::CHANGE);

}
