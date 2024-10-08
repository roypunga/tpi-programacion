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

    time_t fecha;
    char fecha_string[30];


    long int cuilRecibe;
    long int cuilEnvia;
};

void menuAdministrador();
void menuUsuario();

int usrChoice;
int main() {    



    principio:

    printf("Bienvenido a MercadoCobro. A que menu quiere acceder?\n");
    printf("1-Menu de Usuario\t\t2-Menu de Administrador\n");

    scanf("%d", &usrChoice);

    switch(usrChoice){

        case 1:
            menuUsuario();
            break;

        case 2:
            menuAdministrador();
            break;

        default:
            printf("NUmero invalido, ingrese 1 o 2\n");
            goto principio; //esto en teoria es muy mala practica pero me chupa un huevo, es re practico

    }

}

/*
1. Saldo de usuario: Permitir al usuario consultar su saldo actual en la cuenta.
2. Movimientos por fecha: Filtrar y listar todos los movimientos realizados por un usuario en un
rango de fechas específico.
3. Movimientos por tipo: Permitir al usuario ver solo los movimientos de un tipo específico (ingresos,
transferencias, pagos).
4. Movimientos por monto: Listar todos los movimientos que superen o estén por debajo de un
monto específico.
5. Transferencias recibidas: Mostrar todas las transferencias que un usuario ha recibido, incluyendo
detalles del remitente.
6. Transferencias realizadas: Listar todas las transferencias que un usuario ha realizado, incluyendo
detalles del destinatario.
7. Pagos realizados: Mostrar un listado de todos los pagos que un usuario ha realizado a terceros.
8. Movimientos con retención de IIBB: Filtrar y listar los movimientos que han tenido retención de
Ingresos Brutos.
9. Usuarios con saldo bajo: Identificar y listar usuarios cuyo saldo esté por debajo de un umbral
específico.
10. Actividad de usuario: Proporcionar un resumen de la actividad de un usuario, incluyendo el
número total de movimientos, total ingresado, total transferido y total pagado
*/
