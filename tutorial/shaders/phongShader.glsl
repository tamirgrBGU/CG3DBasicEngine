#version 330

 uniform mat4 View;
  uniform mat4 Proj;
  //uniform vec4 fixed_color;
  in vec3 position_eye;
  in vec3 normal_eye;
  uniform vec4 light_position_eye;
  uniform vec4 lightColor;
  in vec4 Ksi;
  in vec4 Kdi;
  in vec4 Kai;
  in vec2 texcoordi;
  uniform sampler2D sampler1;
  uniform float specular_exponent;
  //uniform float lighting_factor;
  //uniform float texture_factor;
  //out vec4 outColor;
  void main()
  {
    vec3 Ls = vec3(lightColor);
    vec3 Ld = vec3(lightColor);
    vec3 La = vec3(lightColor);
    vec3 Ia = La * vec3(Kai);    // ambient intensity

    vec3 vector_to_light_eye = vec3(light_position_eye) - position_eye;
    vec3 direction_to_light_eye = normalize (vector_to_light_eye);
    float dot_prod = dot (direction_to_light_eye, normalize(normal_eye));
    float clamped_dot_prod = max (dot_prod, 0.0);
    vec3 Id = Ld * vec3(Kdi) * clamped_dot_prod;    // Diffuse intensity

    vec3 reflection_eye = reflect (-direction_to_light_eye, normalize(normal_eye));
    vec3 surface_to_viewer_eye = normalize (-position_eye);
    float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
    dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);
    float specular_factor = pow (dot_prod_specular, specular_exponent);
    vec3 Is = Ls * vec3(Ksi) * specular_factor;    // specular intensity
    gl_FragColor = vec4((Is + Id) ,0.5);
     
    //= mix(vec4(1,1,1,1), texture(sampler1, texcoordi), texture_factor) * color;
    //if (fixed_color != vec4(0.0)) outColor = fixed_color;
  }