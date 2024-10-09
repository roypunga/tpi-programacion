#include <stdio.h>
#include <time.h>

void cargarUsuario();

struct struct_usuario {
	char nombre[50];
    long int cuil, cvu, celular;
    char email[30], alias[30];
    int iva;
    float saldo;
}usuario;

struct struct_cuenta {
    int tipo; //caja de ahorro cuenta corriente
    long int cbu;
    long int cuil;
}cuenta;

struct struct_movimiento{
    int tipo; //ingreso transferencia o pago
    float monto;
    float iibb;

    time_t fecha;
    char fecha_string[30];


    long int cuilRecibe;
    long int cuilEnvia;
}movimiento;

void menuAdministrador();
void menuUsuario();

int usrChoice;
int main() {    

	printf("Bienvenido a PagarMercado! A que menu desea acceder?\n");

	do {
        printf("0-Salir\t\t");
		printf("1-Usuario\t\t");

        scanf("%d", &usrChoice);

        switch(usrChoice) {
            case 0:
                printf("Saliendo...\n");
                break;
            case 1:
                menuUsuario();
                break;
            case 2:
                menuAdministrador();
                break;
            default:
                printf("Opción inválida\n");
        }
    } while(usrChoice != 0);
}

void menuUsuario(){

long int usrCuil;

printf("\nIngrese su cuil: ");
scanf("%ld", &usrCuil);

FILE *file_usuarios;


	// do {
    //     printf("0-Salir\t\t");
	// 	printf("1-Usuario\t\t");

    //     scanf("%d", &usrChoice);

    //     switch(usrChoice) {
    //         case 0:
    //             printf("Saliendo...\n");
    //             break;
    //         case 1:
    //             menuUsuario();
    //             break;
    //         case 2:
    //             menuAdministrador();
    //             break;
    //         default:
    //             printf("Opción inválida\n");
    //     }
    // } while(usrChoice != 0);

}

int checkCuil(long int cuilABuscar, FILE *file_usuarios){

    int encontro = 0;

    rewind(file_usuarios);

    while(fread(&usuario, sizeof(usuario), 1, file_usuarios) && encontro != 1){

        if(usuario.cuil == cuilABuscar){
            encontro = 1;
        }

    }

    return encontro;
}

void cargarUsuario(){
	int parar=0;
	FILE *f=fopen("Usuarios.dat","a+w");
	if(f!=NULL){
		while(parar!=1){
			while(!feof(f)){
				fread(&usuario,sizeof(struct struct_usuario),1,f);
			}
			usuario.cvu = usuario.cvu + 1;
			
			printf("Ingrese su nombre\n-----> ");
			fflush(stdin);
			fgets(usuario.nombre,50,stdin);
			
			printf("Ingrese su CUIL\n-----> ");
			scanf("%ld",&usuario.cuil);
			
			printf("Ingrese su numero de celular\n-----> ");
			scanf("%ld",&usuario.cuil);
			
			printf("Ingrese su email\n-----> ");
			scanf("%s",&usuario.email[0]);
			
			printf("Ingrese su alias\n-----> ");
			fflush(stdin);
			fgets(usuario.alias,30,stdin);
			
			printf("Tiene que pagar IVA?(0. NO\n1.SI)\n-----> ");
			scanf("%i",&usuario.iva);
			
			usuario.saldo=0;
			
			fwrite(&usuario,sizeof(struct struct_usuario),1,f);
			printf("El usuario se cargo exitosamente\nPresione 0 para cargar otro usuario\nPresione 1 para volver al menu principal\n-----> ");
			rewind(f);
		}
	}
	else printf("ERROR: no se pudo abrir el archivo");
}

void consultarSaldo (long int cvu){//le pasan el cvu desde el main en el momento q se logea el usuario
	FILE *fp1;
	int encon=0;
	fp1=fopen("Usuarios.dat", "rb");
	if(fp1!=NULL){
		while((encon==0)&&((fread(&usuario,sizeof(usuario),1,fp1))==1)){
			if(usuario.cvu==cvu){
				printf("\nCuenta %ld con un saldo de: $%.2f", usuario.saldo);
				encon=1;
			}
		}
		if(encon==0){
			printf("\nNo se encontro ninguna cuenta con cvu: %ld", cvu);//se podria verificar el cvu antes de pasarlo para aca, es mas eficiente
		}
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO DATOS DE USUARIO");//logicamente no se puede ejecutar antes de cargar usuarios por lo menos 1 vez
	}
	fclose(fp1);//cierren sus archivos qliaos
}

/*
1. Saldo de usuario: Permitir al usuario consultar su saldo actual en la cuenta.
2. Movipmientos por fecha: Filtrar y listar todos los movimientos realizados por un usuario en un
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
