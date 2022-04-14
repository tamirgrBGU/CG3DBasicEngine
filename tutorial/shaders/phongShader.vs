#version 330

  uniform mat4 Proj;
  uniform mat4 View;
  uniform mat4 Model;
  in vec3 position;
  in vec3 normal;
  in vec3 color;
  in vec2 texcoords;

  out vec3 position_eye;
  out vec3 normal_eye;
  out vec2 texcoordi;
  out vec4 Kai;
  out vec4 Kdi;
  out vec4 Ksi;

  void main()
  {
    position_eye = vec3 (View * Model * vec4 (position, 1.0));
    normal_eye = vec3 (Model * vec4 (normal, 0.0));
    normal_eye = normalize(normal_eye);
    Kai = vec4(color,1);
    Kdi = vec4(color,1);
    Ksi = vec4(0.7,0.7,0.7,1);
    texcoordi = texcoords;
    gl_Position = Proj * vec4 (position_eye, 1.0); //Proj * View * vec4(position, 1.0);"
   
  }