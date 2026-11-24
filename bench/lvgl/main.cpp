/*
 * main.cpp -- lh against LVGL on the scenes of scene.h.
 *
 *   bench_lh_vs_lvgl                       frame times, Google Benchmark's table
 *   bench_lh_vs_lvgl --dump=<dir>          one PPM per scene and library, to look at
 *                                          and see that the two draw the same picture
 */

#include "scene.h"

#include <benchmark/benchmark.h>

#include <cstdio>
#include <cstring>
#include <string>

extern "C" const char *const scene_paragraph =
    "The sensor reports every five seconds over Modbus TCP and keeps the last hour of "
    "readings in memory, so a window that opens late still shows a full chart.";

namespace
{
const char *const scene_names[SCENE_COUNT] = {"cards",  "grad_vertical", "grad_diagonal", "grad_radial",
                                              "shadow", "text",          "flex",          "corners"};

struct library
{
    const char *name;
    int (*has)(int);
    void (*setup)(int);
    void (*frame)();
    void (*frame_partial)();
    const uint32_t *(*pixels)();
    void (*teardown)();
};

const library libraries[] = {
    {"lh", lh_scene_has, lh_scene_setup, lh_scene_frame, lh_scene_frame_partial, lh_scene_pixels,
     lh_scene_teardown},
    {"lvgl", lvgl_scene_has, lvgl_scene_setup, lvgl_scene_frame, lvgl_scene_frame_partial, lvgl_scene_pixels,
     lvgl_scene_teardown},
};

void
dump(const std::string &dir)
{
    for (const library &lib : libraries)
    {
        for (int scene = 0; scene < SCENE_COUNT; ++scene)
        {
            if (!lib.has(scene))
            {
                continue;
            }
            lib.setup(scene);
            lib.frame();
            const std::string path = dir + "/" + scene_names[scene] + "_" + lib.name + ".ppm";
            FILE *out = std::fopen(path.c_str(), "wb");
            if (out != nullptr)
            {
                const uint32_t *px = lib.pixels();
                std::fprintf(out, "P6\n%d %d\n255\n", SCENE_W, SCENE_H);
                for (int i = 0; i < SCENE_W * SCENE_H; ++i)
                {
                    const unsigned char rgb[3] = {(unsigned char)(px[i] >> 16), (unsigned char)(px[i] >> 8),
                                                  (unsigned char)px[i]};
                    std::fwrite(rgb, 1, 3, out);
                }
                std::fclose(out);
                std::printf("%s\n", path.c_str());
            }
            lib.teardown();
        }
    }
}

void
register_all()
{
    for (const char *mode : {"full", "partial"})
    {
        const bool whole = std::strcmp(mode, "full") == 0;
        for (int scene = 0; scene < SCENE_COUNT; ++scene)
        {
            for (const library &lib : libraries)
            {
                if (!lib.has(scene))
                {
                    continue;
                }
                const std::string name = std::string(mode) + "/" + scene_names[scene] + "/" + lib.name;
                const library *which = &lib;
                benchmark::RegisterBenchmark(name.c_str(), [which, scene, whole](benchmark::State &state) {
                    which->setup(scene);
                    for (auto _ : state)
                    {
                        if (whole)
                        {
                            which->frame();
                        }
                        else
                        {
                            which->frame_partial();
                        }
                        benchmark::DoNotOptimize(which->pixels()[0]);
                    }
                    which->teardown();
                })->Unit(benchmark::kMicrosecond);
            }
        }
    }
    benchmark::RegisterBenchmark("micro/text/lh", [](benchmark::State &state) {
        for (auto _ : state)
        {
            lh_micro_text();
            benchmark::DoNotOptimize(lh_scene_pixels()[0]);
        }
    })->Unit(benchmark::kNanosecond);
    for (int kind = 0; kind < 4; ++kind)
    {
        static const char *const kinds[] = {"solid", "down", "diagonal", "radial"};
        for (int radius : {0, 8})
        {
            const std::string name = std::string("micro/box/") + kinds[kind] + "/r" + std::to_string(radius);
            benchmark::RegisterBenchmark(name.c_str(), [kind, radius](benchmark::State &state) {
                for (auto _ : state)
                {
                    lh_micro_box(kind, radius);
                    benchmark::DoNotOptimize(lh_scene_pixels()[0]);
                }
            })->Unit(benchmark::kNanosecond);
        }
    }
}
} // namespace

int
main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (std::strncmp(argv[i], "--dump=", 7) == 0)
        {
            dump(argv[i] + 7);
            return 0;
        }
    }
    register_all();
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
