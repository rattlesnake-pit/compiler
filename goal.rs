{
    int i; int j; int iaux
    string saux (20)
    string msg(20)
    string[4] names(20)
    int [4] grades

    for(i = 0; i < 4; i = i + 1) {
       read(names[i])
       read(grades[i])
    }
    for(i = 0; i < 4; i = i + 1) {
        for (j = i + 1; j < 4; j = j + 1) {
            if(grades[i] > grades[j]) {
                iaux = grades[i]
                grades[i] = grades[j]
                grades[j] = iaux
                saux = names[i]
                names[i] = names[j]
                names[j] = saux
            }
        }
    }
    for(i = 0; i < 4; i = i + 1) {
       print(grades[i])
       print(names[i])
    }
}
