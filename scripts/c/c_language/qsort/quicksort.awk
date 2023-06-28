#!/usr/bin/gawk -f

function swap(arr, i, j,      temp)
{
    temp = arr[i]
    arr[i] = arr[j]
    arr[j] = temp
}

function quicksort(arr, first_index, last_index)
{
	# i = low + 1
	# for (j = high; j >= i;) {
	# 	if ((arr[i] <= arr[low] && ++i) + (arr[j] >= arr[low] && --j) == 0) {
	# 		if (i + 1 > j) break;
	# 		swap(arr, i, j)
	# 	}
	# }
	# swap(arr, low, i - 1)
	# if (i - 2 > low) quicksort(arr, low, i - 2);
	# if (high > i) quicksort(arr, i, high);


	i = first_index + 1
	j = last_index
	while(1) {
		printf "%d | %d\n", i, j
		if ((arr[j] >= arr[first_index] && --j) +\
			(arr[i] <= arr[first_index] && j >= i && ++i) == 0) {
			if (i + 1 > j) {
				swap(arr, first_index, i - 1)
				if (first_index < i - 2) quicksort(arr, first_index, i - 2);
				if (i < last_index) quicksort(arr, i, last_index);
				return;
			}
			swap(arr, i, j)
		}
	}
}

BEGIN {
	LINT = "fatal"
	arr[0] = 9
	arr[1] = 5
	arr[2] = 1
	arr[3] = 0
	arr[4] = 2
	arr[5] = 6
	arr[6] = 3
	arr[7] = 7
	arr[8] = 8
	arr[9] = 4
	quicksort(arr, 0, 9)
	for (i = 0; i < 10; ++i) {
		print arr[i]
	}
}
