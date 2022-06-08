void map_int(int* x) {
    int x_tmp = *x;
    int x_bin = 0;
    int pos = 1;
    while (x_tmp > 0) {
        x_bin += pos * (x_tmp&1);
        pos *= 10;
        x_tmp >>= 1;
    }
    *x = x_bin;
}
