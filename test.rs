{
    int a
    int b
    int c
    a = 0
    b = 1
    int i
    for(i = 0; i < 10; i = i + 1) {
        c = a + b
        a = b
        b = c
        print(a)
    }
}
