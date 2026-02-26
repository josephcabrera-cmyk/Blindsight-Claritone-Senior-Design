// spatial_tone.cpp
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <thread>

struct Vec3 { float x, y, z; };

static inline float clampf(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}
static inline float vlen(const Vec3& v) {
    return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
static inline Vec3 normalize(const Vec3& v) {
    float L = vlen(v);
    if (L <= 1e-6f) return {0.f, 0.f, 1.f};
    return {v.x/L, v.y/L, v.z/L};
}

struct SpatialToneState {
    // Direction vector (x right, y up, z forward)
    std::atomic<float> dirX{0.f};
    std::atomic<float> dirY{0.f};
    std::atomic<float> dirZ{1.f};

    // Distance in arbitrary units (>= nearDist)
    std::atomic<float> distance{1.f};

    // Tone params
    std::atomic<float> frequencyHz{440.f};  // A4
    std::atomic<float> baseGain{0.2f};      // overall loudness

    // Spatial params
    std::atomic<float> panStrength{1.0f};   // 0..1
    std::atomic<float> nearDist{0.5f};      // clamp distance lower bound
    std::atomic<float> rolloff{1.0f};       // 0.5..2.0-ish
};

struct EngineCtx {
    SpatialToneState* st = nullptr;
    uint32_t sampleRate = 48000;

    // oscillator phase
    double phase = 0.0;

    // small delay lines for ITD
    static constexpr int MaxDelaySamples = 64; // ~1.3ms @ 48k
    float delayL[MaxDelaySamples]{};
    float delayR[MaxDelaySamples]{};
    int writeIndex = 0;
};

static void audio_cb(ma_device* device, void* output, const void*, ma_uint32 frameCount)
{
    auto* ctx = reinterpret_cast<EngineCtx*>(device->pUserData);
    float* out = reinterpret_cast<float*>(output);
    const ma_uint32 ch = device->playback.channels; // expect 2

    // Load params
    Vec3 d{
        ctx->st->dirX.load(std::memory_order_relaxed),
        ctx->st->dirY.load(std::memory_order_relaxed),
        ctx->st->dirZ.load(std::memory_order_relaxed)
    };
    d = normalize(d);

    float dist     = ctx->st->distance.load(std::memory_order_relaxed);
    float nearDist = ctx->st->nearDist.load(std::memory_order_relaxed);
    float rolloff  = ctx->st->rolloff.load(std::memory_order_relaxed);
    float freq     = ctx->st->frequencyHz.load(std::memory_order_relaxed);
    float baseGain = ctx->st->baseGain.load(std::memory_order_relaxed);
    float panStr   = clampf(ctx->st->panStrength.load(std::memory_order_relaxed), 0.f, 1.f);

    // Clamp
    nearDist = clampf(nearDist, 0.05f, 10.f);
    dist = (dist < nearDist) ? nearDist : dist;
    rolloff = clampf(rolloff, 0.1f, 4.0f);
    freq = clampf(freq, 20.f, 20000.f);
    baseGain = clampf(baseGain, 0.f, 1.5f);

    // Distance attenuation (inverse power)
    // gain = (nearDist / dist)^rolloff
    // - rolloff=2 feels like inverse-square
    float distGain = std::pow(nearDist / dist, rolloff);
    distGain = clampf(distGain, 0.f, 1.f);

    // Azimuth from x/z gives left-right positioning
    float az = std::atan2(d.x, d.z);
    float pan = clampf(std::sin(az) * panStr, -1.f, 1.f);

    // Equal-power panning (ILD-ish)
    float gL = std::sqrt(0.5f * (1.f - pan));
    float gR = std::sqrt(0.5f * (1.f + pan));

    // Simple head shadow: reduce far ear a bit as pan increases
    float shadow = 1.0f - 0.25f * std::fabs(pan);
    if (pan > 0) gL *= shadow; else gR *= shadow;

    // Optional ITD delay
    float maxITDsec = 0.0007f; // 0.7 ms
    float itdSec = maxITDsec * pan; // right => right leads (left delayed)
    int delaySamples = (int)std::round(std::fabs(itdSec) * (float)ctx->sampleRate);
    delaySamples = std::min(delaySamples, EngineCtx::MaxDelaySamples - 1);

    // Final gain
    float gain = baseGain * distGain;

    // Render
    const double twoPi = 6.283185307179586;
    double phaseInc = twoPi * (double)freq / (double)ctx->sampleRate;

    for (ma_uint32 i = 0; i < frameCount; ++i) {
        // Generate sine
        float s = (float)std::sin(ctx->phase);
        ctx->phase += phaseInc;
        if (ctx->phase >= twoPi) ctx->phase -= twoPi;

        s *= gain;

        // Delay lines store same source sample
        ctx->delayL[ctx->writeIndex] = s;
        ctx->delayR[ctx->writeIndex] = s;

        int readIndex = ctx->writeIndex - delaySamples;
        if (readIndex < 0) readIndex += EngineCtx::MaxDelaySamples;

        float sL = s, sR = s;
        if (pan > 0)      sL = ctx->delayL[readIndex]; // delay left
        else if (pan < 0) sR = ctx->delayR[readIndex]; // delay right

        out[i*ch + 0] = sL * gL;
        out[i*ch + 1] = sR * gR;

        ctx->writeIndex = (ctx->writeIndex + 1) % EngineCtx::MaxDelaySamples;
    }
}

int main()
{
    SpatialToneState st;
    EngineCtx ctx;
    ctx.st = &st;

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;
    config.sampleRate        = 48000;
    config.dataCallback      = audio_cb;
    config.pUserData         = &ctx;

    ma_device device;
    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        std::cerr << "Failed to init audio device.\n";
        return 1;
    }
    ctx.sampleRate = device.sampleRate;

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Failed to start audio device.\n";
        ma_device_uninit(&device);
        return 1;
    }

    std::cout << "Playing generated spatial tone.\n"
              << "Controls (updates every ~30ms):\n"
              << "  A/D: rotate left/right\n"
              << "  W/S: distance closer/farther\n"
              << "  Q/E: quieter/louder (baseGain)\n"
              << "  Z/C: lower/higher frequency\n"
              << "  X  : reset direction forward\n"
              << "  Ctrl+C to quit\n";

    // Simple interactive loop (no raw keyboard; uses std::cin line input)
    // Type characters then press Enter.
    while (true) {
        std::string line;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        for (char c : line) {
            // Read current values
            Vec3 d{st.dirX.load(), st.dirY.load(), st.dirZ.load()};
            d = normalize(d);
            float dist = st.distance.load();
            float bg = st.baseGain.load();
            float f = st.frequencyHz.load();

            // yaw rotate around Y axis
            auto yaw = [&](float radians) {
                float cs = std::cos(radians);
                float sn = std::sin(radians);
                Vec3 nd{
                    d.x * cs + d.z * sn,
                    d.y,
                    -d.x * sn + d.z * cs
                };
                nd = normalize(nd);
                st.dirX.store(nd.x);
                st.dirY.store(nd.y);
                st.dirZ.store(nd.z);
            };

            switch (c) {
                case 'a': case 'A': yaw(-0.15f); break;
                case 'd': case 'D': yaw(+0.15f); break;

                case 'w': case 'W': dist = std::max(0.1f, dist - 0.2f); st.distance.store(dist); break;
                case 's': case 'S': dist = dist + 0.2f; st.distance.store(dist); break;

                case 'q': case 'Q': bg = clampf(bg - 0.05f, 0.f, 1.5f); st.baseGain.store(bg); break;
                case 'e': case 'E': bg = clampf(bg + 0.05f, 0.f, 1.5f); st.baseGain.store(bg); break;

                case 'z': case 'Z': f = clampf(f - 25.f, 20.f, 20000.f); st.frequencyHz.store(f); break;
                case 'c': case 'C': f = clampf(f + 25.f, 20.f, 20000.f); st.frequencyHz.store(f); break;

                case 'x': case 'X':
                    st.dirX.store(0.f); st.dirY.store(0.f); st.dirZ.store(1.f);
                    break;

                default: break;
            }

            std::cout << "dir=("
                      << st.dirX.load() << ","
                      << st.dirY.load() << ","
                      << st.dirZ.load() << ")  dist=" << st.distance.load()
                      << "  baseGain=" << st.baseGain.load()
                      << "  freq=" << st.frequencyHz.load()
                      << "\n";
        }
    }

    ma_device_uninit(&device);
    return 0;
}