#version 150 core
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_NV_shader_buffer_load : enable

#define MILLIMETER 0.001
#define DEPTH_SCALE (-65535.0 * MILLIMETER)
#define DEPTH_MAXIMUM (-10.0)

#define KERNEL_SIZE 5

// �X�P�[��
const vec2 scale = vec2(
  1.546592,
  1.222434
);

// �e�N�X�`��
layout (location = 0) uniform sampler2D depth;

// �e�N�X�`�����W
in vec2 texcoord;

// �t���[���o�b�t�@�ɏo�͂���f�[�^
layout (location = 0) out vec3 position;

// 臒l
const float threshold = 0.1 * MILLIMETER;

//���U -- �t�B���^�̋��x�̎w��炵��
const float variance = 0.05;

// 臒l����
vec2 f(const in float z)
{
  return vec2(z, 1.0) * step(threshold, z);
}

// �d�ݕt����f�l�̍��v�Əd�݂̍��v�����߂�
void f2(inout float csum, inout float wsum, const in float base, const in float c, const in float w)
{
  float d = c - base;
  float e = exp(-0.5 * d * d / variance) * w;
  csum += c * e;
  wsum += e;
}


//�J�[�l�����̕��U�����߂�
vec3 Dispersion(){
  
  //�J�[�l�����̕��ϒl
  vec2 base =  f(texture(depth, texcoord + vec2(-2, -2)).r) + f(texture(depth, texcoord + vec2(-1, -2)).r) + f(texture(depth, texcoord + vec2( 0, -2)).r) + f(texture(depth, texcoord + vec2( 1, -2)).r) + f(texture(depth, texcoord + vec2( 2, -2)).r) + 
               f(texture(depth, texcoord + vec2(-2, -1)).r) + f(texture(depth, texcoord + vec2(-1, -1)).r) + f(texture(depth, texcoord + vec2( 0, -1)).r) + f(texture(depth, texcoord + vec2( 1, -1)).r) + f(texture(depth, texcoord + vec2( 2, -1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  0)).r) + f(texture(depth, texcoord + vec2(-1,  0)).r) + f(texture(depth, texcoord + vec2( 0,  0)).r) + f(texture(depth, texcoord + vec2( 1,  0)).r) + f(texture(depth, texcoord + vec2( 2,  0)).r) +
			   f(texture(depth, texcoord + vec2(-2,  1)).r) + f(texture(depth, texcoord + vec2(-1,  1)).r) + f(texture(depth, texcoord + vec2( 0,  1)).r) + f(texture(depth, texcoord + vec2( 1,  1)).r) + f(texture(depth, texcoord + vec2( 2,  1)).r) + 
			   f(texture(depth, texcoord + vec2(-2,  2)).r) + f(texture(depth, texcoord + vec2(-1,  2)).r) + f(texture(depth, texcoord + vec2( 0,  2)).r) + f(texture(depth, texcoord + vec2( 1,  2)).r) + f(texture(depth, texcoord + vec2( 2,  2)).r);
  float avg = base.r / base.g;

  //���U
  float result = texture(depth, texcoord + vec2( -2, -2)).r*texture(depth, texcoord + vec2( -2, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2( -1, -2)).r*texture(depth, texcoord + vec2( -1, -2)).r + texture(depth, texcoord + vec2(  0, -2)).r*texture(depth, texcoord + vec2(  0, -2)).r + texture(depth, texcoord + vec2( 1, -2)).r*texture(depth, texcoord + vec2( 1, -2)).r + texture(depth, texcoord + vec2( 2, -2)).r*texture(depth, texcoord + vec2( 2, -2)).r + 
				 texture(depth, texcoord + vec2( -2, -1)).r*texture(depth, texcoord + vec2( -2, -1)).r + texture(depth, texcoord + vec2( -1, -1)).r*texture(depth, texcoord + vec2( -1, -1)).r + texture(depth, texcoord + vec2(  0, -1)).r*texture(depth, texcoord + vec2(  0, -1)).r + texture(depth, texcoord + vec2( 1, -1)).r*texture(depth, texcoord + vec2( 1, -1)).r + texture(depth, texcoord + vec2( 2, -1)).r*texture(depth, texcoord + vec2( 2, -1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  0)).r*texture(depth, texcoord + vec2( -2,  0)).r + texture(depth, texcoord + vec2( -1,  0)).r*texture(depth, texcoord + vec2( -1,  0)).r + texture(depth, texcoord + vec2(  0,  0)).r*texture(depth, texcoord + vec2(  0,  0)).r + texture(depth, texcoord + vec2( 1,  0)).r*texture(depth, texcoord + vec2( 1,  0)).r + texture(depth, texcoord + vec2( 2,  0)).r*texture(depth, texcoord + vec2( 2,  0)).r +
				 texture(depth, texcoord + vec2( -2,  1)).r*texture(depth, texcoord + vec2( -2,  1)).r + texture(depth, texcoord + vec2( -1,  1)).r*texture(depth, texcoord + vec2( -1,  1)).r + texture(depth, texcoord + vec2(  0,  1)).r*texture(depth, texcoord + vec2(  0,  1)).r + texture(depth, texcoord + vec2( 1,  1)).r*texture(depth, texcoord + vec2( 1,  1)).r + texture(depth, texcoord + vec2( 2,  1)).r*texture(depth, texcoord + vec2( 2,  1)).r + 
			 	 texture(depth, texcoord + vec2( -2,  2)).r*texture(depth, texcoord + vec2( -2,  2)).r + texture(depth, texcoord + vec2( -1,  2)).r*texture(depth, texcoord + vec2( -1,  2)).r + texture(depth, texcoord + vec2(  0,  2)).r*texture(depth, texcoord + vec2(  0,  2)).r + texture(depth, texcoord + vec2( 1,  2)).r*texture(depth, texcoord + vec2( 1,  2)).r + texture(depth, texcoord + vec2( 2,  2)).r*texture(depth, texcoord + vec2( 2,  2)).r;
  
  exp(KERNEL_SIZE*KERNEL_SIZE / result);

  return vec3(result, avg, base.g);

}


// �d�ݕt�����ς����߂�
void main()
{
  vec3 stat = Dispersion();

  //float z = csum / wsum* DEPTH_SCALE;// > 0 ? csum / wsum * DEPTH_SCALE : DEPTH_MAXIMUM;
  //z = z < 10 ? DEPTH_MAXIMUM : z;
  // �f�v�X�l����J�������W�l�����߂�
 // position = vec3(-(texcoord - 0.5) * scale * z, z);
  position = vec3( -texcoord ,stat.g);

}