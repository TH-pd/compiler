array matrix1[2][2];
array matrix2[2][2];
array matrix3[2][2];
define i;
define j;
define k;
matrix1[0][0] = 1;
matrix1[0][1] = 2;
matrix1[1][0] = 3;
matrix1[1][1] = 4;
matrix2[0][0] = 5;
matrix2[0][1] = 6;
matrix2[1][0] = 7;
matrix2[1][1] = 8;
i = 0;
while(i<2){
  j = 0;
  while(j<2){
    matrix3[i][j] = 0;
    j = j + 1;
  }
i = i+1;
}
i = 0;
while(i < 2) {
  j = 0;
  while(j < 2) {
    k = 0;
    while(k < 2) {
      matrix3[i][j] = matrix3[i][j] + matrix1[i][k] * matrix2[k][j];
      k = k + 1;
    }
    j = j + 1;
  }
  i = i + 1;
}