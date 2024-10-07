#include <stdio.h>
#include <time.h>

struct struct_usuario {
    long int cuil;
    char nombre[50];
    long int celular;
    char email[30];
    long int cvu;
    char alias[30];
    int iva;
    float saldo;
};

struct struct_cuenta {
    int tipo; //caja de ahorro cuenta corriente
    long int cbu;
    long int cuil;
};

struct struct_movimiento{
    int tipo; //ingreso transferencia o pago
    float monto;
    float iibb;
    //fecha 
    long int cuilRecibe;
    long int cuilEnvia;
};
int main() {
<<<<<<< HEAD
    
    int i;
	int opc;
	return 0;
}
>>>>>>> 350694c6e38fd428eb82142e4034f58a27d8675a
