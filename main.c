#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
void actividadUsuario(char*);
void listarMovimientosTXT(char*);
int nombreValido(char *);
int checkAlias(char* , FILE*);

//---------------------------------------- DECLARACION DE ESTRUCTURAS GLOBALES ----------------------------------------
struct struct_usuarioYContra{
	char usuario[50], contrasenia[10];
}usuarioYContra;

struct fecha_str {
    int dia;
    int mes;
    int anio;
} fecha;

struct struct_usuario{
	char nombre[50];
    long int cuil; 
	unsigned long celular;
    char email[30], alias[30], cvu[23], contrasenia[10];
    int iva;
    float saldo;
}usuario, usuarioMenu;

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
				printf("1. Transferir dinero\n");
				printf("2. Listar movimientos\n");
				printf("3. Pagar\n");
				printf("4. Listar pagos\n");
				printf("5. Mostrar mis datos\n");
				printf("6. Mostrar resumen de actividad\n");
				printf("7. Listar mis movimientos (generar archivo de texto)");
				scanf("%d", &usrChoice);

				switch(usrChoice) {
					case 0:
						printf("Volviendo...\n");
						break;
					case 1:
						printf("1");
						transferirDinero(usuarioMenu.cvu);
						break;
					case 2:
						printf("2");
						menuListarMovimientosUsuarios(usuarioMenu.cvu);
						break;
					case 3:
						printf("3");
						hacerPago(usuarioMenu.cvu);
						break;
					case 4:
						printf("4");
						listarSoloPagos(usuarioMenu.cvu);
						break;
					case 5:
						printf("5");
						mostrarUnUsuario(usuarioMenu.cuil);
						break;
					case 6:
						printf("6");
						actividadUsuario(usuarioMenu.cvu);
						break;
					case 7:
						listarMovimientosTXT(usuarioMenu.cvu);
						break;
					default:
						printf("Opcion Invalida\n");
				}
   		 	} while(usrChoice != 0);

	}
	else printf("ERROR: no se pudo abrir el archivo\n");	
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
                printf("Opcion Invalida\n");
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
	FILE *pUsuarios;
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
	} else printf("ERROR: no se pudo abrir el archivo");
	return res;
}


int nombreValido(char *nombre){
	int i;
    for(i = 0; i < strlen(nombre); i++) {
        if ((!isalpha(nombre[i])) && nombre[i] != ' ') {  // Verifica que cada carácter sea una letra
            return 0;               // Retorna 0 si algún carácter no es letra
        }
    }
    return 1; // Retorna 1 si todos los caracteres son letras
}
//-------------------------------------------------------------------------------- FUNCIONES VARIAS --------------------------------------------------------------------------------


void cargarUsuario() {
	char *dominios[] = {"@gmail.com", "@hotmail.com", "@yahoo.com", "@outlook.com", "@live.com", "@icloud.com"};
	int usrChoice, validar, esValido = 0, i, lenEmail, lenDominio;
	FILE *file_usuarios;

	if ((file_usuarios = fopen("Usuarios.dat", "a+b")) != NULL) {

		do {

			generarCvu(usuario.cvu, file_usuarios);
			
			do{
				printf("\nIngrese el nombre -----> ");
				getchar();
				fgets(usuario.nombre, 50, stdin);
				usuario.nombre[strcspn(usuario.nombre, "\n")] = '\0';
				
				//control para que no se ingrese un nombre vacio (un enter)
				if((strlen(usuario.nombre)) <= 0 || (nombreValido(usuario.nombre)) == 0){
					printf("ERROR: nombre no valido\n");
				} 	
			} while( (strlen(usuario.nombre)) <= 0 || (nombreValido(usuario.nombre)) == 0);

			do {
				printf("\nIngrese el cuil ------> ");
				validar = scanf("%ld", &usuario.cuil);
				
				if (validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1) {
					printf("ERROR: el cuil no es valido\n");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while(validar != 1 || checkCuil(usuario.cuil, file_usuarios) != -1);

			do{
				printf("Ingrese una contrasenia de no mas de 9 caracteres -----> ");
				scanf("%s", &usuario.contrasenia);
				while (getchar() != '\n');
				
				if((strlen(usuario.contrasenia)) <= 0 || (strlen(usuario.contrasenia)) >= 10){
					printf("ERROR: contrasenia no valida\n");
				}
			} while( (strlen(usuario.contrasenia)) <= 0 || (strlen(usuario.contrasenia)) >= 10 );
			

			esValido = 0;
			do {
    			printf("\nIngrese el mail ------> ");
    			fgets(usuario.email, 30, stdin);
    			usuario.email[strcspn(usuario.email, "\n")] = '\0'; //Elimina el salto de línea

    			// Recorre los dominios permitidos y verifica si el correo termina con alguno
    			for(i = 0; i < 6; i++) {
        			lenDominio = strlen(dominios[i]);
        			lenEmail = strlen(usuario.email);
        			
      			  if(lenEmail >= lenDominio && strcmp(usuario.email + lenEmail - lenDominio, dominios[i]) == 0 && (strcspn(usuario.email, "@")) > 0){
           			esValido = 1;  // Cambia esValido a 1 si el dominio es válido
            		break;         // Termina el bucle si se encuentra un dominio válido
        			}
    			}
   				 if(esValido != 1) {
        			printf("ERROR: correo no valido, ingrese un correo con un dominio permitido.\n");
    			}  
			} while (esValido != 1);
			
				
				printf("\nIngrese el alias -----> ");
				fgets(usuario.alias, 30, stdin);
				usuario.alias[strcspn(usuario.alias, "\n")] = '\0';

			do {
				printf("\nIngrese el telefono ------> ");
				validar = scanf("%lu", &usuario.celular);
				
				if (validar != 1) {
					printf("ERROR: numero de celular no valido");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while(validar != 1);


			do {
				printf("\nEste usuario paga iva?\n1 == monotributista\n2 == trabajador independiente\n3 == responsable inscripto\n4 == excento\n5 == no paga iva\n6 == no paga iva\n7 == no paga iva\n8 == no paga iva\n9 == no paga iva\n ----->  ");
				validar = scanf("%d", &usuario.iva);
				
				if (validar != 1 || usuario.iva < 1 || usuario.iva > 9) {
					printf("ERROR: el numero ingresado no es una opcion\n");
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
					printf("ERROR: no ingreso 1 o 0\n");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while (validar != 1 || usrChoice != 1 && usrChoice != 0);



		} while (usrChoice != 0);

	}
	else printf("\nERROR: no se pudo abrir el archivo"); 
	
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

                for(i = 11, j = 0; i < 21; i++, j++){ 

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
			printf("\nUsuario: %s\nCuil: %ld\nContrasenia: %s\nCVU: %s\nCelular: %lu\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f\n",usuario.nombre, usuario.cuil, usuario.contrasenia, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);
			fread(&usuario,sizeof(struct struct_usuario),1,f);
		}
		printf("\n--------------------------------------------------\n");
		fclose(f);
	} else printf("ERROR: no se pudo abrir el archivo");
}

void transferirDinero(char* origen) {
	FILE *fp1, *fp2;
	int encon, modif, encontrado1 = 0, posDestino = 0;
	float monto;
	char cvuOrigen[23], cvuDestino[23];

	//strcpy(cvuOrigen, origen);
	encon = 0;
	modif = 0;

	do {
		fp1 = fopen("Usuarios.dat", "r+b");
		printf("Ingrese el cvu destinatario -----> ");
		scanf("%s", &cvuDestino);
		posDestino = checkCvu(cvuDestino, fp1);
		if (posDestino == -1) {
			printf("ERROR: cvu no encontrado\n");
			if((strcmp(origen, cvuDestino)) == 0){
				printf("ERROR: el cvu de origen es el mismo que el de destino\n");
			}
		} 
		

	} while (posDestino == -1 && (strcmp(origen, cvuDestino)) == 0);
	fclose(fp1);

	do {
		printf("Ingrese el dinero que desea transferir -----> ");
		scanf("%f", &monto);
		if (monto <= 0) {
			printf("ERROR: el monto ingresado es menor a 1 peso\n");
		}
	} while (monto <= 0);


	if ((checkMonto(origen, monto)) == 1) {
		printf("\nMontos insuficientes para la transaccion");
	}
	else {
		fp1 = fopen("Usuarios.dat", "r+b");
		fp2 = fopen("Movimientos.dat", "a+b");
		if ((fp1 != NULL) && (fp2 != NULL)) {
			fread(&usuario, sizeof(struct struct_usuario), 1, fp1);
			while (encontrado1 != 1 && !feof(fp1)) {
				if (usuarioMenu.cuil == usuario.cuil) {
					
					encontrado1 = 1;
					usuarioMenu.saldo = usuarioMenu.saldo - monto;
					usuario.saldo = usuarioMenu.saldo;
					fseek(fp1, (sizeof(struct struct_usuario)) * (-1), SEEK_CUR);
					fwrite(&usuario, sizeof(struct struct_usuario), 1, fp1);
					
				} else fread(&usuario, sizeof(struct struct_usuario), 1, fp1);
			}

			//leemos los datos del usuario destino
			fseek(fp1, (sizeof(struct struct_usuario)) * (posDestino), SEEK_SET);
			fread(&usuario, sizeof(struct struct_usuario), 1, fp1);

			movimiento.tipo = 1;
			movimiento.monto = monto;
			movimiento.dia = fecha.dia;
			movimiento.mes = fecha.mes;
			movimiento.anio = fecha.anio;
			strcpy(movimiento.cvuOrigen, origen);
			strcpy(movimiento.cvuDestino, usuario.cvu);

			//condiciones para el iibb
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
			fseek(fp1, (sizeof(struct struct_usuario)) * (posDestino), SEEK_SET);
			fwrite(&usuario, sizeof(struct struct_usuario), 1, fp1);

			//cerramos los archivos
			fclose(fp1);
			fclose(fp2);
		}
		else printf("\nERROR: no se pudo abrir el archivo\n");
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
        printf("ERROR: el nombre de la empresa es muy largo. Intente nuevamente.");
        return;
    }

    do {
        printf("Ingrese el monto a pagar -----> ");
        scanf("%f", &monto);
        if (monto <= 0) {
            printf("ERROR: monto no valido. Ingrese otro\n");
        }
    } while (monto <= 0);

    if (checkMonto(cvuUsuario, monto) == 1) {
        printf("Saldo insuficiente para realizar el pago.\n");
        return;
    }

    fpUsuario = fopen("Usuarios.dat", "r+b");
    fpMovimiento = fopen("Movimientos.dat", "a+b");

    if (fpUsuario != NULL && fpMovimiento != NULL) {
    	fread(&usuario, sizeof(struct struct_usuario), 1, fpUsuario);
        while ( !feof(fpUsuario) && !encontrado) {
            if (strcmp(usuario.cvu, cvuUsuario) == 0) {
                encontrado = 1;
                usuario.saldo -= monto;

                fseek(fpUsuario, (sizeof(struct struct_usuario))*(-1), SEEK_CUR);
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
            } else fread(&usuario, sizeof(struct struct_usuario), 1, fpUsuario);
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
	int parar=1, posicion=0, validar;
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
		
			do {
				printf("Desea cargar otro saldo? (1 == si // 0 == no) ----->   ");
				validar = scanf("%i",&parar);
				
				if (validar != 1 || parar != 1 && parar != 0) {
					printf("ERROR: no ingreso 1 o 0\n");
					while (getchar() != '\n'); //reemplazo de fflush(stdin)
				}
			} while (validar != 1 || parar != 1 && parar != 0);

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
		printf("Ingrese su cuil (admin == 0)-----> ");
		scanf("%ld",&cuil);
		
		//bloque de admin, es decir, si el usuario ingresado es "admin"
		if(cuil == 0){

			//le pedimos la contraseniaa del administrador que es 1234
			printf("Ingrese la contrasenia para ingresar como administrador (1234) -----> ");
			scanf("%s", &contrasenia);
			while (getchar() != '\n');
			
				if(strcmp(contrasenia,"1234") == 0){
					return -2;
				} else {
					printf("Contrasenia incorrecta\n");
					return -1;
				}
		} else if(f_cuenta != NULL && (posUsuario = checkCuil(cuil, f_cuenta)) != -1){

		 	printf("Ingrese su contrasenia -----> ");
			scanf("%s", &contrasenia);
			while (getchar() != '\n');
		 	
		 	rewind(f_cuenta);
			fread(&usuario, sizeof(struct struct_usuario), 1, f_cuenta);
			while(!feof(f_cuenta)){
				if((strcmp(contrasenia, usuario.contrasenia)) == 0){
					return posUsuario;
				} else fread(&usuario, sizeof(struct struct_usuario), 1, f_cuenta);
			} 
                printf("Contrasenia incorrecta\n");
                fclose(f_cuenta);
                return -1;
                
            } else {
                printf("El usuario ingresado no existe\n");
                fclose(f_cuenta);
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
		fclose(pUsuarios);
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
		fclose(pUsuarios);
	}
}

void actividadUsuario(char* clienteCvu) {
	FILE* pMovimientos = fopen("Movimientos.dat", "rb");
	char CVU[23];
	int cont = 0, contTransfe = 0, contIngreso = 0, contPago = 0;
	float tTransfe = 0, tIngreso = 0, tPago = 0;

	strcpy(CVU, clienteCvu);

	if (pMovimientos == NULL) {
		printf("\nERROR DE APERTURA DE ARCHIVO DE MOVIMIENTOS");
	}
	else {
		
		fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);

		while (!feof(pMovimientos)) {

			if (strcmp(movimiento.cvuOrigen, CVU) == 0) {

				switch (movimiento.tipo){
				case 1:
					cont++;
					tTransfe = tTransfe + movimiento.monto;
					contTransfe++;
				break;
				case 2:
					cont++;
					tPago = tPago + movimiento.monto;
					contPago++;
				break;
				}

			}
			else {
				if ((strcmp(movimiento.cvuDestino, CVU) == 0) && (movimiento.tipo == 3)) {
					cont++;
					tIngreso = tIngreso + movimiento.monto;
					contIngreso++;
				}
			}

			fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);
		
		}
		fclose(pMovimientos);
		printf("\nEl usuario con CVU: %s tiene un total de %d movmientos, constituidos por: \n%d Transferencias realizadas por un monto de -$%.2f\n%d Pagos realizados por un monto de -$%.2f\n%d Ingresos recibidos por un monto de +$%.2f", CVU, cont, contTransfe, tTransfe, contPago, tPago, contIngreso, tIngreso);
	}
}


//---------------------------------------- MENU Y FUNCIONES DE LISTAR ----------------------------------------


void saldoBajo(){
    FILE *f_usuario = fopen("Usuarios.dat", "rb");
    float saldo;
    int encontro = 0;

    if(f_usuario != NULL){
        printf("Listar usuarios con el saldo menor a: -----> ");
        scanf("%f",&saldo);

        fread(&usuario, sizeof(struct struct_usuario), 1, f_usuario);
        printf("\n");
        while(!feof(f_usuario)){
            if(usuario.saldo < saldo){
            	encontro = 1;
                printf("El usuario: %s con el cuil: %ld, tiene un saldo de: $%.2f\n", usuario.nombre, usuario.cuil, usuario.saldo);
            }
            fread(&usuario, sizeof(struct struct_usuario), 1, f_usuario);
        }
        if(encontro = 0) printf("No hay usuario con saldo mas bajo que %i", saldo);
        printf("\n");
        fclose(f_usuario);
    } else printf("ERROR: no se pudo abrir el archivo\n");
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

void listarMovimientosTXT(char* Origen) {
	FILE* fp1;
	int cont = 0;
	char OrigenTransfe[23];
	FILE* file_txt;

	strcpy(OrigenTransfe, Origen);

	file_txt = fopen("ListaDeMovimientos.txt", "w");
	if(file_txt == NULL){
		printf("ERRORERRORERRORERRORERROR");
	}

	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		fprintf(file_txt, "RESUMEN DE LA CUENTA DE CVU %s", Origen);
		while (fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1) == 1) {
			if (((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0) || (strcmp(movimiento.cvuDestino, OrigenTransfe) == 0))) {
				if (((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) {
					fprintf(file_txt, "\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
					cont++;
				}
				else {
					if (((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) {
						fprintf(file_txt, "\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
						cont++;
					}
					else {
						if (movimiento.tipo == 2) {
							fprintf(file_txt, "\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
							cont++;
						}
						else {
							if (movimiento.tipo == 3) {
								fprintf(file_txt, "\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
								cont++;
							}
						}
					}
				}
			}
		}
		fprintf(file_txt, "\nHay un total de: %d movimientos", cont);
		printf("\nArchivo creado con exito");
		fclose(fp1);
	}
	else {
		printf("\nERROR AL ABRIR EL ARCHIVO MOVIMIENTOS");
	}
	fclose(file_txt);
}

void listarMovimientos(char* Origen) {
	FILE* fp1;
	int cont = 0;
	char OrigenTransfe[23];

	strcpy(OrigenTransfe, Origen);


	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
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
					printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
					obtenerDatosDestino(movimiento.cvuDestino);
				}
				else {
					if (((strcmp(movimiento.cvuDestino, cvuOrigen) == 0)) && (movimiento.tipo == 1)) {
						printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
						obtenerDatosEmisor(movimiento.cvuOrigen);
					}
					else {
						if (movimiento.tipo == 2) {
							printf("\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
						}
						else {
							if (movimiento.tipo == 3) {
								printf("\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);

							}
						}
					}
				}
			}
			fread(&movimiento, sizeof(struct struct_movimiento), 1, pMovimientos);
		}
		fclose(pMovimientos);
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
		fclose(pMovimientos);
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
		
		fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1);

		while (!feof(fp1)) {
			if ((((strcmp(movimiento.cvuOrigen, OrigenTranfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto > monto){
				printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTranfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto > monto) {
					printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosEmisor(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTranfe) == 0)) && (movimiento.tipo == 2)) && movimiento.monto > monto) {
						printf("\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTranfe) == 0)) && (movimiento.tipo == 3)) && movimiento.monto > monto) {
							printf("\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
						}
					}
				}
			}
			fread(&movimiento, sizeof(struct struct_movimiento), 1, fp1);
		}
		fclose(fp1);
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
				printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && movimiento.monto <= monto) {
					printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosEmisor(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 2)) && movimiento.monto <= monto) {
						printf("\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 3)) && movimiento.monto <= monto) {
							printf("\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
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

	printf("Ingrese valor mï¿½nimo y mï¿½ximo de listado: $");
	scanf("%f", &MontoMenor);
	printf(" y $");
	scanf("%f", &MontoMayor);

	fp1 = fopen("Movimientos.dat", "rb");
	if (fp1 != NULL) {
		rewind(fp1);
		while (fread(&movimiento, sizeof(movimiento), 1, fp1) == 1) {
			if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
				printf("\n\nOperacion: transferencia, monto: -$%.2f, fecha: %d-%d-%d, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino);
				obtenerDatosDestino(movimiento.cvuDestino);
			}
			else {
				if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 1)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
					printf("\n\nOperacion: transferencia, monto: +$%.2f, fecha: %d-%d-%d, origen: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuOrigen);
					obtenerDatosDestino(movimiento.cvuOrigen);
				}
				else {
					if ((((strcmp(movimiento.cvuOrigen, OrigenTransfe) == 0)) && (movimiento.tipo == 2)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
						printf("\n\nOperacion: pago, monto: $%.2f, fecha: %d-%d-%d, destino: %s, destino: %s", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio, movimiento.cvuDestino, movimiento.cvuDestino);
					}
					else {
						if ((((strcmp(movimiento.cvuDestino, OrigenTransfe) == 0)) && (movimiento.tipo == 3)) && ((movimiento.monto > MontoMenor) && (movimiento.monto <= MontoMayor))) {
							printf("\n\nOperacion: ingreso, monto: +$%.2f, fecha: %d-%d-%d", movimiento.monto, movimiento.dia, movimiento.mes, movimiento.anio);
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

			printf("\n\nUsuario: %s\nCuil: %ld\nContrasenia: %s\nCVU: %s\nCelular: %lu\nEmail: %s\nAlias: %s\nIVA: %i\nSaldo: %.2f\n\n",usuario.nombre, usuario.cuil, usuario.contrasenia, usuario.cvu, usuario.celular, usuario.email, usuario.alias, usuario.iva, usuario.saldo);


		}

		fclose(pUsuarios);
	}
}



