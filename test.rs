{
  int i
  int j
  int aux
  int[4] calificaciones
  string [4] nombres(100)
  string msg(100)
  for(i = 0; i < 4; i = i + 1) {
    msg = "Ingresa Nombre"
    print(msg)
    read(nombres[i])
    msg = "Ingresa Calificacion"
    print(msg)
    read(calificaciones[i])
  }

  for(i = 0; i < 4; i = i + 1) {
    for(j = i + 1; j < 4; j = j + 1) {
      if(calificaciones[j] < calificaciones[i]) {
          aux = calificaciones[j]
          calificaciones[j] = calificaciones[i]
          calificaciones[i] = aux
          msg = nombres[j]
          nombres[j] = nombres[i]
          nombres[i] = msg
      }
    }
  }
  msg = "calificaciones en orden"
  print(msg)
  for(i = 0; i < 4; i = i + 1) {
    msg = "Nombre:"
    print(nombres[i])
    msg = "Calificacion:"
    print(calificaciones[i])
  }
  
}
