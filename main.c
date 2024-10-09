#include <stdio.h>
#include <time.h>

void cargarUsuario();
void mostrarUsuarios();
void consultarSaldo(long int cvu);

struct struct_usuario{
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

struct struct_pago{
	long int cuilPago;
	char nombreEmpresa[50];
	float monto;
}pago;

int checkCuil(long int cuilABuscar, FILE *file_usuarios);
void menuAdministrador();
void menuUsuario();

int usrChoice;
int main() {    

	printf("Bienvenido a PagarMercado! A que menu desea acceder?\n");

	do {
        printf("0-Salir\n");
		printf("1-Usuario\n");
		printf("2-Administrador\n");
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
	int posicionUsuario;

	printf("\nIngrese su cuil(-1 para volver): ");

	principio:

	scanf("%ld", &usrCuil);

	if(usrCuil == -1){
		return;
	}

	FILE *file_usuarios;

	if((file_usuarios = fopen("Usuarios.dat", "r+b")) != NULL){

		if((posicionUsuario = checkCuil(usrCuil, file_usuarios)) != -1){
			
			rewind(file_usuarios);
			fseek(file_usuarios, sizeof(usuario)*(posicionUsuario), SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, file_usuarios);

			fclose(file_usuarios);

			printf("\nBienvenido %s! Que operacion desea realizar?\n", usuario.nombre);

			do {
				printf("0. Volver\n");
				printf("1. Ingresar dinero\n");
				printf("2. Transferir dinero\n");
				printf("3. Listar movimientos\n");
				printf("4. Pagar\n");
				printf("5. Listar pagos");
				printf("5. Mostrar mis datos\n");
				scanf("%d", &usrChoice);

				switch(usrChoice) {
					case 0:
						printf("Volviendo...\n");
						break;
					case 1:
						printf("1");
						//ingresarDinero();
						break;
					case 2:
						printf("2");
						//transferirDinero(cvuDestino, monto);
						//tiene que restar a la cuenta que envia, sumar a la de destino y guardar la transferencia:
						//destino: origen: monto:
						break;
					case 3:
						printf("3");
						//menuListarMovimientosUsuarios();
						//1-todos 2-segun tipo > 1-2-3-  3-segun fecha > entre x y y 4-por monto > 1-mayores a 2-menores a
						break;
					case 4:
						printf("4");
						//pagar(); //esta funcion tiene que crear/aniadir pagos.dat
						break;
					case 5:
						// listarPagos();
						break;
					case 6:
						//mostrarUnUsuario(usuario.cuil); 
						break;
					default:
						printf("Opción inválida\n");
				}
   		 	} while(usrChoice != 0);
		}
		else{
			printf("\nNo se encontro el cuil, ingrese nuevamente: ");
			goto principio;
		}

	}
	else{
		printf("ERROR");
	}

}

void menuAdministrador(){
	printf("\nIngrese la pass de administrador(le puse 123 por ahora): ");
	scanf("%d", &usrChoice);

	long int usrCuil;

	if(usrChoice == 123){
		printf("\nBienvenido administrador! Que desea hacer?");

		do {
        printf("0-Salir\n");
		printf("1-Crear usuario\n");
		printf("2-Mostrar datos de un usuario\n");
		printf("3-Mostrar los datos de todos los usuarios\n");
		printf("4-Modificar un usuario\n");

        scanf("%d", &usrChoice);

        switch(usrChoice) {
            case 0:
                printf("Saliendo...\n");
                break;
            case 1:
				cargarUsuario();
				break;
			case 2:
				//mostrarUnUsuario(cuil)
				break;
			case 3:
				mostrarUsuarios(); 
				break;
			

            default:
                printf("Opción inválida\n");
        }
    } while(usrChoice != 0);

	}

}

//para usar esta funcion hay que pasarle el ptr al file ya hecho, pq sino hay quilombo
//esta funcion devuelve -1 si no existe el cuil
//si lo encuentra devuelve la posicion del usuario en Usuarios.dat.

// el quilombo seria pq esta funcion probablemente se va a usar dentro de otra funcion que ya tiene abierto el file
// entonces si se abre de nuevo adentro de la funcion estaria abierto 2 veces el mismo archivo
// y eso puede romper todo
int checkCuil(long int cuilABuscar, FILE *file_usuarios){

    int encontro = 0;
	int contador = 0;

    rewind(file_usuarios);

    while(fread(&usuario, sizeof(usuario), 1, file_usuarios) && encontro != 1){

		contador += 1;
        if(usuario.cuil == cuilABuscar){
            encontro = 1;
        }

    }

    if(encontro == 1){
		return contador - 1;
	}
	return -1;
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

void cargarUsuario(){
	
	int usrChoice;
    FILE *file_usuarios;


    if((file_usuarios = fopen("Usuarios.dat", "a+b")) != NULL){

		do{
			printf("\nIngrese el nombre: ");
			getchar();
			fgets(usuario.nombre, 50, stdin);
			

			denuevo:

			printf("\nIngrese el cuil: ");
			fflush(stdin);
			scanf("%ld", &usuario.cuil);
			
			
			// if(checkCuil(usuario.cuil, file_usuarios) != -1){
			// 	printf("Ese cuil ya existe, ingrese de nuevo");
			// 	goto denuevo;
			// }

			// fseek(file_usuarios, 0, SEEK_END);

			printf("\nIngrese el mail: ");
			getchar();
			fgets(usuario.email, 30, stdin);
			

			printf("\nIngrese el alias: ");			
			fgets(usuario.alias, 30, stdin);
			

			printf("\nIngrese el telefono: ");
			scanf("%ld", &usuario.celular);		
			

			printf("\nEste usuario paga iva? 1/0: ");
			scanf("%d", &usuario.iva);
			
			
			usuario.saldo = 0;

			fwrite(&usuario, sizeof(usuario), 1, file_usuarios);

			printf("\nDesea cargar otro usuario? (1/0)");
			scanf("%d", &usrChoice);


		}while(usrChoice != 0);

    }
    else{
        printf("\nError al abrir el archivo");
    }
	fclose(file_usuarios);
}

void mostrarUsuarios(){
	FILE *f=fopen("Usuarios.dat","rb");
	if(f!=NULL){
		printf("\n----------DATOS USUARIOS----------\n\n");
		fread(&usuario,sizeof(struct struct_usuario),1,f);
		while(!feof(f)){
			printf("\nUsuario: %s\nCuil: %ld\nCVU: %ld\nCelular: %li\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f",usuario.nombre, usuario.cuil, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);
			fread(&usuario,sizeof(struct struct_usuario),1,f);
		}
		printf("\n--------------------------------------------------\n");
		fclose(f);
	}
}


/*
1. Saldo de usuario: Permitir al usuario consultar su saldo actual en la cuenta. X


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
