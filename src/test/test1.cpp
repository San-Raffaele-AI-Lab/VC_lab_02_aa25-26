#include "../image.h"
#include "../utils.h"
#include "test1_legacy.hpp"

#include <string>
#include <iostream>
#include <chrono>
#include <snitch/snitch.hpp>

using namespace std;


TEST_CASE("TEST IMAGE RESIZE NN", "[nearest_resize]") {
    Image im = load_image("data/dogsmall.jpg");
    Image im2 = load_image("data/dog.jpg");

    INFO("Test image resize");
    SECTION("Test 1: Resize to 4x") {
        Image resized = nearest_resize(im, im.w * 4, im.h * 4);
        Image gt = load_image("data/dog4x-nn-for-test.png");
        CHECK(same_image(resized, gt));
    }
    SECTION("Test  2: Slightly smaller") {
        Image resized2 = nearest_resize(im2, 713, 467);
        Image gt2 = load_image("data/dog-resize-nn.png");
        CHECK(same_image(resized2, gt2));
    }
    SECTION("Test  3: Identity") {
        Image resized2 = nearest_resize(im2, im2.w, im2.h);
        save_image(resized2, "figs/dog-nn");
        Image gt2 = load_image("data/dog.jpg");
        CHECK(same_image(resized2, gt2));
    }
    // SECTION("Test  4: Very small dog") {
    //     Image resized2 = nearest_resize(im2, 192, 144);
    //     save_image(resized2, "figs/dogmall-nn");
    //     Image gt2 = load_image("data/dogsmall-nn.jpg"); // fixme: this gt file has been low passed!
    //     CHECK(same_image(resized2, gt2));
    // }
}

TEST_CASE("TEST IMAGE RESIZE BL", "[bilinear_resize]") {
    Image im = load_image("data/dogsmall.jpg");
    Image im2 = load_image("data/dog.jpg");

    INFO("Test image resize");
    SECTION("BL Test 1: Resize 4x") {
        Image resized = bilinear_resize(im, im.w * 4, im.h * 4);
        Image gt = load_image("data/dog4x-bl.png");
        CHECK(same_image(resized, gt));
    }
    SECTION("BL Test  2: Slightly smaller") {
        Image resized2 = bilinear_resize(im2, 713, 467);
        save_image(resized2, "figs/dog-slightly-small-bl");
        Image gt2 = load_image("data/dog-resize-bil.png");
        CHECK(same_image(resized2, gt2));
    }
    SECTION("BL Test  3: Identity") {
        Image resized2 = bilinear_resize(im2, im2.w, im2.h);
        save_image(resized2, "figs/dog-bl");
        Image gt2 = load_image("data/dog.jpg");
        CHECK(image_close_enough(resized2, gt2)); // this cannot be exactly the same, unless ones checks for identity in bilinear_resize
    }
    // SECTION("BL Test  4: Small dog") {
    //     Image resized2 = bilinear_resize(im2, 192, 144);
    //     save_image(resized2, "figs/dog-small-bl");
    //     Image gt2 = load_image("data/dogsmall-bl.png"); // fixme: this gt file has been low passed!
    //     CHECK(same_image(resized2, gt2));
    // }
}

TEST_CASE("TEST MULTIPLE RESIZE") {
    Image im = load_image("data/dog.jpg");

    for (int i = 0; i < 10; i++) {
        Image im1 = bilinear_resize(im, im.w * 4, im.h * 4);
        Image im2 = bilinear_resize(im1, im1.w / 4, im1.h / 4);
        im = im2;
    }
    Image gt = load_image("data/dog-multipleresize.png");
    CHECK(same_image(im, gt));
}

TEST_CASE("TEST HIGHPASS FILTER") {
    Image im = load_image("data/dog.jpg");
    Image f = make_highpass_filter();
    Image blur = convolve_image(im, f, false);
    blur.clamp();
    Image gt = load_image("data/dog-highpass.png");
    CHECK(same_image(blur, gt));
}

TEST_CASE("TEST EMBOSS FILTER") {
    Image im = load_image("data/dog.jpg");
    Image f = make_emboss_filter();
    Image blur = convolve_image(im, f, true);
    blur.clamp();

    Image gt = load_image("data/dog-emboss.png");
    CHECK(same_image(blur, gt));
}

TEST_CASE("TEST SHARPEN FILTER") {
    Image im = load_image("data/dog.jpg");
    Image f = make_sharpen_filter();
    Image blur = convolve_image(im, f, true);
    blur.clamp();

    Image gt = load_image("data/dog-sharpen.png");
    CHECK(same_image(blur, gt));
}

TEST_CASE("TEST CONVOLUTION") {
    Image im = load_image("data/dog.jpg");
    Image f = make_box_filter(7);
    Image blur = convolve_image(im, f, true);
    blur.clamp();

    Image gt = load_image("data/dog-box7.png");
    CHECK(same_image(blur, gt));
}

TEST_CASE("TEST FAST CONVOLUTION") {
    Image im = load_image("data/dog.jpg");
    Image f = make_box_filter(15);
    // time the standard convolution
    auto t = chrono::high_resolution_clock::now();
    Image blur = convolve_image(im, f, true);
    auto stop = chrono::high_resolution_clock::now();
    auto standard_time = chrono::duration_cast<chrono::microseconds>(stop - t);
    blur.clamp();
    // time the fast convolution
    t = chrono::high_resolution_clock::now();
    Image blur2 = convolve_image_fast(im, f, true);
    stop = chrono::high_resolution_clock::now();
    auto fast_time = chrono::duration_cast<chrono::microseconds>(stop - t);
    blur2.clamp();
    cout << "Standard convolution took " << standard_time.count() << " seconds" << endl;
    cout << "Fast convolution took " << fast_time.count() << " seconds" << endl;

    Image gt = load_image("data/dog-box7.png");
    CHECK(same_image(blur, gt));
    CHECK(same_image(blur, blur2));
    CHECK(fast_time < standard_time/2);
}


TEST_CASE("TEST GAUSSIAN FILTER") {
    Image f = make_gaussian_filter(7);

    for (int i = 0; i < f.w * f.h * f.c; i++)f.data[i] *= 100;

    Image gt = load_image("data/gaussian_filter_7.png");
    CHECK(same_image(f, gt));
}

TEST_CASE("TEST GAUSSIAN BLUR") {
    Image im = load_image("data/dog.jpg");
    Image f = make_gaussian_filter(2);
    Image blur = convolve_image(im, f, true);
    blur.clamp();

    Image gt = load_image("data/dog-gauss2.png");
    CHECK(same_image(blur, gt));
}

TEST_CASE("TEST HYBRID IMAGE") {
    Image man = load_image("data/melisa.png");
    Image woman = load_image("data/aria.png");
    Image f = make_gaussian_filter(2);
    Image lfreq_man = convolve_image(man, f, true);
    Image lfreq_w = convolve_image(woman, f, true);
    Image hfreq_w = woman - lfreq_w;
    Image reconstruct = lfreq_man + hfreq_w;
    Image gt = load_image("data/hybrid.png");
    reconstruct.clamp();
    CHECK(same_image(reconstruct, gt));
}

TEST_CASE("TEST FREQUENCY IMAGE") {
    Image im = load_image("data/dog.jpg");
    Image f = make_gaussian_filter(2);
    Image lfreq = convolve_image(im, f, true);
    Image hfreq = im - lfreq;
    Image reconstruct = lfreq + hfreq;

    Image low_freq = load_image("data/low-frequency.png");
    Image high_freq = load_image("data/high-frequency-clamp.png");

    lfreq.clamp();
    hfreq.clamp();
    CHECK(same_image(lfreq, low_freq));
    CHECK(same_image(hfreq, high_freq));
    CHECK(same_image(reconstruct, im));
}

TEST_CASE("TEST SOBEL") {
    Image im = load_image("data/dog.jpg");
    pair<Image, Image> res = sobel_image(im);
    Image mag = res.first;
    Image theta = res.second;

    feature_normalize(mag);
    feature_normalize(theta);

    Image gt_mag = load_image("data/magnitude.png");
    Image gt_theta = load_image("data/theta.png");
    CHECK(gt_mag.w == mag.w && gt_theta.w == theta.w);
    CHECK(gt_mag.h == mag.h && gt_theta.h == theta.h);
    CHECK(gt_mag.c == mag.c && gt_theta.c == theta.c);

    // if (gt_mag.w != mag.w || gt_theta.w != theta.w ||
    //     gt_mag.h != mag.h || gt_theta.h != theta.h ||
    //     gt_mag.c != mag.c || gt_theta.c != theta.c)
    //     return;

    for (int i = 0; i < gt_mag.w * gt_mag.h; ++i) {
        if (within_eps(gt_mag.data[i], 0)) {
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
        if (within_eps(gt_theta.data[i], 0) ||
            within_eps(gt_theta.data[i], 1)) {
            gt_theta.data[i] = 0;
            theta.data[i] = 0;
        }
    }

    save_png(mag, "figs/mag");
    save_png(theta, "figs/theta");

    Image imo = colorize_sobel(im);
    save_png(imo, "figs/color_sobel");

    CHECK(same_image(mag, gt_mag));
    CHECK(same_image(theta, gt_theta));
}

TEST_CASE("TEST BILATERAL") {
    Image im = load_image("data/dog.jpg");
    Image bif = bilateral_filter(im, 3, 0.1);

    save_png(bif, "figs/bilateral");
    Image gt = load_image("data/dog-bilateral.png");
    CHECK(same_image(bif, gt));
}

TEST_CASE("TEST FAST BILATERAL") {
    Image im = load_image("data/dog.jpg");
    // time the standard bilateral filter
    auto t = chrono::high_resolution_clock::now();
    Image bif = bilateral_filter(im, 3, 0.1);
    auto stop = chrono::high_resolution_clock::now();
    auto standard_time = chrono::duration_cast<chrono::milliseconds>(stop - t);

    // time the fast bilateral filter
    t = chrono::high_resolution_clock::now();
    Image fast_bif = bilateral_filter_fast(im, 3, 0.1);
    stop = chrono::high_resolution_clock::now();
    auto fast_time = chrono::duration_cast<chrono::milliseconds>(stop - t);

    cout << "Standard bilateral filter took " << standard_time.count() << " milliseconds" << endl;
    cout << "Fast bilateral filter took " << fast_time.count() << " milliseconds" << endl;

    save_png(bif, "figs/bilateral_fast");
    Image gt = load_image("data/dog-bilateral.png");
    CHECK(same_image(bif, gt));
    CHECK(same_image(fast_bif, gt));
    // check that the fast bilateral filter is faster
    CHECK(fast_time < standard_time/2);
}

TEST_CASE("TEST EQUALIZATION") {
    Image im = load_image("data/dog.jpg");
    Image eqim1 = histogram_equalization_rgb(im, 256);
    save_png(eqim1, "figs/equalized_rgb");
    Image gt1 = load_image("data/equalized_rgb.png");
    CHECK(same_image(eqim1, gt1));

    Image eqim2 = histogram_equalization_hsv(im, 256);
    save_png(eqim2, "figs/equalized_hsv");
    Image gt2 = load_image("data/equalized_hsv.png");
    CHECK(same_image(eqim2, gt2));
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

    // To run legacy tests uncomment the following lines
    // run_tests();
    // std::cout << std::endl;

    // Actually run the tests.
    // This will apply any filtering specified on the command-line.
    return snitch::tests.run_tests(*args) ? 0 : 1;
}
