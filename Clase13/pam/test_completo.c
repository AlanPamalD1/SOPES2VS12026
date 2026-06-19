#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pam.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "uso: %s usuario contraseña\n", argv[0]);
        return 2;
    }

    char *pwdup = strdup(argv[2]); // Password
    struct pam_conv conv = { pam_conv_callback, pwdup };
    pam_handle_t *pamh = NULL;
    int rc;

    // 1. Inicializar. NOTA: Usamos "mi_app" en lugar de "login"
    rc = pam_start("mi_app", argv[1], &conv, &pamh);
    if (rc != PAM_SUCCESS) {
        free(pwdup);
        return rc;
    }

    // 2. Autenticación (¿Es quien dice ser?)
    rc = pam_authenticate(pamh, 0);
    
    // 3. Gestión de Cuenta (¿La cuenta está expirada? ¿Tiene permitido entrar a esta hora?)
    if (rc == PAM_SUCCESS) {
        rc = pam_acct_mgmt(pamh, 0);
    }

    // 4. Establecer Credenciales (Inicializa tickets de red, IDs suplementarios de PAM)
    if (rc == PAM_SUCCESS) {
        rc = pam_setcred(pamh, PAM_ESTABLISH_CRED);
    }

    // 5. Gestión de Sesión (Montar directorios, escribir en /var/log/wtmp)
    if (rc == PAM_SUCCESS) {
        rc = pam_open_session(pamh, 0);
        
        if (rc == PAM_SUCCESS) {
            printf("--- SESION INICIADA PARA %s ---\n", argv[1]);
            
            // AQUI VA LA LÓGICA DE TU APLICACIÓN
            // Por ejemplo: lanzar un shell, iniciar el daemon de monitoreo, etc.
            printf("Ejecutando tareas seguras...\n");
            
            // 6. Cerrar Sesión (Desmontar directorios, limpiar entorno)
            pam_close_session(pamh, 0);
            printf("--- SESION CERRADA ---\n");
        }
        
        // Eliminar credenciales
        pam_setcred(pamh, PAM_DELETE_CRED);
    } else if (rc > 0) {
        printf("Autenticación fallida. Código PAM: %d\n", rc);
    } else {
        printf("Error local durante la autenticación.\n");
    }

    // 7. Limpiar e informar a PAM el estado final
    pam_end(pamh, rc);
    free(pwdup);
    return rc;
}