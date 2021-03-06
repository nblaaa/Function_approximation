#include <tgmath.h>
#include <iostream>
#include <fstream>
//#include <iomanip>

long double func(long double x) {
    return 21 * exp(-(1 / 4.0) * pow(x - 2.5, 2)) + 17.2 * exp(-(1 / 6.0) * pow(x - 8, 2));
    //return 0.7 * exp(-(1 / 8.0) * pow(x - 2.3, 2)) + 1.2 * exp(-(1 / 5.0) * pow(x - 6.8, 2));
};

int main()
{
    //std::cout << std::setprecision(33);
    char file_name[80];
    std::cout << "Input name of file: ";
    std::cin >> file_name;
    std::ifstream file(file_name, std::ios::binary);

    int experiments_count = 0, file_size;
    while(experiments_count == 0) {
        while (!file) {
            std::cout << "File didn't open. Input name again: ";
            std::cin >> file_name;
            file.open(file_name, std::ios::binary);
        }
        file.seekg(0, std::ios::end);
        file_size = (int)file.tellg() - 1;
        file.seekg(0, std::ios::beg);
        experiments_count = file_size / 768;
        if (experiments_count == 0) {
            std::cout << "There is an error in opening the file. Input name of another file: ";
            std::cin >> file_name;
            file.open(file_name, std::ios::binary);
        }
    }
    if (file_size % 768 > 0) {
        experiments_count++;
    }
    int experiment_number = 0;
    if (experiments_count > 1) {
        while ((experiment_number < 1) || (experiment_number > experiments_count)) {
            std::cout << "Input number of experiment from 1 to " << experiments_count << ": ";
            std::cin >> experiment_number;
        }
    } else {
        experiment_number = 1;
    }
    file.seekg(768 * (experiment_number - 1) + 257, std::ios::beg);

    int n = 256, min = 0, min_local = 0, input_number = 0, zero_check = 20;
    long double x[n], step = 3.857;
    unsigned char f[n];

    for (uint8_t i = 0; i < 20; ++i) {
        file.read((char *) &f[0], 1);
        if ((f[0] != 63) && (f[0] != 0)) {
            zero_check = i + 1;
            break;
        }
    }
    if ((f[0] == 63) || (f[0] == 0) || ((file.eof()) && (file_size % 768 > 0))) {
        input_number = 1;
    } else {
        file.seekg(256 - zero_check, std::ios::cur);
        for (uint8_t i = 0; i < 20; ++i) {
            file.read((char *) &f[0], 1);
            //std::cout << (int)f[0] << ' ';
            if ((f[0] != 63) && (f[0] != 0)) {
                break;
            }
        }
        if ((f[0] == 63) || (f[0] == 0)  || ((file.eof()) && (file_size % 768 > 0))) {
            input_number = 2;
        } else {
            input_number = 3;
        }
    }
    if (input_number > 1) {
        int input_number_temp = 0;
        while ((input_number_temp < 1) || (input_number_temp > input_number)) {
            std::cout << "Input number from 1 to " << input_number << ": ";
            std::cin >> input_number_temp;
        }
        input_number = input_number_temp;
    }
    file.seekg(768 * (experiment_number - 1) + 1 + 256 * (input_number - 1), std::ios::beg);

    for (int i = 0; i < n; ++i) {
        x[i] = i * step;
        file.read((char *) &f[i], 1);
        if (f[i] < f[min]) {
            min = i;
        }
        std::cout << i << ' ' << (int)f[i] << '\n';
    }
    std::cout << '\n';
    file.close();
    /*for (int i = 0; i < n; ++i) {
        f[i] -= f[min];
        std::cout << (int)f[i] << ' ';
    }
    std::cout << '\n';*/

    /*for (int i = min; f[i] > f[min] + 2; ++i) {

    }*/
    int start, end;
    std::cout << "Input start: ";
    std::cin >> start;
    std::cout << "Input end: ";
    std::cin >> end;
    std::cout << '\n';
    end++;
    min_local = start;
    for (int i = start + 1; i < end; ++i) {
        if (f[i] < f[min_local]) {
            min_local = i;
        }
    }
    int coefficient = f[min_local];
    for (int i = start; i < end; ++i) {
        f[i] -= coefficient;
    }

    n = end - start;
    int index = start + n / 10, peak_beginning;
    bool is_peak_unavailable = false;
    long double a1, a2, b1, b2, c1, c2;
    while (f[index] <= f[index + 1]) {
        if (f[index] + coefficient == 128) {
            if (f[index - 1] + coefficient < 128) {
                peak_beginning = index;
                is_peak_unavailable = true;
            }
        }
        index++;
        if ((f[index + 1] + coefficient < 128) && (is_peak_unavailable)) {
            index = (peak_beginning + index) / 2;
            break;
        }
    }
    c1 = x[index];
    a1 = f[index];
    b1 = std::abs(-log(f[start + (index - start) * 100 / 245] / a1) / (pow(x[start + (index - start) * 100 / 245] - c1, 2)));

    if (is_peak_unavailable) {
        a1 += coefficient / 2;
    }

    index = int(start + n * 0.9);
    is_peak_unavailable = false;
    while (f[index] <= f[index - 1]) {
        if (f[index] + coefficient == 128) {
            if (f[index + 1] + coefficient < 128) {
                peak_beginning = index;
                is_peak_unavailable = true;
            }
        }
        index--;
        if ((f[index - 1] + coefficient < 128) && (is_peak_unavailable)) {
            index = (peak_beginning + index) / 2;
            break;
        }
    }
    c2 = x[index];
    a2 = f[index];
    b2 = std::abs(-log(f[index + (n - index) * 100 / 182] / a2) / (pow(x[index + (n - index) * 100 / 182] - c2, 2)));

    bool long_line = false;
    if ((c1 > c2) && (!is_peak_unavailable)) {
        long_line = true;
        int max_local = a1;
        index = start + n / 10;
        while ((max_local - f[index] > 4) || (f[index] < f[index + 1])) {
            index++;
        }
        c1 = x[index];

        index = int(start + n * 0.9);
        while ((max_local - f[index] > 4) || (f[index] < f[index - 1])) {
            index--;
        }
        c2 = x[index];
    }

    if (is_peak_unavailable) {
        a2 += coefficient / 2;
    }

    bool is_two_extremums = true;
    if (std::abs(c1 - c2) <= step) {
        int index_temp = start + n;
        while (f[index_temp - 1] < f[index_temp]) {
            index_temp--;
        }
        while ((f[index_temp] <= f[index_temp - 1]) && (index_temp > int(n * 0.9))) {
            index_temp--;
        }
        if (index_temp <= n * 0.9) {
            index_temp = start;
            while(f[index_temp + 1] < f[index_temp]) {
                index_temp++;
            }
            while ((f[index_temp] <= f[index_temp + 1]) && (index_temp < n / 10)) {
                index_temp++;
            }
            if (index_temp >= 100) {
                is_two_extremums = false;
            } else {
                c1 = x[index_temp];
                a1 = f[index_temp];
                b1 = std::abs(-log(f[start + (index_temp - start) / 2] / a1) / (pow(x[start + (index_temp - start) / 2] - c1, 2)));
            }
        } else {
            c2 = x[index_temp];
            a2 = f[index_temp];
            b2 = std::abs(-log(f[index_temp + (n - index_temp) / 2] / a2) / (pow(x[index_temp + (n - index_temp) / 2] - c2, 2)));
        }
    }


    std::cout << "a1 = " << a1 << ", b1 = " << b1 << ", c1 = " << c1 << '\n';
    if (is_two_extremums) {
        std::cout << "a2 = " << a2 << ", b2 = " << b2 << ", c2 = " << c2 << "\n\n";
    } else {
        a1 /= 2;
        a2 /= 2;
        std::cout << '\n';
    }
    //return 0;



    long double step1 = 0.5, step2 = 0.01, step3 = 0.1, min_value = INFINITY, previous_value, temp;
    if ((a1 > 1000) || (a2 > 1000)) {
        step1 = 2;
        step3 = 0.05;
    } else if ((a1 > 100) || (a2 > 100)) {
        step1 = 1;
        step3 = 0.1;
    } else if ((a1 > 10) || (a2 > 10)) {
        step1 = 0.5;
        step3 = 0.5;
    } else {
        step1 = 0.1;
        step3 = 1;
    }
    if ((b1 > 10) || (b2 > 10)) {
        step2 = 0.5;
    } else if ((b1 > 1) || (b2 > 1)) {
        step2 = 0.05;
    } else {
        step2 = 0.01;
    }
    if (!is_two_extremums) {
        step1 *= 2;
        step2 *= 2;
        step3 *= 2;
    }
    int numbers_of_points = n, counter = 0;
    if (n < numbers_of_points) {
        numbers_of_points = n;
    }
    if ((n > 150) && (!long_line)) {
        numbers_of_points = 50;
    } else if (n > 150) {
        numbers_of_points = 100;
    }
    uint8_t min_point_indexes[6];
    for (uint8_t i = 0; i < 6; ++i) {
        min_point_indexes[i] = 0;
    }
    bool q = true, need_center_correction = false;
    uint8_t center_correcter = 1;
    int little_changes_count = 0;
    long double reset[6];
    while (min_value > 18) {
    q = false;
    previous_value = min_value;
    for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t j = 0; j < 5; ++j) {
            for (uint8_t k = 0; k < 5; ++k) {
                for (uint8_t l = 0; l < 5; ++l) {
                    for (uint8_t m = 0; m < 5; ++m) {
                        for (uint8_t r = 0; r < 5; ++r) {
                            temp = 0;
                            for (int p = 0; p < numbers_of_points; ++p) {
                                center_correcter = 1;
                                if ((p > n * 0.25) && (p < n * 0.75)) {
                                    if (need_center_correction) {
                                        center_correcter = 8;
                                    }
                                }
                                if (f[start + p * n / numbers_of_points] + coefficient < 128) {
                                    temp += center_correcter * std::abs(f[start + p * (n / numbers_of_points)] - std::abs(a1 - (2 - i) * step1) * exp(-std::abs(b1 - (2 - j) * step2) * pow(x[start + p * (n / numbers_of_points)] - c1 + (2 - k) * step3, 2)) - std::abs(a2 - (2 - l) * step1) * exp(-std::abs(b2 - (2 - m) * step2) * pow(x[start + p * (n / numbers_of_points)] - c2 + (2 - r) * step3, 2)));
                                }
                            }
                            if (temp < min_value) {
                                q = true;
                                min_value = temp;
                                min_point_indexes[0] = i;
                                min_point_indexes[1] = j;
                                min_point_indexes[2] = k;
                                min_point_indexes[3] = l;
                                min_point_indexes[4] = m;
                                min_point_indexes[5] = r;
                            }
                        }
                    }
                }
            }
        }
    }
    if (!q) {
        step1 /= 10;
        step2 /= 10;
        step3 /= 10;
        if (step1 < 1e-20) {
            step1 = 0.5;
            step2 = 0.01;
            step3 = 0.1;

            if (((min_value > 160) && (!long_line)) || ((long_line) && (min_value > 200))) {
                temp = min_value + 301;
                reset[0] = a1;
                reset[1] = b1;
                reset[2] = c1;
                reset[3] = a2;
                reset[4] = b2;
                reset[5] = c2;
                while (temp - min_value > 300) {
                    a1 = reset[0] + (rand() % 100 - 50) / 1.0;
                    b1 = reset[1] + (rand() % 10 - 5) / 10.0;
                    c1 = reset[2] + (rand() % 10 - 5) / 1.0;
                    a2 = reset[3] + (rand() % 100 - 50) / 1.0;
                    b2 = reset[4] + (rand() % 10 - 5) / 10.0;
                    c2 = reset[5] + (rand() % 10 - 5) / 1.0;

                    temp = 0;
                    for (int p = 0; p < numbers_of_points; ++p) {
                        center_correcter = 1;
                        if ((p > n * 0.25) && (p < n * 0.75)) {
                            if (need_center_correction) {
                                center_correcter = 8;
                            }
                        }
                        if (f[start + p * n / numbers_of_points] + coefficient < 128) {
                            temp += center_correcter * std::abs(f[start + p * (n / numbers_of_points)] - std::abs(a1) * exp(-std::abs(b1) * pow(x[start + p * (n / numbers_of_points)] - c1, 2)) - std::abs(a2) * exp(-std::abs(b2) * pow(x[start + p * (n / numbers_of_points)] - c2, 2)));
                        }
                    }
                }
            }

            min_value = temp;
            counter = 0;
        }
        continue;
    }
    a1 -= (2 - min_point_indexes[0]) * step1;
    b1 -= (2 - min_point_indexes[1]) * step2;
    c1 -= (2 - min_point_indexes[2]) * step3;
    a2 -= (2 - min_point_indexes[3]) * step1;
    b2 -= (2 - min_point_indexes[4]) * step2;
    c2 -= (2 - min_point_indexes[5]) * step3;
    if ((std::abs(previous_value - min_value) < 0.0001) && (((min_value < 160) && (!long_line)) || ((long_line) && (min_value < 200)))) {
        little_changes_count++;
        if (little_changes_count >= 10) {
            break;
        }
    } else {
        little_changes_count = 0;
    }
    if ((min_point_indexes[0] % 4 != 0) && (min_point_indexes[1] % 4 != 0) && (min_point_indexes[2] % 4 != 0) && (min_point_indexes[3] % 4 != 0) && (min_point_indexes[4] % 4 != 0) && (min_point_indexes[5] % 4 != 0)) {
        step1 /= 10;
        step2 /= 10;
        step3 /= 10;
        //counter = 0;
    } else {
        counter++;
        if ((counter % 10 == 0) && (step2 < 0.01)) {
            step1 = 0.5 * (counter / 10);
            step2 = 0.01 * (counter / 10);
            step3 = 0.1 * (counter / 10);
            //counter = 0;
        }
        if ((counter % 100 == 0) && (min_value > 160)) {
            step1 = 0.5;
            step2 = 0.01;
            step3 = 0.1;

            temp = min_value + 301;
            reset[0] = a1;
            reset[1] = b1;
            reset[2] = c1;
            reset[3] = a2;
            reset[4] = b2;
            reset[5] = c2;
            while (temp - min_value > 300) {
                a1 = reset[0] + (rand() % 100 - 50) / 1.0;
                b1 = reset[1] + (rand() % 10 - 5) / 10.0;
                c1 = reset[2] + (rand() % 10 - 5) / 1.0;
                a2 = reset[3] + (rand() % 100 - 50) / 1.0;
                b2 = reset[4] + (rand() % 10 - 5) / 10.0;
                c2 = reset[5] + (rand() % 10 - 5) / 1.0;

                temp = 0;
                for (int p = 0; p < numbers_of_points; ++p) {
                    center_correcter = 1;
                    if ((p > n * 0.25) && (p < n * 0.75)) {
                        if (need_center_correction) {
                            center_correcter = 8;
                        }
                    }
                    if (f[start + p * n / numbers_of_points] + coefficient < 128) {
                        temp += center_correcter * std::abs(f[start + p * (n / numbers_of_points)] - std::abs(a1) * exp(-std::abs(b1) * pow(x[start + p * (n / numbers_of_points)] - c1, 2)) - std::abs(a2) * exp(-std::abs(b2) * pow(x[start + p * (n / numbers_of_points)] - c2, 2)));
                    }
                }
            }

            min_value = temp;
            counter = 0;
        }
    }
    temp = 0;
    if (!need_center_correction) {
        for (int p = 0; p < numbers_of_points; ++p) {
            center_correcter = 0;
            if ((p > n * 0.25) && (p < n * 0.75)) {
                center_correcter = 1;
            }
            if (f[start + p * n / numbers_of_points] + coefficient < 128) {
                temp += center_correcter * std::abs(f[start + p * (n / numbers_of_points)] - std::abs(a1) * exp(-std::abs(b1) * pow(x[start + p * (n / numbers_of_points)] - c1, 2)) - std::abs(a2) * exp(-std::abs(b2) * pow(x[start + p * (n / numbers_of_points)] - c2, 2)));
            }
        }
        if ((min_value < 80) && (temp > 50)) {
            need_center_correction = true;
            min_value = INFINITY;
        }
    }
    std::cout << "a1 = " << a1 << ", b1 = " << b1 << ", c1 = " << c1 << '\n';
    std::cout << "a2 = " << a2 << ", b2 = " << b2 << ", c2 = " << c2 << '\n';
    std::cout << min_value << ' ' << previous_value << '\n';

    for (uint8_t i = 0; i < 6; ++i) {
        std::cout << (int)min_point_indexes[i] << ' ';
    }
    std::cout << '\n' << step1 << ' ' << step2 << ' ' << step3 << '\n';
    std::cout << "\n\n";
    }

    a1 = std::abs(a1);
    a2 = std::abs(a2);
    b1 = std::abs(b1);
    b2 = std::abs(b2);


    std::cout << "a1 = " << a1 << ", b1 = " << b1 << ", c1 = " << c1 << '\n';
    std::cout << "a2 = " << a2 << ", b2 = " << b2 << ", c2 = " << c2 << '\n';

    return 0;
}
