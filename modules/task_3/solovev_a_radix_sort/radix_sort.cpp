// Copyright 2021 Solovev Aleksandr

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "../../../modules/task_3/solovev_a_radix_sort/radix_sort.h"

int generateRandomArray(std::vector<int> *buffer, int min, int max) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    for (size_t i = 0; i < buffer->size(); i++) {
        buffer->at(i) = static_cast<int>(gen()) % (max - min + 1) + min;
    }
    return 0;
}

int MergeArrays(std::vector<int> *buffer1, std::vector<int> *buffer2, int left, std::vector<int> *result) {
    int current_buff = 0;
    for (size_t i = left; i < buffer1->size() + left; i++) {
        result->at(i) = buffer1->at(current_buff);
        current_buff++;
    }
    current_buff = 0;
    for (size_t i = left + buffer1->size() ; i < buffer2->size() + left + buffer1->size(); i++) {
        result->at(i) = buffer2->at(current_buff);
        current_buff++;
    }

    return 0;
}



int SortingCheck(std::vector<int> *buffer) {
    for (size_t i = 0; i < buffer->size() - 1; i++) {
        if (buffer->at(i) <= buffer->at(i + 1)) {
            continue;
        } else {
            return -1;
        }
    }
    return 0;
}

void CountingSort(std::vector<int> *input, std::vector<int> *output, int valbyte) {
    unsigned char *buffer = (unsigned char *)input->data();
    int counter[256] = {0};
    size_t length_counter = 256;
    int value = 0;

    for (size_t i = 0; i < input->size(); i++) {
        counter[buffer[4 * i + valbyte]]++;
    }

    for (size_t j = 0; j < length_counter; j++) {
        int tmp = counter[j];
        counter[j] = value;
        value += tmp;
    }

    for (size_t i = 0; i < input->size(); i++) {
        output->at(counter[buffer[4 * i + valbyte]]++) = input->at(i);
    }
}

int RadixSortUnsigned(std::vector<int> *buffer) {
    if (buffer->empty()) {
        return -1;
    }
    std::vector<int> outbuf(buffer->size());
    if (outbuf.data() == nullptr) {
        return -1;
    }
    for (int i = 0; i < 2; i++) {
        CountingSort(buffer, &outbuf, 2 * i);
        CountingSort(&outbuf, buffer, 2 * i + 1);
    }
    return 0;
}
int RadixSort(std::vector<int> *buffer) {
    if (buffer->size() < 1)
        return -1;
    int positive_length = 0, negative_length = 0;
    int status = 0;

    for (size_t i = 0; i < buffer->size(); i++) {
        if (buffer->at(i) >= 0) {
            positive_length++;
        } else {
            negative_length++;
        }
    }
    std::vector<int> positive_numbers(positive_length);
    std::vector<int> negative_numbers(negative_length);
    positive_length = 0;
    negative_length = 0;
    for (size_t i = 0; i < buffer->size(); i++) {
        if (buffer->at(i) >= 0) {
            positive_numbers[positive_length] = buffer->at(i);
            positive_length++;
        } else {
            negative_numbers[negative_length] = buffer->at(i);
            negative_length++;
        }
    }

    status = RadixSortUnsigned(&positive_numbers);
    status = RadixSortUnsigned(&negative_numbers);

    status = MergeArrays(&negative_numbers,  &positive_numbers, 0,  buffer);
    return status;
}
int RadixSortParallel(std::vector<int> *buffer, int left, int right) {
    if (buffer->size() < 1)
        return -1;
    int positive_length = 0, negative_length = 0;
    int status = 0;
    for (int i = left; i <= right; i++) {
        if (buffer->at(i) >= 0) {
            positive_length+=1;
        } else {
            negative_length+=1;
        }
    }
    std::vector<int> positive_numbers(positive_length);
    std::vector<int> negative_numbers(negative_length);
    positive_length = 0;
    negative_length = 0;

    for (int i = left; i <= right; i++) {
        if (buffer->at(i) >= 0) {
            positive_numbers[positive_length] = buffer->at(i);
            positive_length+=1;
        } else {
            negative_numbers[negative_length] = buffer->at(i);
            negative_length+=1;
        }
    }

    status = RadixSortUnsigned(&positive_numbers);
    status = RadixSortUnsigned(&negative_numbers);

    status = MergeArrays(&negative_numbers,  &positive_numbers, left,  buffer);

    return status;
}
void merge(int* a, int size_a, int* b, int size_b) {
    int i = 0, j = 0, k = 0;
    int size_c = size_a + size_b;
    int* c = new int[size_c];
    while ((i < size_a) && (j < size_b)) {
        if (a[i] <= b[j]) {
            c[k++] = a[i++];
        } else {
            c[k++] = b[j++];
        }
    }

    if (i == size_a) {
        while (j < size_b) {
            c[k++] = b[j++];
        }
    } else {
        while (i < size_a) {
            c[k++] = a[i++];
        }
    }

    for (int i = 0; i < size_c; i++) {
        a[i] = c[i];
    }
}

int ParallelSortingTBB(std::vector<int> *arr) {
    int size = static_cast<int>(arr->size());
    int min_grain_size = 10;
    int divider = 5;
    int part_arr_size = size / divider;
    int grain_size = part_arr_size > min_grain_size ? part_arr_size : min_grain_size;
    tbb::parallel_for(tbb::blocked_range<int>(0, size, grain_size), [&arr](const tbb::blocked_range<int>& r) {
        RadixSortParallel(arr, r.begin(), r.end() - 1);
    });
    int i_prev = 0;
    int k = 0;
    for (int i = 1; i < size; i++) {
        if (arr->at(i) < arr->at(i - 1)) {
            if (k++ > 0)
                merge(arr->data(), i_prev, arr->data() + i_prev, i - i_prev);
            i_prev = i;
        }
    }
    merge(arr->data(), i_prev, arr->data() + i_prev, size - i_prev);
    return 0;
}
