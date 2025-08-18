#version 150 core
uniform mat4 modelview;
uniform mat4 projection;

in vec2 vertex_position;
in vec3 instance_position;
in vec4 instance_color;
in float instance_size;
in float instance_rotation;
out vec2 texCoord;
out vec4 particle_color;

void main(){
    texCoord = vertex_position + 0.5;
    // 回転行列を作成
    float cosR = cos(instance_rotation);
    float sinR = sin(instance_rotation);
    vec2 rotated_position = vec2(vertex_position.x * cosR - vertex_position.y * sinR,
                                 vertex_position.x * sinR + vertex_position.y * cosR );
    
    // ビルボード効果
    vec4 view_position = modelview * vec4(instance_position, 1.0);
    view_position.xy += rotated_position * instance_size;
    
    gl_Position = projection * view_position;
    
    particle_color = instance_color;
}
