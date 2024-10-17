#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>



#define CODIGO_SEGURIDAD1 "6"
#define CODIGO_SEGURIDAD2 "8"
#define CODIGO_PROVEEDOR "6666666"

void generarCvu(char *cadena, FILE *file_usuarios);
void cargarUsuario();
void mostrarUsuarios();
void consultarSaldo(char cvu);
void menuListarMovimientosUsuarios(long int x);
void listarSoloPagos (long int x);
void listarSoloTrans(long int x);
void listarSoloIngr(long int x);
void listarMovsMayrA(long int x, float mnt);
void listarMovsMenrA(long int x, float mnt);
void listarMovsEntre(long int x, float mnt1, float mnt2);
void listarMovimientos(long int x);

struct struct_usuario{
	char nombre[50];
    long int cuil, celular;
    char email[30], alias[30], cvu[22];
    int iva;
    float saldo;
}usuario;

struct struct_cuenta {
    int tipo; //caja de ahorro cuenta corriente
    char cvu[22];
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
						menuListarMovimientosUsuarios(atoi(usuario.cvu));
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
	usuario.cuil = cuilABuscar;
	return -1;
}

void consultarSaldo (char cuil){//le pasan el cuil desde el main en el momento q se logea el usuario
	FILE *fp1;
	int encon=0;
	fp1=fopen("Usuarios.dat", "rb");
	if(fp1!=NULL){
		while((encon==0)&&((fread(&usuario,sizeof(usuario),1,fp1))==1)){
			if(usuario.cuil==cuil){
				printf("\nCuenta con un saldo de: $%.2f", usuario.saldo);
				encon=1;
			}
		}
		if(encon==0){
			printf("\nNo se encontro ninguna cuenta con cuil: %ld", cuil);//se podria verificar el cvu antes de pasarlo para aca, es mas eficiente
		}
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO DATOS DE USUARIO");//logicamente no se puede ejecutar antes de cargar usuarios por lo menos 1 vez
	}
	fclose(fp1);//cierren sus archivos qliaos
}

void cargarUsuario(){
	
	int usrChoice, validar,puto;
    FILE *file_usuarios;


    if((file_usuarios = fopen("Usuarios.dat", "a+b")) != NULL){

		do{

			generarCvu(usuario.cvu, file_usuarios);

			printf("\nIngrese el nombre -----> ");
			getchar();
			fgets(usuario.nombre, 50, stdin);
			usuario.nombre[strcspn(usuario.nombre, "\n")] = '\0';
			

			denuevo:
			
			do{
				printf("\nIngrese el cuil ------> ");
				validar = scanf("%ld", &usuario.cuil);
				
				puto = checkCuil(usuario.cuil, file_usuarios);
				printf("cuil = %ld ----- validar = %i ----- fverificar =  %i \n\n",usuario.cuil,validar,puto);
				
				if (validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1){
					printf("ERROR: cuil no valido %ld validar: %i",usuario.cuil,validar);
					fflush(stdin);
				}
			} while (validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1);
			

			printf("\nIngrese el mail ------> ");
			getchar();
			fgets(usuario.email, 30, stdin);
			usuario.email[strcspn(usuario.email, "\n")] = '\0';
			

			printf("\nIngrese el alias -----> ");			
			fgets(usuario.alias, 30, stdin);
			usuario.alias[strcspn(usuario.alias, "\n")] = '\0';
			
			do{
				printf("\nIngrese el telefono ------> ");
				validar = scanf("%ld", &usuario.celular);	
				if(validar != 1){
					printf("ERROR: numero de celular no valido");
					fflush(stdin);
				}
			} while(validar != 1);
				
			
			do{
				printf("\nEste usuario paga iva? (1 == si // 0 == no) ----->  ");
				validar = scanf("%d", &usuario.iva);	
				if(validar != 1 || usuario.iva != 1 && usuario.iva != 0){
					printf("ERROR: no ingreso 1 o 0");
					fflush(stdin);
				}
			} while(validar != 1 || usuario.iva != 1 && usuario.iva != 0);
			
			
			usuario.saldo = 0;

			fseek(file_usuarios, 0, SEEK_END);

			fwrite(&usuario, sizeof(usuario), 1, file_usuarios);

			printf("\nDesea cargar otro usuario? (1 == si // 0 == no) ----->  ");
			scanf("%d", &usrChoice);


		}while(usrChoice != 0);

    }
    else{
        printf("\nError al abrir el archivo");
    }
	fclose(file_usuarios);
}

void generarCvu(char *cadena, FILE *file_usuarios){

    int i, j = 0;

    char charIdCvuUsuario[11] = "0000000001";
    char preCadena[23] = "";

    long int idCvuUsuario;

            rewind(file_usuarios);


            if((fread(&usuario, sizeof(usuario), 1, file_usuarios)) != NULL){

				printf("ENTRO AL IF");

                fseek(file_usuarios, sizeof(struct struct_usuario)*(-1), SEEK_END);

                fread(&usuario, sizeof(usuario), 1, file_usuarios);

                for(i = 11, j = 0; i < 21; i++, j++){ //no sabia si esto iba a funcionar, reloco

                    charIdCvuUsuario[j] = usuario.cvu[i];
					
 
                }
				charIdCvuUsuario[j] = '\0';

                idCvuUsuario = atoi(charIdCvuUsuario);
                idCvuUsuario += 1;
                snprintf(charIdCvuUsuario, sizeof(charIdCvuUsuario), "%010ld", idCvuUsuario);

            }

        strcat(preCadena, "000"); //id cvu
        strcat(preCadena, CODIGO_PROVEEDOR);
        strcat(preCadena, CODIGO_SEGURIDAD1);
        strcat(preCadena, charIdCvuUsuario);
        strcat(preCadena, CODIGO_SEGURIDAD2);

        strcpy(cadena, preCadena);

}


void mostrarUsuarios(){
	FILE *f=fopen("Usuarios.dat","rb");
	if(f!=NULL){
		printf("\n----------DATOS USUARIOS----------\n\n");
		fread(&usuario,sizeof(struct struct_usuario),1,f);
		while(!feof(f)){
			printf("\nUsuario: %s\nCuil: %ld\nCVU: %s\nCelular: %li\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f\n",usuario.nombre, usuario.cuil, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);
			fread(&usuario,sizeof(struct struct_usuario),1,f);
		}
		printf("\n--------------------------------------------------\n");
		fclose(f);
	}
}

void menuListarMovimientosUsuarios(long int x){
	float monto1=0.0, monto2=0.0;
	int opc, usrChoiceSeTip, cnt=0, cntG=0;
	printf("\nQue movimientos desea listar? \n\t0.Salir.\n\t1.Todos. \n\t2.Segun tipo. \n\t3.Segun fechas. \n\t4.Segun monto.\n");
	while(cntG==0){
	scanf("%d", &opc);
		switch(opc){
			case 0:
				printf("\nSaliendo...");
				cntG=1;
			break;
			case 1:
				listarMovimientos(x);
				cntG=1;
			break;
			case 2: 
				printf("\nListar segun tipo: \n\t1.Transferencias. \n\t2.Ingresos. \n\t3.Pagos.\n");
				while(cnt==0){
				scanf("%d", &usrChoiceSeTip);
					switch (usrChoiceSeTip){
						case 1:
							listarSoloTrans(x);
							cnt=1;
						break;
						case 2:
							listarSoloIngr(x);
							cnt=1;
						break;
						case 3:
							listarSoloPagos(x);
							cnt=1;
						break;
					default:
						printf("\nIngrese una opcion valida: ");
						cnt=0;
					break;
					}
				}
				cntG=1;
			break;
			case 3:
				printf("\nListar segun fecha: \n\t1.Entre 2 fehcas. \n\t2.Una fecha en particular. \n");
				while(cnt==0){
				scanf("%d", &usrChoiceSeTip);
					switch (usrChoiceSeTip){
						case 1:
							//listarEntrFecha(fecha 1, fecha 2, x);
							cnt=1;
						break;
						case 2:
							//listarSoloIngr(fecha 1,x);
							cnt=1;
						break;
					default:
						printf("\nIngrese una opcion valida: ");
						cnt=0;
					break;
					}
				}
				cntG=1;
			break;
			case 4:
				printf("\nListar segun monto: \n\t1.Mayor a... \n\t2.Menor a ... \n\t3.Entre ... y ...\n");
				while(cnt==0){
				scanf("%d", &usrChoiceSeTip);
					switch (usrChoiceSeTip){
						case 1:
							printf("\nIngrese el monto minimo a partir del cual listar: ");
							scanf("%f", &monto1);
							listarMovsMayrA(x, monto1);
							cnt=1;
						break;
						case 2:
							printf("\nIngrese monto maximo bajo el cual listar: ");
							scanf("%f", monto1);
							listarMovsMenrA(x, monto1);
							cnt=1;
						break;
						case 3:
							printf("\nIngrese rango de valores entre los cuales listar (primero el menor), separados por un ENTER: ");
							scanf("%f", &monto1);
							scanf("%f", &monto2);
							listarMovsEntre(x, monto1, monto2);
							cnt=1;
						break;
					default:
						printf("\nIngrese una opcion valida: ");
						cnt=0;
					break;
					}
				}
				cntG=1;
			break;
			default:
				printf("\nIngrese una opcion valida: ");
				cntG=0;
			break;
		}
	}
}
void listarMovimientos (long int x){
	FILE *fp1;
	int cont=0;
	fp1=fopen("movimientos.dat", "rb");
	if(fp1!=NULL){
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if(movimiento.cuilEnvia==x){
				if(movimiento.tipo==1){
					printf("\nOperacion: transferencia, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
					//chequear la mascara de la fecha
				}
				else{
					if(movimiento.tipo==2){
						printf("\nOperacion: pago, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
					}
					else{
						printf("\nOperacion: ingreso, monto: $%.2f, fecha: %t", movimiento.monto, movimiento.fecha);
					}
				}
				cont++;
			}
		}
		printf("\nHay un total de: %d", cont);
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarSoloTrans(long int x){
	FILE *fp1;
	fp1=fopen("movimientos.dat", "rb");
	if(fp1!=NULL){
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if((movimiento.cuilEnvia==x)&&(movimiento.tipo==1)){
				printf("\nOperacion: transferencia, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
		}
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarSoloIngr(long int x){
	FILE *fp1;
	fp1=fopen("movimientos.dat", "rb");
	if(fp1!=NULL){
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if((movimiento.cuilEnvia==x)&&(movimiento.tipo==3)){
				printf("\nOperacion: Ingreso, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
		}
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarSoloPagos (long int x){
	FILE *fp1;
	fp1=fopen("movimientos.dat", "rb");
	if(fp1!=NULL){
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if((movimiento.cuilEnvia==x)&&(movimiento.tipo==2)){
				printf("\nOperacion: Pago, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
		}
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarMovsMayrA (long int x, float mnt){
	FILE *fp1;
	fp1=fopen("movimientos.dat", "rb");
	if(fp1!=NULL){
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if (((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt)) && (movimiento.tipo == 1)) {
				printf("\nOperacion: Transferencia, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
			else {
				if (((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt)) && (movimiento.tipo == 2)) {
					printf("\nOperacion: Pago, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
					//chequear la mascara de la fecha
				}
				else {
					if ((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt)) {
						printf("\nOperacion: ingreso, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
						//chequear la mascara de la fecha
					}
				}
			}
		}
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarMovsMenrA(long int x, float mnt) {
	FILE* fp1;
	fp1 = fopen("movimientos.dat", "rb");
	if (fp1 != NULL) {
		while (fread(&movimiento, sizeof(movimiento), 1, fp1) == 1) {
			if (((movimiento.cuilEnvia == x) && (movimiento.monto < mnt)) && (movimiento.tipo == 1)) {
				printf("\nOperacion: Transferencia, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
			else {
				if (((movimiento.cuilEnvia == x) && (movimiento.monto < mnt)) && (movimiento.tipo == 2)) {
					printf("\nOperacion: Pago, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
					//chequear la mascara de la fecha
				}
				else {
					if ((movimiento.cuilEnvia == x) && (movimiento.monto < mnt)) {
						printf("\nOperacion: ingreso, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
						//chequear la mascara de la fecha
					}
				}
			}
		}
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}
void listarMovsEntre(long int x, float mnt1, float mnt2) {
	FILE* fp1;
	fp1 = fopen("movimientos.dat", "rb");
	if (fp1 != NULL) {
		while (fread(&movimiento, sizeof(movimiento), 1, fp1) == 1) {
			if (((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt1)) && (movimiento.monto <= mnt2)&&(movimiento.tipo==1)) {
				printf("\nOperacion: Transferencia, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
				//chequear la mascara de la fecha
			}
			else {
				if (((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt1)) && (movimiento.monto <= mnt2) && (movimiento.tipo == 2)) {
					printf("\nOperacion: Pago, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
					//chequear la mascara de la fecha
				}
				else {
					if (((movimiento.cuilEnvia == x) && (movimiento.monto >= mnt1)) && (movimiento.monto <= mnt2) && (movimiento.tipo == 3)) {
						printf("\nOperacion: ingreso, monto: $%.2f, fecha: %t, destino: %ld", movimiento.monto, movimiento.fecha, movimiento.cuilRecibe);
						//chequear la mascara de la fecha
					}
				}
			}
		}
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(fp1);
}



//menuListarMovimientosUsuarios();
//1-todos 2-segun tipo > 1-2-3-  3-segun fecha > entre x y y 4-por monto > 1-mayores a 2-menores a

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
