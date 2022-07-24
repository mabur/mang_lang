* Compile to C and macros.
* Generic programming.
* Array programming.
* Start by writing sample programs and library in C, including macros,
  and look for patterns that I want to improve.

SEMANTICS
Mutable reference by default.

DATA STRUCTURES
Arrays, SOA, AOS, dataframes.
Have dataframe as only datastructure?

ACTIONS
copy
fill
set
transform

set(x, 1)
set(y, 2)
set(z, 4)
copy(x, y)
mul(x, y, z)

EXAMPLES
define square(in, out)
    mul(in, in, out)

transform(df[length], df[square_length], square)

FUNCTION DEFINITION

C:
typedef struct {
    int* data;
    int size;
} MutableArrayInt;

typedef struct {
    const int* data;
    int size;
} ImutableArrayInt;

MutableArrayInt makeMutableArrayInt(int size) {
    MutableArrayInt result;
    result.data = malloc(size * sizeof(int));
    result.size = size;
    return result;
}

MutableArrayInt freeMutableArrayInt(MutableArrayInt* x) {
    free(x->data);
    x->data = 0;
    x->size = 0;
}

void range_i(int* x, int N) {
    for (int i = 0; i < N; ++i) {
        x[i] = i; 
    }    
}

void add_vectors_i(const int* a, const int* b, int* c, int N) {
    for (int i = 0; i < N; ++i) {
        c[i] = a[i] + b[i]; 
    }
}
int main() {
    int N = 100;
    int* a = malloc(N*sizeof(int));
    int* b = malloc(N*sizeof(int));
    int* c = malloc(N*sizeof(int));
    add_vectors_i(a,b,c);
    ...
    free(c);
    free(b);
    free(a);
}
