#ifndef SYM_CTOR_CRACK
#define SYM_CTOR_CRACK

#define CTOR(T, ptr) new(ptr) T {}  //call std constructior for object with type T to ptr address 

#endif