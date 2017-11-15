#pragma once

#include <string>
#include <vector>

#include "opengl.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace sgl
{

struct Shader
{
  GLuint id;

  Shader( const std::string& shader_code, GLenum shader_type )
  {
    compile( shader_code, shader_type );
  }

  void
  compile( const std::string& shader_code, GLenum shader_type )
  {
    id = glCreateShader( shader_type );
    if( id == 0)
      throw std::runtime_error("glCreateShader failed");

    const char* code = shader_code.c_str();
    glShaderSource( id, 1, (const GLchar**)&code, NULL);
    glCompileShader( id );
    
    GLint status;
    glGetShaderiv( id, GL_COMPILE_STATUS, &status );
    if( status == GL_FALSE ) 
    {
      std::string msg("Compile failure in shader:\n");
          
      GLint infoLogLength;
      glGetShaderiv( id, GL_INFO_LOG_LENGTH, &infoLogLength );
      
      char* strInfoLog = new char[ infoLogLength + 1 ];
      glGetShaderInfoLog( id, infoLogLength, NULL, strInfoLog );
      msg += strInfoLog;
      delete[] strInfoLog;
          
      glDeleteShader( id );
      throw std::runtime_error(msg);
    }
  }
};


struct ShaderProgram
{
  GLuint id;

  void
  create( std::vector<Shader> shaders )
  {
    id = glCreateProgram();
    if( id == 0)
      throw std::runtime_error( "glCreateProgram failed" );

    for( auto shader : shaders )
      glAttachShader( id, shader.id );
    
    glLinkProgram( id );

    for( auto shader : shaders )
      glDetachShader( id, shader.id );

    GLint status;
    glGetProgramiv( id, GL_LINK_STATUS, &status);
    if( status == GL_FALSE ) 
    {
      std::string msg("Program linking failure: ");
          
      GLint infoLogLength;
      glGetProgramiv( id, GL_INFO_LOG_LENGTH, &infoLogLength);
      char* strInfoLog = new char[infoLogLength + 1];
      glGetProgramInfoLog( id, infoLogLength, NULL, strInfoLog);
      msg += strInfoLog;
      delete[] strInfoLog;
          
      glDeleteProgram( id );
      throw std::runtime_error(msg);
    }    
  }

  // void
  // use() const
  // {
  //   glUseProgram( id );
  // }

  // void
  // stop() const
  // {
  //   glUseProgram( 0 );
  // }

  GLint 
  get_attrib_loc( const GLchar* name ) const
  {
    GLint attrib = glGetAttribLocation( id, name );
    if( attrib == -1 )
    {
      throw std::runtime_error( name + std::string(": no such program attribute") );
    }
    return attrib;    
  }

  GLint
  get_unif_loc( const GLchar* name ) const
  {
    GLint uniform = glGetUniformLocation( id, name );
    if( uniform == -1 )
    {
      throw std::runtime_error( name + std::string(": no such uniform location") );
    }
    return uniform;    
  }

  void 
  set_camera( const glm::mat4& m ) 
  {
    glUniformMatrix4fv( get_unif_loc( "camera" ), 1, GL_FALSE, glm::value_ptr( m ) );
  }
};

} // namespace sgl
