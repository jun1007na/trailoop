#if !defined(VECT_H)
#define VECT_H

/* w = v1 + v2 */
void vect_add(GLdouble v1[], GLdouble v2[], GLdouble w[]){
  int i;
  for(i=0; i<3; i++){
    w[i] = v1[i] + v2[i];
  }
}
/* w = v1 - v2 */
void vect_sub(GLdouble v1[], GLdouble v2[], GLdouble w[]){
  int i;
  for(i=0; i<3; i++){
    w[i] = v1[i] - v2[i];
  }
}
/* w = s*v */
void vect_scale(GLdouble s, GLdouble v[], GLdouble w[]){
  int i;
  for(i=0; i<3; i++){
    w[i] = s * v[i];
  }
}
/* |v| */
GLdouble vect_norm(GLdouble v[]){
  int i;
  GLdouble gd;
  
  for(i=0; i<3; i++){
    gd += v[i]*v[i];
  }
  return sqrt(gd);
}
/* v1とv2の内積を返却 */
GLdouble vect_innerproduct(GLdouble v1[], GLdouble v2[]){
  int i;
  GLdouble gd = 0.0;
  
  for(i=0; i<3; i++){
    gd += v1[i] * v2[i];
  }
  
  return gd;
}
/* v1とv2の外積を返却 */
void vect_outerproduct(GLdouble v1[], GLdouble v2[], GLdouble w[]){
  w[0] = v1[1]*v2[2] - v1[2]*v2[1];
  w[1] = v1[2]*v2[0] - v1[0]*v2[2];
  w[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

#endif /* VECT_H */
