#include "../image.h"
#include "../utils.h"
#include "test0_legacy.hpp"

#include <iostream>
#include <string>
#include <tuple>
#include <ranges>

#include <snitch/snitch.hpp>

using namespace std;

struct Pixel {
    int x;
    int y;
    int ch;

    Pixel(int x, int y, int ch) : x(x), y(y), ch(ch) {
    }
};

TEST_CASE("Test get pixel", "[get_pixel]") {
    Image im = load_image("data/dots.png");

    INFO("Testing image writing");
    SECTION("Test writing image:") {
        auto points = std::vector<Pixel>{
            Pixel(0, 0, 0),
            Pixel(1, 0, 1),
            Pixel(2, 0, 1)
        };
        auto true_values = std::vector<float>{0, 1, 0};
        for (auto i = 0; i < 3; ++i) {
            CAPTURE(i, points[i].x, points[i].y, points[i].ch);
            CHECK(within_eps(true_values[i], im.clamped_pixel(points[i].x, points[i].y, points[i].ch)));
        }
    }
    INFO("Testing padding");
    SECTION("Test padding:") {
        auto points = std::vector<Pixel>{
            Pixel(0, 3, 1),
            Pixel(7, 8, 0),
            Pixel(7, 8, 1),
            Pixel(7, 8, 2)
        };
        auto true_values = std::vector<float>{1, 1, 0, 1};
        for (auto i = 0; i < 4; ++i) {
            CAPTURE(i, points[i].x, points[i].y, points[i].ch);
            CHECK(within_eps(true_values[i], im.clamped_pixel(points[i].x, points[i].y, points[i].ch)));
        }
    }
    std::cout << std::endl;
}

TEST_CASE("Test set pixel", "[set_pixel]") {
    Image im = load_image("data/dots.png");
    Image d(4, 2, 3);

    // Set first row
    d.set_pixel(0, 0, 0, 0);
    d.set_pixel(0, 0, 1, 0);
    d.set_pixel(0, 0, 2, 0);
    d.set_pixel(1, 0, 0, 1);
    d.set_pixel(1, 0, 1, 1);
    d.set_pixel(1, 0, 2, 1);
    d.set_pixel(2, 0, 0, 1);
    d.set_pixel(2, 0, 1, 0);
    d.set_pixel(2, 0, 2, 0);
    d.set_pixel(3, 0, 0, 1);
    d.set_pixel(3, 0, 1, 1);
    d.set_pixel(3, 0, 2, 0);

    // Set second row
    d.set_pixel(0, 1, 0, 0);
    d.set_pixel(0, 1, 1, 1);
    d.set_pixel(0, 1, 2, 0);
    d.set_pixel(1, 1, 0, 0);
    d.set_pixel(1, 1, 1, 1);
    d.set_pixel(1, 1, 2, 1);
    d.set_pixel(2, 1, 0, 0);
    d.set_pixel(2, 1, 1, 0);
    d.set_pixel(2, 1, 2, 1);
    d.set_pixel(3, 1, 0, 1);
    d.set_pixel(3, 1, 1, 0);
    d.set_pixel(3, 1, 2, 1);

    // Test images are same
    CHECK(same_image(im, d));

    std::cout << std::endl;
}

TEST_CASE("Test grayscale conversion", "[grayscale]") {
    Image im = load_image("data/colorbar.png");
    Image gray = rgb_to_grayscale(im);
    Image g = load_image("data/gray.png");
    CHECK(same_image(gray, g));
    std::cout << std::endl;
}

TEST_CASE("Test image copy", "[copy]") {
    Image im = load_image("data/dog.jpg");
    Image c = im;
    CHECK(same_image(im, c));
    std::cout << std::endl;
}

TEST_CASE("Test shift image", "[shift]") {
    Image im = load_image("data/dog.jpg");
    Image c = im;
    shift_image(c, 1, .1);

    SECTION("Test channel 0 unchanged") {
        CHECK(within_eps(im.data[0], c.data[0]));
    }

    SECTION("Test images are different objects") {
        CHECK(&im != &c);
    }

    SECTION("Test channel 1 shifted by 0.1") {
        CHECK(within_eps(im.data[im.w*im.h+13] + .1, c.data[im.w*im.h + 13]));
        CHECK(within_eps(im.data[im.w*im.h+47] + .1, c.data[im.w*im.h + 47]));
    }

    SECTION("Test channel 2 unchanged") {
        CHECK(within_eps(im.data[2*im.w*im.h+72], c.data[2*im.w*im.h + 72]));
    }
    std::cout << std::endl;
}

TEST_CASE("Test scale image", "[scale]") {
    Image im = load_image("data/dog.jpg");
    Image s = im;
    rgb_to_hsv(im);
    rgb_to_hsv(s);
    scale_image(s, 1, 2);

    int ch_size = im.w * im.h;

    SECTION("Test channel 1 scaled by 2") {
        CHECK(within_eps(im.data[ch_size]*2, s.data[ch_size]));
        CHECK(within_eps(im.data[ch_size+72]*2, s.data[ch_size + 72]));
    }

    SECTION("Test other channels unchanged") {
        CHECK(within_eps(im.data[2*ch_size+72], s.data[2*ch_size + 72]));
        CHECK(within_eps(im.data[47], s.data[47]));
    }
    std::cout << std::endl;
}

TEST_CASE("Test RGB to HSV conversion", "[rgb_to_hsv]") {
    Image im = load_image("data/dog.jpg");
    rgb_to_hsv(im);
    Image hsv = load_image("data/dog.hsv.png");
    CHECK(same_image(im, hsv));
    std::cout << std::endl;
}

TEST_CASE("Test HSV to RGB conversion", "[hsv_to_rgb]") {
    Image im = load_image("data/dog.jpg");
    Image c = im;
    rgb_to_hsv(im);
    hsv_to_rgb(im);
    CHECK(same_image(im, c));
    std::cout << std::endl;
}



int main(int argc, char **argv) {
    // Parse the command-line arguments.
    std::optional<snitch::cli::input> args = snitch::cli::parse_arguments(argc, argv);
    if (!args) {
        // Parsing failed, an error has been reported, just return.
        return 1;
    }

    // Configure snitch using command-line options.
    // You can then override the configuration below, or just remove this call to disable
    // command-line options entirely.
    snitch::tests.configure(*args);
    snitch::tests.verbose = snitch::registry::verbosity::full; // change this to ::normal to print only failed tests

    // // Image manipulation for fun testing.
    // Image im2 = load_image("data/dog.jpg");
    // for (int i = 0; i < im2.w; i++)
    //     for (int j = 0; j < im2.h; j++)
    //         im2(i, j, 0) = 0;
    // im2.save_image("output/pixel_modifying_output");

    // To run legacy tests uncomment the following line
    run_tests();
    std::cout << std::endl;

    // Actually run the tests.
    // This will apply any filtering specified on the command-line.
    return snitch::tests.run_tests(*args) ? 0 : 1;
}
