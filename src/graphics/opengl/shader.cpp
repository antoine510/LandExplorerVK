#include "shader.h"

Shader* createShader()
{
    Shader* shader = malloc(sizeof(Shader));
    shader->vertexID = 0;
    shader->fragmentID = 0;
    shader->programID = 0;

    return shader;
}

Shader* createShaderFromSource(const char* vertexSource, const char* fragmentSource)
{
    Shader* shader = createShader();

    strncpy(shader->vertexSource, vertexSource, MAX_SHADER_FILENAME_SIZE);
    strncpy(shader->fragmentSource, fragmentSource, MAX_SHADER_FILENAME_SIZE);

    return shader;
}

Shader* copyShader(const Shader* src)
{
    // Copie des fichiers sources
    Shader* shader = createShader();

    memcpy(shader->vertexSource, src->vertexSource, sizeof(Shader));

    loadShader(shader);
    return shader;
}

void destroyShader(Shader* shader)
{
    // Destruction du shader

    glDeleteShader(shader->vertexID);
    glDeleteShader(shader->fragmentID);
    glDeleteProgram(shader->programID);

    free(shader);
}

void setUniformsData(Shader* shader, void* uniforms, void (*setUniforms)(void*))
{
    shader->uniforms = uniforms;
    shader->setUniforms = setUniforms;
}

void applyShaderUniforms(Shader* shader)
{
    shader->setUniforms(shader->uniforms);
}

bool loadShader(Shader* shader)
{
    // Destruction d'un éventuel ancien Shader

    if(glIsShader(shader->vertexID) == GL_TRUE)
        glDeleteShader(shader->vertexID);

    if(glIsShader(shader->fragmentID) == GL_TRUE)
        glDeleteShader(shader->fragmentID);

    if(glIsProgram(shader->programID) == GL_TRUE)
        glDeleteProgram(shader->programID);


    // Compilation des shaders

    if(!compileShader(&shader->vertexID, GL_VERTEX_SHADER, shader->vertexSource)) return false;
    if(!compileShader(&shader->fragmentID, GL_FRAGMENT_SHADER, shader->fragmentSource)) return false;


    // Création du programme

    shader->programID = glCreateProgram();


    // Association des shaders

    glAttachShader(shader->programID, shader->vertexID);
    glAttachShader(shader->programID, shader->fragmentID);


    // Verrouillage des entrées shader

    glBindAttribLocation(shader->programID, 0, "in_Vertex");
    glBindAttribLocation(shader->programID, 1, "in_Color");
    glBindAttribLocation(shader->programID, 2, "in_TexCoord0");


    // Linkage du programme

    glLinkProgram(shader->programID);


    // Vérification du linkage

    GLint erreurLink = 0;
    glGetProgramiv(shader->programID, GL_LINK_STATUS, &erreurLink);


    // S'il y a eu une erreur

    if(erreurLink != GL_TRUE)
    {
        // Récupération de la taille de l'erreur

        GLint tailleErreur = 0;
        glGetProgramiv(shader->programID, GL_INFO_LOG_LENGTH, &tailleErreur);


        // Allocation de mémoire

        char *erreur = malloc((tailleErreur + 1) * sizeof(char));


        // Récupération de l'erreur

        glGetShaderInfoLog(shader->programID, tailleErreur, &tailleErreur, erreur);
        erreur[tailleErreur] = '\0';


        // Affichage de l'erreur

        printf("Erreur de link : %s", erreur);


        // Libération de la mémoire et retour du booléen false

        free(erreur);
        glDeleteProgram(shader->programID);

        return false;
    }

    return true;
}

bool compileShader(GLuint* shader, GLenum type, const char* fichierSource)
{
    // Création du shader

    *shader = glCreateShader(type);


    // Vérification du shader

    if(*shader == 0)
    {
        printf("Erreur, le type de shader (%d) n'existe pas\n", type);
        return false;
    }


    // Flux de lecture

    FILE* fichier = fopen(fichierSource, "r");
    // Test d'ouverture

    if(!fichier)
    {
        printf("Erreur le fichier %s est introuvable\n", fichierSource);
        glDeleteShader(*shader);

        return false;
    }


    // Strings permettant de lire le code source
    GLchar* codeSource = malloc(MAX_SHADER_SIZE * sizeof(GLchar));
    int length = 0;

    // Lecture
    while(!feof(fichier))
    {
        codeSource[length] = fgetc(fichier);
        length++;
    }
    codeSource[length-1] = '\0';

    // Fermeture du fichier

    fclose(fichier);

    // Envoi du code source au shader
    glShaderSource(*shader, 1, (const GLchar**)(&codeSource), 0);

    free(codeSource);

    // Compilation du shader

    glCompileShader(*shader);


    // Vérification de la compilation

    GLint erreurCompilation = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &erreurCompilation);


    // S'il y a eu une erreur

    if(erreurCompilation != GL_TRUE)
    {
        // Récupération de la taille de l'erreur

        GLint tailleErreur = 0;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &tailleErreur);


        // Allocation de mémoire

        char *erreur = malloc((tailleErreur + 1) * sizeof(char));


        // Récupération de l'erreur

        glGetShaderInfoLog(*shader, tailleErreur, &tailleErreur, erreur);
        erreur[tailleErreur] = '\0';


        // Affichage de l'erreur

        printf("Error : compilation failed :%s\n\nLength : %d\n", erreur, length);


        // Libération de la mémoire et retour du booléen false

        free(erreur);
        glDeleteShader(*shader);

        return false;
    }
    else
        return true;
}
