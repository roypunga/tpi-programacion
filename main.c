#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define CODIGO_SEGURIDAD1 "3"
#define CODIGO_SEGURIDAD2 "8"
#define CODIGO_PROVEEDOR "6666666"

//---------------------------------------- PROTOTIPO DE FUNCIONES --------------------------------------
int checkCuil(long int cuilABuscar, FILE *file_usuarios);
void menuAdministrador();
void menuUsuario(int);
void generarCvu(char *cadena, FILE *file_usuarios);
void cargarUsuario();
void mostrarUsuarios();
void menuListarMovimientosUsuarios(char *);
void listarSoloPagos(char*);
void listarSoloTrans(char*);
void listarSoloIngr(char*);
void listarEntrFecha(char*);
void listarSoloUnaFecha(char*);
void listarMovsMayrA(char*);
void listarMovsMenrA(char*);
void listarMovsEntre(char*);
void listarMovimientos(char*);
int checkMonto(char *, float );
void transferirDinero(char *);
void cargarSaldo();
int iniciarSesion();
void olvidarContra();
void obtenerFechaActual();
void saldoBajo();
int checkCvu(char *cvuABuscar, FILE *file_usuarios);
void obtenerDatosDestino(char*);
void obtenerDatosEmisor(char*);
void mostrarUnUsuario(long int);
void hacerPago(char* cvuUsuario);


//---------------------------------------- DECLARACION DE ESTRUCTURAS GLOBALES ----------------------------------------
struct struct_usuarioYContra{
	char usuario[50], contrasenia[10];
}usuarioYContra;

struct fecha_str {
    int dia;
    int mes;
    int anio;
    char dia_semana[10];
} fecha;

struct struct_usuario{
	char nombre[50];
    long int cuil, celular;
    char email[30], alias[30], cvu[23], contrasenia[10];
    int iva;
    float saldo;
}usuario, usuarioMenu;

struct struct_cuenta {
    int tipo; //caja de ahorro cuenta corriente
    char cvu[23];
    long int cuil;
}cuenta;

struct struct_movimiento{
    int tipo; //ingreso transferencia o pago
    float monto;
    int iibb;
    int dia, mes, anio;
    char cvuOrigen[23], cvuDestino[23];
}movimiento;



//---------------------------------------- MAIN Y FUNCIONES DE LOS MENUS ----------------------------------------


int usrChoice, menu;
int main() {    
	
	printf("Bienvenido a PagarMercado! A que menu desea acceder?\n");
	obtenerFechaActual();
	
	do {
        printf("0-Salir\n");
		printf("1-Iniciar Sesion\n");
		printf("2-Crear Nuevo Usuario\n");
		printf("3-Me olvide mi contrasenia\n");
        scanf("%d", &usrChoice);	

        switch(usrChoice) {
            case 0:
                printf("Saliendo...\n");
                break;
            case 1:
                menu = iniciarSesion();
                if(menu != -1 && menu != -2) menuUsuario(menu);
				else if (menu == -2) menuAdministrador();
				else if (menu == -1)printf("No se pudo inciar sesion\n");
                break;
            case 2:
                cargarUsuario();
                break;
            case 3:
            	olvidarContra();
            	break;
            default:
                printf("Opcion Invalida\n");
        }
    } while(usrChoice != 0);
}

void menuUsuario(int posUsuario){

	int posicionUsuario = posUsuario;


	FILE *file_usuarios;

	if((file_usuarios = fopen("Usuarios.dat", "r+b")) != NULL){
			
			rewind(file_usuarios);
			fseek(file_usuarios, sizeof(usuario)*(posicionUsuario), SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, file_usuarios);
			usuarioMenu = usuario;
			fclose(file_usuarios);

			printf("\nBienvenido %s! Que operacion desea realizar?\n", usuario.nombre);

			do {
				printf("\n\n0. Volver\n");
				printf("1. Ingresar dinero\n");
				printf("2. Transferir dinero\n");
				printf("3. Listar movimientos\n");
				printf("4. Pagar\n");
				printf("5. Listar pagos\n");
				printf("6. Mostrar mis datos\n");
				scanf("%d", &usrChoice);

				switch(usrChoice) {
					case 0:
						printf("Volviendo...\n");
						break;
					case 1:
						printf("1");
						//ingresarDinero(); esto lo hace el admin en teoria, ya existe la funcion en el menu de administrador. Vean si la eliminan de aca o la habilitan
						break;
					case 2:
						printf("2");
						transferirDinero(usuarioMenu.cvu);
						break;
					case 3:
						printf("3");
						menuListarMovimientosUsuarios(usuarioMenu.cvu);
						break;
					case 4:
						printf("4");
						hacerPago(usuarioMenu.cvu);
						break;
					case 5:
						listarSoloPagos(usuarioMenu.cvu);
						break;
					case 6:
						mostrarUnUsuario(usuarioMenu.cuil); 
						break;
					default:
						printf("Opcion Invalida\n");
				}
   		 	} while(usrChoice != 0);

	}
	else{
		printf("ERROR");
	}

}

void menuAdministrador(){

	char cvu[23];

	long int usrCuil;

		printf("\nBienvenido administrador! Que desea hacer?\n");

		do {
        printf("0-Salir\n");
		printf("1-Crear usuario\n");
		printf("2-Mostrar datos de un usuario\n");
		printf("3-Mostrar los datos de todos los usuarios\n");
		printf("4-Cargar Saldo\n");
		printf("5-Mostrar usuarios con saldo bajo\n-----> ");
		//printf("5-Modificar un usuario\n");//

        scanf("%d", &usrChoice);

        switch(usrChoice) {
            case 0:
                printf("Saliendo...\n");
                break;
            case 1:
				cargarUsuario();
				break;
			case 2:
				printf("\nIngrese el cuil del usuario que quiere mostrar: ");
				scanf("%ld", &usrCuil);
				mostrarUnUsuario(usrCuil); //esto  no te da todos los datos, omite cuil, contrasenia, 
				break;
			case 3:
				mostrarUsuarios(); 
				break;
			case 4:
				cargarSaldo();
				break;
			case 5:
				saldoBajo();
				break;
            default:
                printf("Opción inválida\n");
        }
    } while(usrChoice != 0);

}


//---------------------------------------- FUNCIONES PARA VERIFICAR (CUIL, CVU, SALDO, ETC.) ----------------------------------------


int checkCuil(long int cuilABuscar, FILE *file_usuarios){

    int encontro = 0;
	int contador = 0;

    rewind(file_usuarios);

    struct struct_usuario usuario;

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

int checkCvu(char *cvuABuscar, FILE *file_usuarios){

    int encontro = 0;
	int contador = 0;

    rewind(file_usuarios);

    while(fread(&usuario, sizeof(usuario), 1, file_usuarios) && encontro != 1){

		contador += 1;
        if(!strcmp(usuario.cvu, cvuABuscar)){
            encontro = 1;
        }

    }

    if(encontro == 1){
		return contador - 1;
	}
	return -1;
}

int checkMonto(char* origen, float monto) {
	FILE* pUsuarios;
	int res = 1;
	pUsuarios = fopen("Usuarios.dat", "rb");
	if (pUsuarios != NULL) {
		fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		while (!feof(pUsuarios) && res != 0) {
			if (strcmp(usuario.cvu, origen) == 0) {
				if (usuario.saldo >= monto) {
					res = 0;
				}
			}
			fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		}
		fclose(pUsuarios);
	}
	return res;
}


//-------------------------------------------------------------------------------- FUNCIONES VARIAS --------------------------------------------------------------------------------


void cargarUsuario() {

	int usrChoice, validar;
	FILE* file_usuarios;


	if ((file_usuarios = fopen("Usuarios.dat", "a+b")) != NULL) {

		do {

			generarCvu(usuario.cvu, file_usuarios);

			printf("\nIngrese el nombre -----> ");
			getchar();
			fgets(usuario.nombre, 50, stdin);
			usuario.nombre[strcspn(usuario.nombre, "\n")] = '\0';


		denuevo:

			do {
				printf("\nIngrese el cuil ------> ");
				validar = scanf("%ld", &usuario.cuil);
				if (validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1) {
					printf("ERROR: el cuil no es valido\n");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while (validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1);


			printf("Ingrese una contrasenia de no mas de 9 caracteres -----> ");
			scanf("%s", &usuario.contrasenia);
			while (getchar() != '\n');


			printf("\nIngrese el mail ------> ");
			fgets(usuario.email, 30, stdin);
			usuario.email[strcspn(usuario.email, "\n")] = '\0';


			printf("\nIngrese el alias -----> ");
			fgets(usuario.alias, 30, stdin);
			usuario.alias[strcspn(usuario.alias, "\n")] = '\0';

			do {
				printf("\nIngrese el telefono ------> ");
				validar = scanf("%ld", &usuario.celular);
				if (validar != 1) {
					printf("ERROR: numero de celular no valido");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while (validar != 1);


			do {
				printf("\nEste usuario paga iva?\n1 == monotributista\n2 == trabajador independiente\n3 == responsable inscripto\n4 == excento\n5 == no paga iva\n6 == no paga iva\n7 == no paga iva\n8 == no paga iva\n9 == no paga iva\n ----->  ");
				validar = scanf("%d", &usuario.iva);
				if (validar != 1 || usuario.iva < 1 || usuario.iva > 9) {
					printf("ERROR: no ingreso 1 o 0");
					while (getchar() != '\n'); //reemplazo de fflush(stdin);
				}
			} while (validar != 1 || usuario.iva < 1 || usuario.iva > 9);


			usuario.saldo = 0;

			fseek(file_usuarios, 0, SEEK_END);

			fwrite(&usuario, sizeof(usuario), 1, file_usuarios);

			do {
				printf("El usuario se cargo exitosamente, YA PUEDE INICIAR SESION\n");
				printf("\nDesea cargar otro usuario? (1 == si // 0 == no) ----->  ");
				validar = scanf("%d", &usrChoice);
				if (validar != 1 || usrChoice != 1 && usrChoice != 0) {
					printf("ERROR: no ingreso 1 o 0");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while (validar != 1 || usrChoice != 1 && usrChoice != 0);



		} while (usrChoice != 0);

	}
	else {
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
			printf("\nUsuario: %s\nCuil: %ld\nContrasenia: %s\nCVU: %s\nCelular: %li\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f\n",usuario.nombre, usuario.cuil, usuario.contrasenia, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);
			fread(&usuario,sizeof(struct struct_usuario),1,f);
		}
		printf("\n--------------------------------------------------\n");
		fclose(f);
	}
}

void transferirDinero(char* origen) {
	FILE* fp1, * fp2;
	int encon, modif, encontrado1 = 0, posDestino = 0;
	float monto;
	char cvuOrigen[23], cvuDestino[23];

	strcpy(cvuOrigen, origen);
	encon = 0;
	modif = 0;

	do {
		fp1 = fopen("Usuarios.dat", "r+b");
		printf("Ingrese el cvu destinatario -----> ");
		scanf("%s", &cvuDestino);
		posDestino = checkCvu(cvuDestino, fp1);
		if (posDestino == -1) {
			printf("Cvu no encontrado");
		}

	} while (posDestino == -1);
	fclose(fp1);

	do {
		printf("Ingrese el dinero que desea transferir -----> ");
		scanf("%f", &monto);
		if (monto <= 0) {
			printf("Monto no valido. Ingrese otro\n");
		}
	} while (monto <= 0);


	if ((checkMonto(cvuOrigen, monto)) == 1) {
		printf("\nMontos insuficientes para la transaccion");
	}
	else {
		fp1 = fopen("Usuarios.dat", "r+b");
		fp2 = fopen("Movimientos.dat", "a+b");
		if ((fp1 != NULL) && (fp2 != NULL)) {
			fread(&usuario, sizeof(struct struct_usuario), 1, fp1);
			while (encontrado1 != 1 && !feof(fp1)) {
				if (usuarioMenu.cuil == usuario.cuil) {
					printf("saldo antes de descontar == %f\n", usuarioMenu.saldo);
					encontrado1 = 1;
					usuarioMenu.saldo = usuarioMenu.saldo - monto;
					usuario.saldo = usuarioMenu.saldo;
					fseek(fp1, (sizeof(struct struct_usuario)) * (-1), SEEK_CUR);
					fwrite(&usuario, sizeof(struct struct_usuario), 1, fp1);
					printf("despues del fwrite usuarioMenu == %f ----- usuario.saldo == %f ----- nombre == %s", usuarioMenu.saldo, usuario.saldo, usuario.nombre);
				}
				else fread(&usuario, sizeof(struct struct_usuario), 1, fp1);
			}

			//leemos los datos del usuario destino

			fseek(fp1, sizeof(struct struct_usuario) * posDestino, SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, fp1);

			movimiento.tipo = 1;
			movimiento.monto = monto;
			movimiento.dia = fecha.dia;
			movimiento.mes = fecha.mes;
			movimiento.anio = fecha.anio;
			strcpy(movimiento.cvuOrigen, cvuOrigen);
			strcpy(movimiento.cvuDestino, usuario.cvu);

			if (usuario.iva >= 5 && usuario.iva <= 9) {
				movimiento.iibb = 0;
				usuario.saldo = usuario.saldo + monto;

			}
			else {
				movimiento.iibb = 1;
				fwrite(&movimiento, sizeof(struct struct_movimiento), 1, fp2);
				movimiento.monto = monto * 0.05;
				movimiento.tipo = 4;
				usuario.saldo = usuario.saldo + (monto - (monto * 0.05));
			}

			//printeamos los datos
			fwrite(&movimiento, sizeof(struct struct_movimiento), 1, fp2);
			fseek(fp1, sizeof(struct struct_usuario) * posDestino, SEEK_SET);
			fwrite(&usuario, sizeof(struct struct_usuario), 1, fp1);

			//cerramos los archivos
			fclose(fp1);
			fclose(fp2);
		}
		else {
			printf("\nError de apertura de archivos");
		}
	}
}


void hacerPago(char* cvuUsuario) {
	
    FILE* fpUsuario, * fpMovimiento;
    int encontrado = 0;
    float monto;
    char empresa[23];


    printf("Ingrese el nombre de la empresa a pagar -----> ");
    scanf("%s", empresa);
    if (strlen(empresa) >= 23) {
        printf("El nombre de la empresa es muy largo. Intente nuevamente.");
        return;
    }

    do {
        printf("Ingrese el monto a pagar -----> ");
        scanf("%f", &monto);
        if (monto <= 0) {
            printf("Monto no válido. Ingrese otro\n");
        }
    } while (monto <= 0);

    if (checkMonto(cvuUsuario, monto) == 1) {
        printf("Saldo insuficiente para realizar el pago.\n");
        return;
    }

    fpUsuario = fopen("Usuarios.dat", "r+b");
    fpMovimiento = fopen("Movimientos.dat", "a+b");

    if (fpUsuario != NULL && fpMovimiento != NULL) {
        while (fread(&usuario, sizeof(struct struct_usuario), 1, fpUsuario) == 1 && !encontrado) {
            if (strcmp(usuario.cvu, cvuUsuario) == 0) {
                encontrado = 1;
                usuario.saldo -= monto;

                fseek(fpUsuario, -sizeof(struct struct_usuario), SEEK_CUR);
                fwrite(&usuario, sizeof(struct struct_usuario), 1, fpUsuario);

                movimiento.tipo = 2;
                movimiento.monto = monto;
                movimiento.dia = fecha.dia;
                movimiento.mes = fecha.mes;
                movimiento.anio = fecha.anio;
                strcpy(movimiento.cvuOrigen, usuario.cvu);
                strcpy(movimiento.cvuDestino, empresa);

                fwrite(&movimiento, sizeof(struct struct_movimiento), 1, fpMovimiento);
                printf("Pago realizado exitosamente.\n");
            }
        }

        // Cerrar archivos
        fclose(fpUsuario);
        fclose(fpMovimiento);

        if (!encontrado) {
            printf("Usuario no encontrado.\n");
        }
    } else {
        printf("Error al abrir archivos.\n");
    }
}

void cargarSaldo(){
	FILE *f_usuarios=fopen("Usuarios.dat","r+b"), *pMovimientos=fopen("Movimientos.dat","ab");
	int parar=1, posicion=0;
	float saldo;
	long int cuil;
	
	if(f_usuarios!=NULL){
		while(parar!=0){
		printf("Ingrese el cuil de la persona que desea cargar saldo -----> ");
		scanf("%ld",&cuil);
		
		if((posicion = checkCuil(cuil, f_usuarios)) != -1){

			
			fseek(f_usuarios,sizeof(struct struct_usuario)*posicion,SEEK_SET);
			fread(&usuario,sizeof(struct struct_usuario),1,f_usuarios);
			fseek(f_usuarios,sizeof(struct struct_usuario)*posicion,SEEK_SET);
			
			printf("Ingrese la cantidad de dinero a cargar -----> ");
			scanf("%f",&saldo);
			usuario.saldo = saldo + usuario.saldo;

			movimiento.anio = fecha.anio;
			movimiento.mes = fecha.mes;
			movimiento.dia = fecha.dia;
			movimiento.tipo = 3;
			movimiento.iibb = 0;
			strcpy(movimiento.cvuDestino, usuario.cvu);
			strcpy(movimiento.cvuOrigen, "0");
			movimiento.monto = saldo;
			
			fwrite(&usuario,sizeof(struct struct_usuario),1,f_usuarios);
			
			fwrite(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);

				
		} else printf("\nERROR: no se encontro el cuil ingresado\n");
		
		printf("Desea cargar otro saldo? (1 == si // 0 == no) ----->   ");
		scanf("%i",&parar);
	}
	fclose(f_usuarios);
	fclose(pMovimientos);
	} else printf("\nERROR: no se pudo abrir el archivo");
	
}

int iniciarSesion(){
	
	FILE *f_cuenta = fopen("Usuarios.dat","rb");
	char contrasenia[10];
	long int cuil;
	int posUsuario;
		
		//le pedimos el cuil
		printf("Ingrese su cuil (admin = 0000)-----> ");
		scanf("%ld",&cuil);
		
		//bloque de admin, es decir, si el usuario ingresado es "admin"
		if(cuil == 0000){

			//le pedimos la contrase�a del administrador que es 1234
			printf("Ingrese la contrasenia para ingresar como administrador (1234) -----> ");
			scanf("%s",&contrasenia);
			while (getchar() != '\n');
			
				if(strcmp(contrasenia,"1234") == 0){
					return -2;
				} else {
					printf("Contrasenia incorrecta");
					return -1;
				}
		} else if(f_cuenta != NULL && (posUsuario = checkCuil(cuil, f_cuenta)) != -1){

		 	printf("Ingrese su contrasenia -----> ");
			scanf("%s",&contrasenia);
			while (getchar() != '\n');
		 	
		 	rewind(f_cuenta);
			fread(&usuario,sizeof(struct struct_usuario),1,f_cuenta);
			while(!feof(f_cuenta)){
				if(strcmp(contrasenia, usuario.contrasenia)==0){
					return posUsuario;
				} else fread(&usuario,sizeof(struct struct_usuario),1,f_cuenta);
			} 
                printf("Contrasenia incorrecta\n");
                fclose(f_cuenta);
                return -1;
                
            } else {
                printf("El usuario ingresado no existe\n");
                fclose(f_cuenta);
                return -1;
            }
		return -1;
} 	
	
void olvidarContra(){
	FILE *f = fopen("Usuarios.dat","rb"), *f_txt = fopen("olvidar.txt","wb");
	long int cuil;
	int pos, validar=0;
	
	do{
		printf("\nIngrese su cuil ------> ");
		validar = scanf("%ld", &cuil);
		if (validar != 1){
				printf("ERROR: el cuil no es valido\n");
				while (getchar() != '\n');
		}
	} while (validar != 1);
	
	if(f!=NULL && f_txt != NULL){
		if((pos = checkCuil(cuil,f)) != -1){

			fseek(f, sizeof(struct struct_usuario)*pos, SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, f);
			fseek(f, sizeof(struct struct_usuario)*pos, SEEK_SET);
			
			fprintf(f_txt,"La contrasenia de la cuenta con el cuil: %ld. Es: %s",usuario.cuil,usuario.contrasenia);
			printf("Archivo txt generado exitosamente\n");
			fclose(f);
			fclose(f_txt);
			
		} else printf("ERROR: el cuil no existe en nuestra base de datos\n");
	} else printf("ERROR: no se pudo abrir el archivo\n");
}	
	
void obtenerFechaActual(){
    time_t tiempo_actual = time(NULL);
    struct tm tiempo_local = *localtime(&tiempo_actual);

    fecha.dia = tiempo_local.tm_mday;
    fecha.mes = tiempo_local.tm_mon + 1;
    fecha.anio = tiempo_local.tm_year + 1900;

    const char *dias_semana[] = {"Domingo", "Lunes", "Martes", "Mi�rcoles", "Jueves", "Viernes", "S�bado"};
    strcpy(fecha.dia_semana, dias_semana[tiempo_local.tm_wday]);
}

void obtenerDatosDestino(char *CVU){
	FILE* pUsuarios = fopen("Usuarios.dat", "rb");
	char objetivo[23];
	int encontro = 0;

	strcpy(objetivo, CVU);
	if (pUsuarios == NULL) {
		printf("\nERROR DE OBTENCION DE DATOS");
	}
	else {
		fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		while (!feof(pUsuarios) && encontro == 0) {
			if (strcmp(usuario.cvu, objetivo) == 0) {
				printf("\nDatos del destinatario: nombre: %s, alias: %s, cuil: %li", usuario.nombre, usuario.alias, usuario.cuil);
				encontro = 1;
			}
			fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		}
	}
}

void obtenerDatosEmisor(char* CVU) {
	FILE* pUsuarios = fopen("Usuarios.dat", "rb");
	char objetivo[23];
	int encontro = 0;

	strcpy(objetivo, CVU);
	if (pUsuarios == NULL) {
		printf("\nERROR DE OBTENCION DE DATOS");
	}
	else {
		fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		while (!feof(pUsuarios) && encontro == 0) {
			if (strcmp(usuario.cvu, objetivo) == 0) {
				printf("\nDatos del emisor: nombre: %s, alias: %s, cuil: %li", usuario.nombre, usuario.alias, usuario.cuil);
				encontro = 1;
			}
			fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);
		}
	}
}


//---------------------------------------- MENU Y FUNCIONES DE LISTAR ----------------------------------------


void saldoBajo(){
    FILE *f_usuario = fopen("Usuarios.dat", "rb");
    float saldo;

    if(f_usuario != NULL){
        printf("Listar usuarios con el saldo menor a: -----> ");
        scanf("%f",&saldo);

        fread(&usuario, sizeof(struct struct_usuario), 1, f_usuario);
        printf("\n");
        while(!feof(f_usuario)){
            if(usuario.saldo < saldo){
                printf("El usuario: %s con el cuil: %ld, tiene un saldo de: $%.2f\n", usuario.nombre, usuario.cuil, usuario.saldo);
            }
            fread(&usuario, sizeof(struct struct_usuario), 1, f_usuario);
        }
        printf("\n");
        fclose(f_usuario);
    } else printf("ERROR: No se pudo abrir el archivo\n");
}

void menuListarMovimientosUsuarios(char *origen){
	
	float monto1=0.0, monto2=0.0;
	int opc, usrChoiceSeTip, cnt=0, cntG=0;
	char CvuOrigen[23];

	strcpy(CvuOrigen, origen);
	
	printf("\nQue movimientos desea listar? \n\t0.Salir.\n\t1.Todos. \n\t2.Segun tipo. \n\t3.Segun fechas. \n\t4.Segun monto.\n");
	while(cntG==0){
	scanf("%d", &opc);
		switch(opc){
			case 0:
				printf("\nSaliendo...");
				cntG=1;
			break;
			case 1:
				listarMovimientos(CvuOrigen);
				cntG=1;
			break;
			case 2: 
				printf("\nListar segun tipo: \n\t1.Transferencias. \n\t2.Ingresos. \n\t3.Pagos.\n");
				while(cnt==0){
				scanf("%d", &usrChoiceSeTip);
					switch (usrChoiceSeTip){
						case 1:
							listarSoloTrans(CvuOrigen);
							cnt=1;
						break;
						case 2:
							listarSoloIngr(CvuOrigen);
							cnt=1;
						break;
						case 3:
							listarSoloPagos(CvuOrigen);
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
							listarEntrFecha(CvuOrigen);
							cnt=1;
						break;
						case 2:
							listarSoloUnaFecha(CvuOrigen);
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
							listarMovsMayrA(CvuOrigen);
							cnt=1;
						break;
						case 2:
							listarMovsMenrA(CvuOrigen);
							cnt=1;
						break;
						case 3:
							listarMovsEntre(CvuOrigen);
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

void listarMovimientos(char* Origen) {
	FILE* fp1;
	int cont = 0;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);


	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		printf("\nUsuario tiene este cvu: %s", Origen);
		while (fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1) == 1) {
			if (((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0) || (strcmp(movimiento.cvuDestino, OrigenTransfe) == 0))) {
				if (((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) {
					printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
					obtenerDatosDestino(movimiento.cvuDestino);
					cont++;
				}
				else {
					if (((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) {
						printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
						obtenerDatosEmisor(movimiento.cvuOrigen);
						cont++;
					}
					else {
						if (movimiento.tipo == 2) {
							printf("\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
							cont++;
						}
						else {
							if (movimiento.tipo == 3) {
								printf("\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
								cont++;
							}
						}
					}
				}
			}
		}
		printf("\nHay un total de: %d", cont);
		fclose(fp1);
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarSoloTrans(char *Origen){
	
	FILE *fp1;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);

	fp1=fopen("Movimientos.dat", "rb");
	if(fp1!=NULL){
		rewind(fp1);
		while(fread(&movimiento, sizeof(struct struct_movimiento),1,fp1) == 1){
			if( ((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0))&&(movimiento.tipo==1) ){
				printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else
				if ((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0) && (movimiento.tipo) == 1) {
					printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
					obtenerDatosEmisor(movimiento.cvuOrigen);
				}
		}
		fclose(fp1);
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarSoloIngr(char *Origen){
	FILE *fp1;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);

	fp1=fopen("Movimientos.dat", "rb");
	
	if(fp1!=NULL){
		rewind(fp1);
		while(fread(&movimiento, sizeof(struct struct_movimiento),1,fp1) == 1){
			if (((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 3)) {
				printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
			}
		}
		fclose(fp1);
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarSoloPagos (char *Origen){
	FILE* fp1;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);

	fp1=fopen("Movimientos.dat", "rb");
	if(fp1!=NULL){
		rewind(fp1);
		while(fread(&movimiento, sizeof(movimiento),1,fp1)==1){
			if (((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 2)) {
				printf("\nOperacion: pago, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
			}
		}
		fclose(fp1);
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarEntrFecha(char* Origen) {
	FILE* pMovimientos = fopen("Movimientos.dat", "rb");
	int diaMenor, diaMayor, mesMenor, mesMaayor, anioMenor, anioMayor, fechaValida;
	char cvuOrigen[23];

	strcpy(cvuOrigen, Origen);

	printf("\nIngrese la fecha menor de listado (dd mm aaaa): ");
	scanf("%d %d %d", &diaMenor, &mesMenor, &anioMenor);

	printf("\nIngrese la fecha mayor de listado (dd mm aaaa): ");
	scanf("%d %d %d", &diaMayor, &mesMaayor, &anioMayor);

	if (pMovimientos == NULL) {

		printf("\nERROR DE APERTURA DE ARCHIVO MOVIMIENTOS");

	}
	else {

		fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);

		while (!feof(pMovimientos)) {

			fechaValida = 0;

			if ((movimiento.anio > anioMenor ||
				(movimiento.anio == anioMenor && movimiento.mes > mesMenor) ||
				(movimiento.anio == anioMenor && movimiento.mes == mesMenor && movimiento.dia >= diaMenor))
				&&
				(movimiento.anio < anioMayor ||
					(movimiento.anio == anioMayor && movimiento.mes < mesMaayor) ||
					(movimiento.anio == anioMayor && movimiento.mes == mesMaayor && movimiento.dia <= diaMayor))) {
				fechaValida = 1;
			}

			if (((strcmp(cvuOrigen, movimiento.cvuOrigen) == 0) || (strcmp(cvuOrigen, movimiento.cvuDestino) == 0)) && fechaValida == 1) {

				if (((strcmp(movimiento.cvuOrigen, cvuOrigen) == 0)) && (movimiento.tipo == 1)) {
					printf("\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
					obtenerDatosDestino(movimiento.cvuDestino);
				}
				else {
					if (((strcmp(movimiento.cvuDestino, cvuOrigen) == 0)) && (movimiento.tipo == 1)) {
						printf("\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
						obtenerDatosEmisor(movimiento.cvuOrigen);
					}
					else {
						if (movimiento.tipo == 2) {
							printf("\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
						}
						else {
							if (movimiento.tipo == 3) {
								printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);

							}
						}
					}
				}
			}
			fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);
		}
	}
}

void listarSoloUnaFecha(char* Origen) {
	FILE* pMovimientos = fopen("Movimientos.dat", "rb");
	char cvuOrigen[23];
	int dia, mes, anio, fechaValida = 0;

	strcpy(cvuOrigen, Origen);

	printf("\nINgrese la fecha para buscar todas las operaciones en ese dia (dd mm aaaa): ");
	scanf("%d %d %d", &dia, &mes, &anio);

	if (pMovimientos == NULL) {
		printf("\nERROR DE APERTURA DE ARCHIVO DE MOVIMIENTOS");
	}
	else {
		
		fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);

		while (!feof(pMovimientos)) {

			fechaValida = 0;

			if ((movimiento.anio == anio) && (movimiento.mes == mes) && (movimiento.dia == dia)) {
				fechaValida = 1;
			}

			if (((strcmp(cvuOrigen, movimiento.cvuOrigen) == 0) || (strcmp(cvuOrigen, movimiento.cvuDestino) == 0)) && fechaValida == 1) {

				if (((strcmp(movimiento.cvuOrigen, cvuOrigen) == 0)) && (movimiento.tipo == 1)) {
					printf("\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
					obtenerDatosDestino(movimiento.cvuDestino);
				}
				else {
					if (((strcmp(movimiento.cvuDestino, cvuOrigen) == 0)) && (movimiento.tipo == 1)) {
						printf("\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
						obtenerDatosEmisor(movimiento.cvuOrigen);
					}
					else {
						if (movimiento.tipo == 2) {
							printf("\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
						}
						else {
							if (movimiento.tipo == 3) {
								printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);

							}
						}
					}
				}
			}
			fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);
		}
	}
}

void listarMovsMayrA (char *Origen){
	FILE *fp1;
	char OrigenTranfe[23];
	float monto;

	strcpy(OrigenTranfe, Origen);

	printf("\nIngrese monto minimo a partir del cual listar: $");
	scanf("%f", &monto);

	fp1=fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		while (fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1) == 1) {
			if ((((strcmp(movimiento.cvuOrigen, OrigenTranfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto > monto){
				printf("\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTranfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto > monto) {
					printf("\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosEmisor(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTranfe) == 0)) && (movimiento.tipo == 2)) && movimiento.monto > monto) {
						printf("\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTranfe) == 0)) && (movimiento.tipo == 3)) && movimiento.monto > monto) {
							printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
						}
					}
				}
			}
			fclose(fp1);
		}
	}
	else{
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarMovsMenrA(char *Origen) {
	FILE* fp1;
	char OrigenTransfe[23];
	float monto;
	
	strcpy(OrigenTransfe, Origen);

	printf("\nIngrese el monto maximo de listado, solo se mostraran movimiento con montos menores a: $");
	scanf("%f", &monto);

	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		while (fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1) == 1) {
			if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto <= monto) {
				printf("\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto <= monto) {
					printf("\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosEmisor(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 2)) && movimiento.monto <= monto) {
						printf("\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 3)) && movimiento.monto <= monto) {
							printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
						}
					}
				}
			}
		}
		fclose(fp1);
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void listarMovsEntre(char *Origen) {
	FILE* fp1;
	float MontoMenor, MontoMayor;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);

	printf("Ingrese valor m�nimo y m�ximo de listado: $");
	scanf("%f", &MontoMenor);
	printf(" y $");
	scanf("%f", &MontoMayor);

	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		while (fread(&movimiento, sizeof(movimiento), 1, fp1) == 1) {
			if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
				printf("\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
					printf("\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosDestino(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 2)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
						printf("\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 3)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
							printf("\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
						}
					}
				}
			}
		}
		fclose(fp1);
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
}

void mostrarUnUsuario(long int cuil) {
	FILE* pUsuarios = fopen("Usuarios.dat", "rb");
	int posicion;

	if (pUsuarios == NULL) {
		printf("ERROR DE APERTURA");
	}
	else {
	
		if((posicion = checkCuil(cuil, pUsuarios)) == -1){
			
			printf("\nNo se encontro ese CUIL");

		}
		else {
			
			fseek(pUsuarios, (sizeof(struct struct_usuario) * posicion), SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, pUsuarios);

			printf("\n\nUsuario: %s\nCuil: %ld\nContrasenia: %s\nCVU: %s\nCelular: %li\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f\n\n",usuario.nombre, usuario.cuil, usuario.contrasenia, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);


		}

	}
}


//--------------------------------------------------------------------------------CONSIGNAS--------------------------------------------------------------------------------
/*1. Saldo de usuario: Permitir al usuario consultar su saldo actual en la cuenta. X


2. Movipmientos por fecha: Filtrar y listar todos los movimientos realizados por un usuario en un
rango de fechas específico.X

3. Movimientos por tipo: Permitir al usuario ver solo los movimientos de un tipo específico (ingresos,
transferencias, pagos).X

4. Movimientos por monto: Listar todos los movimientos que superen o estén por debajo de un
monto específico.X

5. Transferencias recibidas: Mostrar todas las transferencias que un usuario ha recibido, incluyendo
detalles del remitente.X

6. Transferencias realizadas: Listar todas las transferencias que un usuario ha realizado, incluyendo
detalles del destinatario.X

7. Pagos realizados: Mostrar un listado de todos los pagos que un usuario ha realizado a terceros. X

8. Movimientos con retención de IIBB: Filtrar y listar los movimientos que han tenido retención de
Ingresos Brutos.

9. Usuarios con saldo bajo: Identificar y listar usuarios cuyo saldo esté por debajo de un umbral
específico. X

10. Actividad de usuario: Proporcionar un resumen de la actividad de un usuario, incluyendo el
número total de movimientos, total ingresado, total transferido y total pagado
*/
