#version 150 core
const int Lcount = 2;
uniform vec4 Lpos[Lcount];
uniform vec3 Lamb[Lcount]; 
uniform vec3 Ldiff[Lcount]; 
uniform vec3 Lspec[Lcount];
uniform sampler2D texSampler1;

layout (std140) uniform Material
{
	vec3 Kamb; 
	vec3 Kdiff; 
	vec3 Kspec; 
	float Kshi; 

};


in vec4 P;
in vec3 N;
in vec2 TexCoord;
out vec4 fragment;


void main()
{
   vec3 V = -normalize(P.xyz);
   vec3 Idiff = vec3(0.0); 
   vec3 Ispec = vec3(0.0);
   for(int i = 0; i < Lcount; ++i)
   {
		vec3 L = normalize((Lpos[i] * P.w - P * Lpos[i].w).xyz);
		vec3 Iamb = Kamb * Lamb[i];
		Idiff += max(dot(N, L), 0.0) * Kdiff * Ldiff[i] + Iamb;
		vec3 H = normalize(L + V);
		Ispec += pow(max(dot(normalize(N), H), 0.0), Kshi) * Kspec * Lspec[i];
   }
   vec4 texture_color = texture(texSampler1, TexCoord);
   fragment = vec4(Idiff + Ispec * texture_color.rgb, 1.0);

}
