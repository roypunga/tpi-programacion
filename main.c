#include <stdio.h>
#include <time.h>

void funcionFalopa();


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

    time_t fecha;
    char fecha_string[30];


    long int cuilRecibe;
    long int cuilEnvia;
};
void falopa(int x);
int main() {    
    int i;
	int opc;

	return 0;
}


