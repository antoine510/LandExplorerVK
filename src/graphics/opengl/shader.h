#ifndef DEF_SHADER
#define DEF_SHADER

#include "oglCommon.h"

#define MAX_SHADER_SIZE 65535
#define MAX_SHADER_FILENAME_SIZE 255

typedef enum ShaderType {shader_base, shader_chunck} ShaderType;

// Classe Shader

typedef struct Shader
{
    char vertexSource[MAX_SHADER_FILENAME_SIZE];
    char fragmentSource[MAX_SHADER_FILENAME_SIZE];

    GLuint vertexID;
    GLuint fragmentID;
    GLuint programID;

    void* uniforms;
    void (*setUniforms)(void*);
} Shader;

Shader* createShader();
Shader* createShaderFromSource(const char* vertexSource, const char* fragmentSource);
Shader* copyShader(const Shader* src);
void destroyShader(Shader* shader);

void setUniformsData(Shader* shader, void* uniforms, void (*setUniforms)(void*));
void applyShaderUniforms(Shader* shader);
bool loadShader(Shader* shader);
bool compileShader(GLuint* shader, GLenum type, const char* fichierSource);

#endif
