#if defined(HAVE_GLES2)

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gles/hessian_detector.h>

namespace baldzarika { namespace ucv { namespace gles {

	namespace {
		static char const *vs_hessian_response_src=
			"attribute vec4 a_vtx_pos;\n"
			"attribute vec4 a_vtx_tex_coord;\n"
			"varying vec4 v_vtx_tex_coord;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position=a_vtx_pos;\n"
			"	v_vtx_tex_coord=a_vtx_tex_coord;\n"
			"}\n"
		;

		static char const *fs_hessian_response_src=
			"precision mediump float;\n"
			"uniform vec2 u_integral_img_span;\n"
			"uniform ivec2 u_integral_tex_size;\n"
			"uniform int u_layer_filter_size;\n"
			"uniform sampler2D u_integral_img;\n"
			"varying vec4 v_vtx_tex_coord;\n"
			"vec2 g_inv_integral_tex_size=vec2(1.0/float(u_integral_tex_size.x), 1.0/float(u_integral_tex_size.y));\n"
			"float g_inv_layer_filter_area=1.0/float(u_layer_filter_size*u_layer_filter_size);\n"
			"float g_b=float((u_layer_filter_size-1)/2+1);\n"
			"float g_l=float(u_layer_filter_size/3);\n"
			"float g_w=float(u_layer_filter_size);\n"
			"vec2 g_o1=vec2(-g_b,-g_l+1.0);\n"
			"vec2 g_s1=vec2(g_w, 2.0*g_l-1.0);\n"
			"vec2 g_o2=vec2(-0.5*g_l,-g_l+1.0);\n"
			"vec2 g_s2=vec2(g_l, 2.0*g_l-1.0);\n"
			"vec2 g_fs1=vec2(g_l, g_l)*g_inv_integral_tex_size;\n"
			"vec2 g_o11=g_o1*g_inv_integral_tex_size;\n"
			"vec2 g_o21=g_o2*g_inv_integral_tex_size;\n"
			"vec2 g_o12=g_o1.yx*g_inv_integral_tex_size;\n"
			"vec2 g_o22=g_o2.yx*g_inv_integral_tex_size;\n"
			"vec2 g_s11=g_s1*g_inv_integral_tex_size;\n"
			"vec2 g_s21=g_s2*g_inv_integral_tex_size;\n"
			"vec2 g_s12=g_s1.yx*g_inv_integral_tex_size;\n"
			"vec2 g_s22=g_s2.yx*g_inv_integral_tex_size;\n"
			"vec2 g_o31=vec2(1.0, -g_l)*g_inv_integral_tex_size;\n"
			"vec2 g_o32=vec2(-g_l, 1.0)*g_inv_integral_tex_size;\n"
			"vec2 g_o33=vec2(-g_l, -g_l)*g_inv_integral_tex_size;\n"
			"vec2 g_o34=g_inv_integral_tex_size;\n"
			"\n"
			"float unpack_int_sum_from_rgba(in vec4 rgba)\n"
			"{\n"
			"	const vec4 rgb2sum_scale=vec4(255.0/256.0, 255.0, 255.0*256.0, 255.0*256.0*256.0);\n"
			"	return dot(rgba, rgb2sum_scale);\n"
			"}\n"
			"\n"
			"float box_integral(in vec2 pt, in vec2 sz)\n"
			"{\n"
			"	const vec4 box_integral_helper=vec4(1.0, -1.0, -1.0, 1.0);\n"
			"	float min_x=clamp(pt.x,0.0,u_integral_img_span.x);\n"
			"	float max_x=clamp(pt.x+sz.x,0.0,u_integral_img_span.x);\n"
			"	float min_y=clamp(pt.y,0.0,u_integral_img_span.y);\n"
			"	float max_y=clamp(pt.y+sz.y,0.0,u_integral_img_span.y);\n"
			"	float f_area=sz.x*sz.y;\n"
			"	float r_area=(max_x-min_x)*(max_y-min_y);\n"
			"	vec4 ABCD=vec4(\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(min_x, min_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(max_x, min_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(min_x, max_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(max_x, max_y)))\n"
			"	);\n"
			"	return max(0.0, dot( ABCD, box_integral_helper)*(2.0-abs(r_area/f_area)));"
			"}\n"
			"\n"
			"vec4 pack_float_to_vec4(in float r)\n"
			"{\n"
			"	const vec3 bit_shift=vec3(256.0, 1.0, 1.0/256.0);\n"
			"	const vec3 bit_mask=vec3(0.0, 1.0/256.0, 1.0/256.0);\n"
			"	const float inv_255=1.0/255.0;\n"
			"	const float inv_log10=1.0/log(10.0)\n;"
			"	float r_abs=abs(r);\n"
			"	int r_log10=int(clamp(ceil(log(r_abs)*inv_log10), -64.0, 63.0));\n"
			"	float r_mantisa=0.5+0.5*r*pow(10.0, -float(r_log10));\n"
			"	vec3 res=fract((256.0*r_mantisa)*bit_shift);\n"
			"	res-=res.xxy*bit_mask;\n"
			"	return vec4(res, inv_255*float(128+2*r_log10));\n"
			"}\n"
			"void main()\n"
			"{\n"
			"	vec4 derivs=vec4(0.0);\n"
			"	derivs.x=\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o11, g_s11)-\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o21, g_s21)*3.0;\n"
			"	derivs.y=\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o12, g_s12)-\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o22, g_s22)*3.0;\n"
			"	derivs.z=\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o31, g_fs1)+\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o32, g_fs1)-\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o33, g_fs1)-\n"
			"		box_integral(v_vtx_tex_coord.xy+g_o34, g_fs1);\n"
			"	derivs=derivs*g_inv_layer_filter_area;\n"
			"	gl_FragColor=pack_float_to_vec4(derivs.x*derivs.y-0.81*derivs.z*derivs.z);\n"
			//"	gl_FragColor=vec4(0.5+5.0*(derivs.x*derivs.y-0.81*derivs.z*derivs.z));\n"
			"}\n"
		;

		static char const *vs_feature_detection_src=
			"precision mediump float;\n"
			"attribute vec4 a_vtx_pos;\n"
			"attribute vec4 a_vtx_tex_coord;\n"
			"varying vec4 v_vtx_tex_coord;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position=a_vtx_pos;\n"
			"	v_vtx_tex_coord=a_vtx_tex_coord;\n"
			"}\n"
		;

		static char const *fs_feature_detection_src=
			"precision mediump float;\n"
			"uniform float u_resp_trshld;\n"
			"uniform ivec2 u_resp_tex_sz;\n"
			"uniform vec2 u_top_resp_off;\n"
			"uniform vec2 u_top_resp_sz;\n"
			"uniform vec2 u_mid_resp_off;\n"
			"uniform vec2 u_mid_resp_sz;\n"
			"uniform vec2 u_btm_resp_off;\n"
			"uniform vec2 u_btm_resp_sz;\n"
			"uniform int u_top_step;\n"
			"uniform int u_top_filter_size;\n"
			"uniform ivec2 u_top_det_sz;\n"
			"uniform sampler2D u_response_layer;\n"
			"varying vec4 v_vtx_tex_coord;\n"
			"vec2 g_inv_top_resp_sz=vec2(1.0)/u_top_resp_sz;\n"
			"vec2 g_inv_resp_tex_size=vec2(1.0)/vec2(float(u_resp_tex_sz.x), float(u_resp_tex_sz.y));\n"
			"vec3 g_inc_dec=vec3(g_inv_resp_tex_size.x, g_inv_resp_tex_size.y, 0.0);\n"
			"vec2 g_inc_dec_diag=vec2(-g_inv_resp_tex_size.x, g_inv_resp_tex_size.y);\n"
			"int top_layer_border_sz=(u_top_filter_size+1)/(2*u_top_step);\n"
			"vec2 top_layer_border=vec2(float(top_layer_border_sz))/vec2(u_top_det_sz);\n"
			"vec2 top_layer_border_min=vec2(-1.0,-1.0)+top_layer_border;\n"
			"vec2 top_layer_border_max=vec2(1.0,1.0)-top_layer_border;\n"
			"\n"
			"float unpack_float_from_vec4(in vec4 r)\n"
			"{\n"
			"	const vec3 bit_shift=vec3(1.0/256.0, 1.0, 256.0);\n"
			"	const float inv_256=1.0/256.0;\n"
			"	float r_mantisa=2.0*(dot(inv_256*r.xyz, bit_shift)-0.5);\n"
			"	int r_exp=(int(r.w*255.0)-128)/2;\n"
			"	return r_mantisa*pow(10.0, float(r_exp));\n"
			"}\n"
			"\n"
			"float get_middle_response(in vec2 a_loc)\n"
			"{\n"
			"	vec2 norm_loc=(a_loc-u_top_resp_off)*g_inv_top_resp_sz;\n"
			"	return unpack_float_from_vec4(texture2D(u_response_layer, norm_loc*u_mid_resp_sz+u_mid_resp_off));\n"
			"}\n"
			"\n"
			"float get_bottom_response(in vec2 a_loc)\n"
			"{\n"
			"	vec2 norm_loc=(a_loc-u_top_resp_off)*g_inv_top_resp_sz;\n"
			"	return unpack_float_from_vec4(texture2D(u_response_layer, norm_loc*u_btm_resp_sz+u_btm_resp_off));\n"
			"}\n"
			"float get_response(in vec2 a_loc)\n"
			"{\n"
			"	return unpack_float_from_vec4(texture2D(u_response_layer, a_loc));\n"
			"}\n"
			"\n"
			"bool is_extremum()\n"
			"{\n"
			"	if(	all(greaterThan(gl_FragCoord.xy,top_layer_border_min)) && \n"
			"		all(lessThan(gl_FragCoord.xy,top_layer_border_max)) ) return false;\n"
			"	float candidate=get_middle_response(v_vtx_tex_coord.xy);\n"
			"	if(candidate<u_resp_trshld) return false;\n"
			"	for(int y=-1;y<=1;++y)\n"
			"		for(int x=-1;x<=1;++x)\n"
			"		{\n"
			"			vec2 i_loc=v_vtx_tex_coord.xy+vec2(float(x),float(y))*g_inv_resp_tex_size;\n"
			"			if(	get_response(i_loc)>=candidate ||\n"
			"				((x!=0 || y!=0) && get_middle_response(i_loc)>=candidate) ||\n"
			"				get_bottom_response(i_loc)>=candidate\n"
			"			)\n"
			"			return false;\n"
			"		}\n"
			"	return true;\n"
			"}\n"
			"vec3 deriv_3d()\n"
			"{\n"
			"	return 0.5*vec3(\n"
			"		get_middle_response(v_vtx_tex_coord.xy+g_inc_dec.xz)-get_middle_response(v_vtx_tex_coord.xy-g_inc_dec.xz),\n"
			"		get_middle_response(v_vtx_tex_coord.xy+g_inc_dec.zy)-get_middle_response(v_vtx_tex_coord.xy-g_inc_dec.zy),\n"
			"		get_response(v_vtx_tex_coord.xy)-get_bottom_response(v_vtx_tex_coord.xy)\n"
			"	);\n"
			"}\n"
			"\n"
			"mat3 hessian_3d()\n"
			"{\n"
			"	float v=get_middle_response(v_vtx_tex_coord.xy);\n"
			"	float dxx=-2.0*v+\n"
			"		get_middle_response(v_vtx_tex_coord.xy+g_inc_dec.xz)+\n"
			"		get_middle_response(v_vtx_tex_coord.xy-g_inc_dec.xz);\n"
			"	float dyy=-2.0*v+\n"
			"		get_middle_response(v_vtx_tex_coord.xy+g_inc_dec.zy)+\n"
			"		get_middle_response(v_vtx_tex_coord.xy-g_inc_dec.zy);\n"
			"	float dss=-2.0*v+\n"
			"		get_response(v_vtx_tex_coord.xy)+get_bottom_response(v_vtx_tex_coord.xy);\n"
			"	float dxy=0.25*(\n"
			"		get_middle_response(v_vtx_tex_coord.xy+g_inc_dec.xy)-get_middle_response(v_vtx_tex_coord.xy+g_inc_dec_diag)-\n"
			"		-get_middle_response(v_vtx_tex_coord.xy-g_inc_dec_diag)+get_middle_response(v_vtx_tex_coord.xy-g_inc_dec.xy));\n"
			"	float dxs=0.25*("
			"		get_response(v_vtx_tex_coord.xy+g_inc_dec.xz)-get_response(v_vtx_tex_coord.xy-g_inc_dec.xz)-\n"
			"		get_bottom_response(v_vtx_tex_coord.xy+g_inc_dec.xz)+get_bottom_response(v_vtx_tex_coord.xy-g_inc_dec.xz));\n"
			"	float dys=0.25*("
			"		get_response(v_vtx_tex_coord.xy+g_inc_dec.zy)-get_response(v_vtx_tex_coord.xy-g_inc_dec.zy)-\n"
			"		get_bottom_response(v_vtx_tex_coord.xy+g_inc_dec.zy)+get_bottom_response(v_vtx_tex_coord.xy-g_inc_dec.zy));\n"
			"	return mat3(\n"
			"		dxx, dxy, dxs,\n"
			"		dxy, dyy, dys,\n"
			"		dxs, dys, dss\n"
			"	);\n"
			"}\n"
			"vec3 interpolate_step()\n"
			"{\n"
			"	vec3 dD=deriv_3d();\n"
			"	mat3 H=hessian_3d();\n"
			"	float invdet=1.0/(\n"
			"		H[0][0]*H[1][1]*H[2][2]+\n"
			"		H[1][0]*H[2][1]*H[0][2]+\n"
			"		H[2][0]*H[0][1]*H[1][2]-\n"
			"		H[0][0]*H[2][1]*H[1][2]-\n"
			"		H[1][0]*H[0][1]*H[2][2]-\n"
			"		H[2][0]*H[1][1]*H[0][2]\n"
			"	);\n"
			"	mat3 Hinv=invdet*mat3(\n"
			"		 (H[1][1]*H[2][2]-H[2][1]*H[1][2]),	-(H[0][1]*H[2][2]-H[2][1]*H[0][2]),	 (H[0][1]*H[1][2]-H[1][1]*H[0][2]),\n"
			"		-(H[1][0]*H[2][2]-H[2][0]*H[1][2]),  (H[0][0]*H[2][2]-H[2][0]*H[0][2]),	-(H[0][0]*H[1][2]-H[1][0]*H[0][2]),\n"
			"		 (H[1][0]*H[2][1]-H[2][0]*H[1][1]), -(H[0][0]*H[2][1]-H[2][0]*H[0][1]),  (H[0][0]*H[1][1]-H[1][0]*H[0][1])\n"
			"	);\n"
			"	return -(Hinv*dD);\n"
			"}\n"
			"\n"
			"vec4 pack_interpolation(in vec3 xys)\n"
			"{\n"
			"	vec3 xys_offset=xys+vec3(0.5);\n"
			"	float x_low=fract(xys_offset.x*16.0);\n"
			"	float x_hi=(xys_offset.x-x_low/16.0)/16.0;\n"
			"	float y_low=fract(xys_offset.y*16.0);\n"
			"	float y_hi=(xys_offset.y-y_low/16.0)/16.0;\n"
			"	return vec4(x_low, y_low, x_hi+16.0*y_hi, xys_offset.z);\n"
			"}\n"
			"\n"
			"void main()\n"
			"{\n"
			"	vec4 frag_color=vec4(0.0);\n"
			"	if(is_extremum())\n"
			"	{\n"
			"		vec3 x=interpolate_step();\n"
			"		if(all(lessThan(abs(x), vec3(0.5))))\n"
			"			frag_color=pack_interpolation(x);\n"
			"	}\n"
			"	gl_FragColor=frag_color;\n"
			"}\n"
		;

		static char const *vs_orientation_1_src=
			"precision mediump float;\n"
			"attribute vec4 a_vtx_pos;\n"
			"attribute vec4 a_vtx_fpt;\n"
			"varying vec4 v_vtx_fpt_coord;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position=a_vtx_pos;\n"
			"	v_vtx_fpt_coord=a_vtx_fpt;\n"
			"}\n"
		;

		const boost::uint32_t ORI_GAUSS25_SIZE=7*7;
		const float ori_gauss_25[ORI_GAUSS25_SIZE]=
		{
			0.02350693969273f,	0.01849121369071f,	0.01239503121241f,	0.00708015417522f,	0.00344628101733f,	0.00142945847484f,	0.00050524879060f,
			0.02169964028389f,	0.01706954162243f,	0.01144205592615f,	0.00653580605408f,	0.00318131834134f,	0.00131955648461f,	0.00046640341759f,
			0.01706954162243f,	0.01342737701584f,	0.00900063997939f,	0.00514124713667f,	0.00250251364222f,	0.00103799989504f,	0.00036688592278f,
			0.01144205592615f,	0.00900063997939f,	0.00603330940534f,	0.00344628101733f,	0.00167748505986f,	0.00069579213743f,	0.00024593098864f,
			0.00653580605408f,	0.00514124713667f,	0.00344628101733f,	0.00196854695367f,	0.00095819467066f,	0.00039744277546f,	0.00014047800980f,
			0.00318131834134f,	0.00250251364222f,	0.00167748505986f,	0.00095819467066f,	0.00046640341759f,	0.00019345616757f,	0.00006837798818f,
			0.00131955648461f,	0.00103799989504f,	0.00069579213743f,	0.00039744277546f,	0.00019345616757f,	0.00008024231247f,	0.00002836202103f
		};


		const boost::uint32_t ORI_INDICES_SIZE=109;

		class ori_indices
		{
		private:
			ori_indices()
			{
				int idx=0;
				for(int i=-6;i<=6;++i)
				{
					for(int j=-6;j<=6;++j)
					{
						if(i*i+j*j<36)
						{
							m_values[2*idx+0]=i;
							m_values[2*idx+1]=j;
							idx++;
						}
					}
				}
			}

		public:
			static ori_indices const& get()
			{
				static ori_indices _ori_indices;
				return _ori_indices;
			}
		public:
			GLint	m_values[ORI_INDICES_SIZE*2];
		};


		static char const *fs_orientation_1_src=
			"precision mediump float;\n"
			"uniform vec2 u_integral_img_span;\n"
			"uniform ivec2 u_integral_tex_size;\n"
			"uniform sampler2D u_integral_img;\n"
			"uniform float u_gauss_25[49];\n"
			"uniform ivec2 u_indices[109];\n"
			"varying vec4 v_vtx_fpt_coord;\n"
			"vec2 g_inv_integral_tex_size=vec2(1.0/float(u_integral_tex_size.x), 1.0/float(u_integral_tex_size.y));\n"
			"const float pi=3.14159;\n"
			"const float pi_2=2.0*pi;\n"
			"const float pi_i3=pi/3.0;\n"
			"float unpack_int_sum_from_rgba(in vec4 rgba)\n"
			"{\n"
			"	const vec4 rgb2sum_scale=vec4(255.0/256.0, 255.0, 255.0*256.0, 255.0*256.0*256.0);\n"
			"	return dot(rgba, rgb2sum_scale);\n"
			"}\n"
			"\n"
			"float box_integral(in vec2 pt, in vec2 sz)\n"
			"{\n"
			"	const vec4 box_integral_helper=vec4(1.0, -1.0, -1.0, 1.0);\n"
			"	float min_x=clamp(pt.x,0.0,u_integral_img_span.x);\n"
			"	float max_x=clamp(pt.x+sz.x,0.0,u_integral_img_span.x);\n"
			"	float min_y=clamp(pt.y,0.0,u_integral_img_span.y);\n"
			"	float max_y=clamp(pt.y+sz.y,0.0,u_integral_img_span.y);\n"
			"	float f_area=sz.x*sz.y;\n"
			"	float r_area=(max_x-min_x)*(max_y-min_y);\n"
			"	vec4 ABCD=vec4(\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(min_x, min_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(max_x, min_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(min_x, max_y))),\n"
			"		unpack_int_sum_from_rgba(texture2D(u_integral_img, vec2(max_x, max_y)))\n"
			"	);\n"
			"	return max(0.0, dot( ABCD, box_integral_helper)*(2.0-abs(r_area/f_area)));"
			"}\n"
			"vec4 pack_res_to_vec4(in vec2 r)\n"
			"{\n"
			"	const vec2 bit_shift=vec2(256.0, 1.0);\n"
			"	const vec2 bit_mask=vec2(0.0, 1.0/256.0);\n"
			"	vec2 res_x=fract((0.1*r.x+0.5)*bit_shift);\n"
			"	res_x-=res_x.xx*bit_mask;\n"
			"	vec2 res_y=fract((0.1*r.y+0.5)*bit_shift);\n"
			"	res_y-=res_y.xx*bit_mask;\n"
			"	return vec4(res_x, res_y);\n"
			"}\n"
			"vec2 haar(in vec2 pt, in float s)\n"
			"{\n"
			"	return vec2(\n"
			"		box_integral(pt-0.5*vec2(0.0, s)*g_inv_integral_tex_size, vec2(0.5*s, s)*g_inv_integral_tex_size)-\n"
			"		box_integral(pt-0.5*vec2(s, s)*g_inv_integral_tex_size, vec2(0.5*s, s)*g_inv_integral_tex_size),\n"
			"		box_integral(pt-0.5*vec2(s, 0.0)*g_inv_integral_tex_size, vec2(s, 0.5*s)*g_inv_integral_tex_size)-\n"
			"		box_integral(pt-0.5*vec2(s, s)*g_inv_integral_tex_size, vec2(s, 0.5*s)*g_inv_integral_tex_size)\n"
			"	);\n"
			"}\n"
			"void main()\n"
			"{\n"
			"	float s=floor(v_vtx_fpt_coord.z+0.5);\n"
			"	int idx=int(gl_FragCoord.x);\n"
			"	ivec2 ij=u_indices[idx];\n"
			"	vec2 res=haar(v_vtx_fpt_coord.xy+s*vec2(float(ij.x),float(ij.y))*g_inv_integral_tex_size, 4.0*s);\n"
			"	gl_FragColor=pack_res_to_vec4(u_gauss_25[7*int(abs(float(ij.x)))+int(abs(float(ij.y)))]*res);\n"
			"}\n"
		;

		static char const *vs_orientation_2_src=
			"precision mediump float;\n"
			"attribute vec4 a_vtx_pos;\n"
			"attribute vec4 a_vtx_fpt;\n"
			"varying vec4 v_vtx_fpt_coord;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position=a_vtx_pos;\n"
			"	gl_PointSize=1.0;\n"
			"	v_vtx_fpt_coord=a_vtx_fpt;\n"
			"}\n"
		;

		static char const *fs_orientation_2_src=
			"precision mediump float;\n"
			"uniform sampler2D u_orientation_1_tex;\n"
			"varying vec4 v_vtx_fpt_coord;\n"
			"const float pi=3.14159;\n"
			"const float pi_2=2.0*pi;\n"
			"const float pi_i2=1.0/pi_2;\n"
			"const float pi_i3=pi/3.0;\n"
			"const float pi_5i3=5.0*pi_i3;\n"
			"const float inv_42=1.0/42.0;\n"
			"\n"
			"vec4 pack_orientation_to_vec4(in float o)\n"
			"{\n"
			"	const vec3 bit_shift=vec3(256.0, 1.0, 1.0/256.0);\n"
			"	const vec3 bit_mask=vec3(0.0, 1.0/256.0, 1.0/256.0);\n"
			"	vec3 res=fract((256.0*(o*pi_i2))*bit_shift);\n"
			"	res-=res.xxy*bit_mask;\n"
			"	return vec4(res, 0.0);\n"
			"}\n"
			"vec2 unpack_res_from_vec4(in vec4 r)\n"
			"{\n"
			"	const vec2 bit_shift=vec2(1.0/256.0, 1.0);\n"
			"	return vec2(\n"
			"		10.0*(dot(r.xy, bit_shift)-0.5),\n"
			"		10.0*(dot(r.zw, bit_shift)-0.5)\n"
			"	);\n"
			"}\n"
			"\n"
			"float get_angle(in vec2 r)\n"
			"{\n"
			"	if(r.y<0.0)\n"
			"	{\n"
			"		if(r.x<0.0) return pi+atan(r.y/r.x);\n"
			"		if(r.x>0.0) return pi_2-atan(-r.y/r.x);\n"
			"	}\n"
			"	else\n"
			"	{\n"
			"		if(r.x<0.0) return pi-atan(-r.y/r.x);\n"
			"		if(r.x>0.0) return atan(r.y/r.x);\n"
			"	}\n"
			"	return 0.0;\n"
			"}\n"
			"\n"
			"void main()\n"
			"{\n"
			"	float max_sum=0.0;\n"
			"	float ori=0.0;\n"
			"	for(int a=0;a<42;++a)\n"
			"	{\n"
			"		float ang1=float(a)*0.15;\n"
			"		float ang2=ang1+pi_i3>pi_2?ang1-pi_5i3:ang1+pi_i3;\n"
			"		vec2 sum=vec2(0.0);\n"
			"		for(int k=0;k<109;++k)\n"
			"		{\n"
			"			vec2 res=unpack_res_from_vec4(texture2D(u_orientation_1_tex, vec2(gl_FragCoord.x*109.0/128.0, v_vtx_fpt_coord.y)));\n"
			"			float ang=get_angle(res);\n"
			"			if(ang1<ang2 && ang1<ang && ang<ang2)\n"
			"				sum+=res;\n"
			"			else\n"
			"			if(ang2<ang1 && ((ang>0.0 && ang<ang2) || (ang>ang1 && ang<pi_2)))\n"
			"				sum+=res;\n"
			"		}\n"
			"		float a_sum_dot=dot(sum, sum);\n"
			"		if(a_sum_dot>max_sum)\n"
			"		{\n"
			"			max_sum=a_sum_dot;\n"
			"			ori=get_angle(sum);\n"
			"		}\n"
			"	}\n"
			"	gl_FragColor=pack_orientation_to_vec4(ori);\n"
			"}\n"
		;


		static const std::size_t OCTAVES=5;
		static const std::size_t INTERVALS=4;


		static const std::size_t filter_map[OCTAVES][INTERVALS]=
		{
			{	0,	1,	2,	3	},
			{	1,	3,	4,	5	},
			{	3,	5,	6,	7	},
			{	5,	7,	8,	9	},
			{	7,	9,	10,	11	}
		};
	} //namespace anonymous

	hessian_detector::response_layer::response_layer()
		: m_response_offset(0,0)
		, m_response_size(0,0)
		, m_sample_step(0)
		, m_filter_size(0)
	{
	}

	hessian_detector::response_layer::response_layer(point2ui const &ro, size2ui const &rs, boost::uint32_t ss, boost::uint32_t fs)
		: m_response_offset(ro)
		, m_response_size(rs)
		, m_sample_step(ss)
		, m_filter_size(fs)
	{
	}

	hessian_detector::response_layer::response_layer(response_layer const &that)
	{
		*this=that;
	}

	hessian_detector::response_layer& hessian_detector::response_layer::operator =(response_layer const &rhs)
	{
		m_response_offset=rhs.m_response_offset;
		m_response_size=rhs.m_response_size;
		m_sample_step=rhs.m_sample_step;
		m_filter_size=rhs.m_filter_size;
		return *this;
	}

	hessian_detector::response_layer::~response_layer()
	{
	}

	hessian_detector::response_layer::operator bool() const
	{
		return !m_response_size.empty();
	}

	point2ui const& hessian_detector::response_layer::get_offset() const
	{
		return m_response_offset;
	}

	size2ui const& hessian_detector::response_layer::get_size() const
	{
		return m_response_size;
	}

	boost::uint32_t hessian_detector::response_layer::get_sample_step() const
	{
		return m_sample_step;
	}


	hessian_detector::detection_layer::detection_layer()
		: m_detection_offset(0,0)
		, m_detection_size(0,0)
	{
	}

	hessian_detector::detection_layer::detection_layer(point2ui const &o, size2ui const &s)
		: m_detection_offset(o)
		, m_detection_size(s)
	{
	}

	hessian_detector::detection_layer::detection_layer(detection_layer const &that)
	{
		*this=that;
	}

	hessian_detector::detection_layer& hessian_detector::detection_layer::operator =(detection_layer const &rhs)
	{
		m_detection_offset=rhs.m_detection_offset;
		m_detection_size=rhs.m_detection_size;
		return *this;
	}

	hessian_detector::detection_layer::~detection_layer()
	{
	}

	hessian_detector::detection_layer::operator bool() const
	{
		return !m_detection_size.empty();
	}

	point2ui const& hessian_detector::detection_layer::get_offset() const
	{
		return m_detection_offset;
	}

	size2ui const& hessian_detector::detection_layer::get_size() const
	{
		return m_detection_size;
	}

	hessian_detector::hessian_detector(size2ui const &is, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf)
		: m_integral_img_size(0,0)
		, m_octaves(o)
		, m_intervals(i)
		, m_sample_step(s)
		, m_treshold(t)
		, m_max_features(mf)
	{
		if(!create_programs())
		{
#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "hessian_detector::hessian_detector: create_shaders() failed!");
#endif
		}
		if(!resize(is))
		{
#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "hessian_detector::hessian_detector: resize() failed!");
#endif
		}
	}

	hessian_detector::~hessian_detector()
	{
	}

	hessian_detector::operator bool() const
	{
		if(!(m_hessian_response_program && m_integral_texture && m_fbo && m_response_texture))
			return false;

		if(m_response_layers.empty())
			return false;

		return true;
	}
	
	bool hessian_detector::create_programs()
	{
		if(!create_hessian_response_program())
			return false;
		if(!create_feature_detection_program())
			return false;
		if(!create_orientation_program())
			return false;
		return true;
	}

	bool hessian_detector::create_hessian_response_program()
	{
		if(m_hessian_response_program)
		{
			ucv::gles::vertex_shader hessian_response_vs;
			ucv::gles::fragment_shader hessian_response_fs;
			if(hessian_response_vs && hessian_response_fs)
			{

				hessian_response_vs.set_source(vs_hessian_response_src);
				hessian_response_vs.compile();
				if(!hessian_response_vs.is_compiled())
				{
#if defined(ANDROID)
					__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_shaders: hessian_response_vs.is_compiled()=false!");
#endif
					return false;
				}

				hessian_response_fs.set_source(fs_hessian_response_src);
				hessian_response_fs.compile();
				if(!hessian_response_fs.is_compiled())
				{
#if defined(ANDROID)
					__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_shaders: hessian_response_fs.is_compiled()=false!");
#endif
					return false;
				}

#if defined(ANDROID)
				__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_shaders: shaders compiled, linking ...");
#endif
				if(	m_hessian_response_program.attach(hessian_response_vs) && m_hessian_response_program.attach(hessian_response_fs) &&
					m_hessian_response_program.link() && m_hessian_response_program.is_linked()
				)
				{
					return true;
				}
			}
		}
		else
		{
#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "hessian_detector::create_shaders: m_hessian_response_program is not valid!");
#endif
		}
		return false;
	}

	bool hessian_detector::create_feature_detection_program()
	{
		if(m_feature_detection_program)
		{
			ucv::gles::vertex_shader feature_detection_vs;
			ucv::gles::fragment_shader feature_detection_fs;
			if(feature_detection_vs && feature_detection_fs)
			{

				feature_detection_vs.set_source(vs_feature_detection_src);
				feature_detection_vs.compile();
				if(!feature_detection_vs.is_compiled())
				{
#if defined(ANDROID)
					__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_feature_detection_program: feature_detection_vs.is_compiled()=false!");
#endif
					return false;
				}

				feature_detection_fs.set_source(fs_feature_detection_src);
				feature_detection_fs.compile();
				if(!feature_detection_fs.is_compiled())
				{
#if defined(ANDROID)
					__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_feature_detection_program: feature_detection_fs.is_compiled()=false!");
#endif
					return false;
				}

#if defined(ANDROID)
				__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "hessian_detector::create_feature_detection_program: shaders compiled, linking ...");
#endif
				if(	m_feature_detection_program.attach(feature_detection_vs) && m_feature_detection_program.attach(feature_detection_fs) &&
					m_feature_detection_program.link() && m_feature_detection_program.is_linked()
					)
				{
					return true;
				}
			}
		}
		else
		{
#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "hessian_detector::create_feature_detection_program: m_feature_detection_program is not valid!");
#endif
		}
		return false;
	}


	bool hessian_detector::create_orientation_program()
	{
		if(!m_orientation_1_program)
			return false;

		ucv::gles::vertex_shader orientation_1_vs;
		ucv::gles::fragment_shader orientation_1_fs;
		if(!orientation_1_vs || !orientation_1_fs)
			return false;
		
		orientation_1_vs.set_source(vs_orientation_1_src);
		orientation_1_vs.compile();
		if(!orientation_1_vs.is_compiled())
			return false;
		
		orientation_1_fs.set_source(fs_orientation_1_src);
		orientation_1_fs.compile();
		if(!orientation_1_fs.is_compiled())
			return false;

		if(!(m_orientation_1_program.attach(orientation_1_vs) && m_orientation_1_program.attach(orientation_1_fs) && m_orientation_1_program.link() && m_orientation_1_program.is_linked()))
			return false;

		if(!m_orientation_2_program)
			return false;

		ucv::gles::vertex_shader orientation_2_vs;
		ucv::gles::fragment_shader orientation_2_fs;
		if(!orientation_2_vs || !orientation_2_fs)
			return false;

		orientation_2_vs.set_source(vs_orientation_2_src);
		orientation_2_vs.compile();
		if(!orientation_2_vs.is_compiled())
			return false;

		orientation_2_fs.set_source(fs_orientation_2_src);
		orientation_2_fs.compile();
		if(!orientation_2_fs.is_compiled())
			return false;

		if(!(m_orientation_2_program.attach(orientation_2_vs) && m_orientation_2_program.attach(orientation_2_fs) && m_orientation_2_program.link() && m_orientation_2_program.is_linked()))
			return false;
		
		return true;
	}

	
	bool hessian_detector::resize(size2ui const &is)
	{
		if(!m_integral_texture)
			return false;

		if(m_integral_img_size==is)
			return true;

		if(is.empty())
			return false;
		
		m_integral_img_size=is;

		size2ui integral_texture_size(
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(m_integral_img_size.width())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			),
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(m_integral_img_size.height())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			)
		);

		{
			GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
			m_integral_texture.bind();
			m_integral_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			m_integral_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			m_integral_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			m_integral_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			m_integral_texture.resize(integral_texture_size);
			ucv::gles::rgba8_texture_2d::bind(old_tex);
		}
		
		return resize_response() && resize_detection() && resize_descript();
	}

	bool hessian_detector::resize_response()
	{
		if(!m_response_texture)
			return false;

		m_response_layers.clear();

		boost::uint32_t	width=m_integral_img_size.width()/m_sample_step;
		boost::uint32_t	height=m_integral_img_size.height()/m_sample_step;
		

		size2ui response_img_sz_1(
			2*width+width/2,
			2*height
		);

		size2ui response_img_sz_1_pow2(
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(response_img_sz_1.width())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			),
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(response_img_sz_1.height())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			)
		);

		size2ui response_img_sz_2(
			2*width,
			2*height+height/2
		);

		size2ui response_img_sz_2_pow2(
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(response_img_sz_2.width())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			),
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(response_img_sz_2.height())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			)
		);
		
		if((response_img_sz_1_pow2.area()-response_img_sz_1.area())<(response_img_sz_1_pow2.area()-response_img_sz_2.area()))
		{
			{
				GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
				m_response_texture.bind();
				m_response_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				m_response_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				m_response_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				m_response_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				m_response_texture.resize(response_img_sz_1_pow2);
				ucv::gles::rgba8_texture_2d::bind(old_tex);
			}

			if(m_octaves>=1)
			{
				m_response_layers.push_back(response_layer(point2ui(0, 0), size2ui(width, height), m_sample_step, 9));
				m_response_layers.push_back(response_layer(point2ui(width, 0), size2ui(width, height), m_sample_step, 15));
				m_response_layers.push_back(response_layer(point2ui(0, height), size2ui(width, height), m_sample_step, 21));
				m_response_layers.push_back(response_layer(point2ui(width, height), size2ui(width, height), m_sample_step, 27));
			}

			if(m_octaves>=2)
			{
				m_response_layers.push_back(response_layer(point2ui(2*width, 0), size2ui(width/2, height/2), m_sample_step*2, 39));
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)), size2ui(width/2, height/2), m_sample_step*2, 51));
			}

			if(m_octaves>=3)
			{
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)), size2ui(width/4, height/4), m_sample_step*4, 75));
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)+height/4), size2ui(width/4, height/4), m_sample_step*4, 99));
			}

			if(m_octaves>=4)
			{
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)+2*(height/4)), size2ui(width/8, height/8), m_sample_step*8, 147));
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)+2*(height/4)+height/8), size2ui(width/8, height/8), m_sample_step*8, 195));
			}

			if(m_octaves>=5)
			{
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)+2*(height/4)+2*(height/8)), size2ui(width/16, height/16), m_sample_step*16, 291));
				m_response_layers.push_back(response_layer(point2ui(2*width, 2*(height/2)+2*(height/4)+2*(height/8)+height/16), size2ui(width/16, height/16), m_sample_step*16, 387));
			}
		}
		else
		{
			{
				GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
				m_response_texture.bind();
				m_response_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				m_response_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				m_response_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				m_response_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				m_response_texture.resize(response_img_sz_2_pow2);
				ucv::gles::rgba8_texture_2d::bind(old_tex);
			}

			if(m_octaves>=1)
			{
				m_response_layers.push_back(response_layer(point2ui(0, 0), size2ui(width, height), m_sample_step, 9));
				m_response_layers.push_back(response_layer(point2ui(width, 0), size2ui(width, height), m_sample_step, 15));
				m_response_layers.push_back(response_layer(point2ui(0, height), size2ui(width, height), m_sample_step, 21));
				m_response_layers.push_back(response_layer(point2ui(width, height), size2ui(width, height), m_sample_step, 27));
			}

			if(m_octaves>=2)
			{
				m_response_layers.push_back(response_layer(point2ui(0, 2*height), size2ui(width/2, height/2), m_sample_step*2, 39));
				m_response_layers.push_back(response_layer(point2ui(width/2, 2*height), size2ui(width/2, height/2), m_sample_step*2, 51));
			}

			if(m_octaves>=3)
			{
				m_response_layers.push_back(response_layer(point2ui(2*(width/2), 2*height), size2ui(width/4, height/4), m_sample_step*4, 75));
				m_response_layers.push_back(response_layer(point2ui(2*(width/2)+width/4, 2*height), size2ui(width/4, height/4), m_sample_step*4, 99));
			}

			if(m_octaves>=4)
			{
				m_response_layers.push_back(response_layer(point2ui(2*(width/2)+2*(width/4), 2*height), size2ui(width/8, height/8), m_sample_step*8, 147));
				m_response_layers.push_back(response_layer(point2ui(2*(width/2)+2*(width/4)+width/8, 2*height), size2ui(width/8, height/8), m_sample_step*8, 195));
			}

			if(m_octaves>=5)
			{
				m_response_layers.push_back(response_layer(point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height), size2ui(width/16, height/16), m_sample_step*16, 291));
				m_response_layers.push_back(response_layer(point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height), size2ui(width/16, height/16), m_sample_step*16, 387));
			}
		}
		return true;
		
	}

	bool hessian_detector::resize_detection()
	{
		if(!m_feature_detection_texture)
			return false;

		m_detection_layers.clear();

		boost::uint32_t	width=m_integral_img_size.width()/m_sample_step;
		boost::uint32_t	height=m_integral_img_size.height()/m_sample_step;


		size2ui detection_sz_1(
			width+width/2,
			2*height
		);

		size2ui detection_sz_1_pow2(
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(detection_sz_1.width())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			),
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(detection_sz_1.height())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			)
		);

		size2ui detection_sz_2(
			2*width,
			height+height/2
		);

		size2ui detection_sz_2_pow2(
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(detection_sz_2.width())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			),
			static_cast<boost::uint32_t>(
				std::pow(2.0f,
					std::ceil(
						static_cast<float>(std::log(static_cast<float>(detection_sz_2.height())))/
						static_cast<float>(std::log(2.0f))
					)
				)
			)
		);

		if((detection_sz_1_pow2.area()-detection_sz_1.area())<(detection_sz_2_pow2.area()-detection_sz_2.area()))
		{
			m_feature_detection_img.recreate(detection_sz_1_pow2.width(), detection_sz_1_pow2.height());
			{
				GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
				m_feature_detection_texture.bind();
				m_feature_detection_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				m_feature_detection_texture.resize(detection_sz_1_pow2);
				ucv::gles::rgba8_texture_2d::bind(old_tex);
			}

			if(m_octaves>=1)
			{
				m_detection_layers.push_back(detection_layer(point2ui(0,0), size2ui(width, height)));
				m_detection_layers.push_back(detection_layer(point2ui(0,height), size2ui(width, height)));
			}

			if(m_octaves>=2)
			{
				m_detection_layers.push_back(detection_layer(point2ui(width,0), size2ui(width/2, height/2)));
				m_detection_layers.push_back(detection_layer(point2ui(width,height/2), size2ui(width/2, height/2)));
			}

			if(m_octaves>=3)
			{
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)), size2ui(width/4, height/4)));
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)+height/4), size2ui(width/4, height/4)));
			}

			if(m_octaves>=4)
			{
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)+2*(height/4)), size2ui(width/8, height/8)));
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)+2*(height/4)+height/8), size2ui(width/8, height/8)));
			}

			if(m_octaves>=4)
			{
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)+2*(height/4)+2*(height/8)), size2ui(width/16, height/16)));
				m_detection_layers.push_back(detection_layer(point2ui(width, 2*(height/2)+2*(height/4)+2*(height/8)+height/16), size2ui(width/16, height/16)));
			}
		}
		else
		{
			m_feature_detection_img.recreate(detection_sz_2_pow2.width(), detection_sz_2_pow2.height());
			{
				GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
				m_feature_detection_texture.bind();
				m_feature_detection_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				m_feature_detection_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				m_feature_detection_texture.resize(detection_sz_2_pow2);
				ucv::gles::rgba8_texture_2d::bind(old_tex);
			}

			if(m_octaves>=1)
			{
				m_detection_layers.push_back(detection_layer(point2ui(0,0), size2ui(width, height)));
				m_detection_layers.push_back(detection_layer(point2ui(width,0), size2ui(width, height)));
			}

			if(m_octaves>=2)
			{
				m_detection_layers.push_back(detection_layer(point2ui(0,height), size2ui(width/2, height/2)));
				m_detection_layers.push_back(detection_layer(point2ui(width/2,height), size2ui(width/2, height/2)));
			}

			if(m_octaves>=3)
			{
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2), height), size2ui(width/4, height/4)));
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2)+width/4, height), size2ui(width/4, height/4)));
			}

			if(m_octaves>=4)
			{
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2)+2*(width/4), height), size2ui(width/8, height/8)));
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2)+2*(width/4)+width/8, height), size2ui(width/8, height/8)));
			}

			if(m_octaves>=5)
			{
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2)+2*(width/4)+2*(width/8), height), size2ui(width/16, height/16)));
				m_detection_layers.push_back(detection_layer(point2ui(2*(width/2)+2*(width/4)+2*(width/8)+width/16, height), size2ui(width/16, height/16)));
			}
		}
		return true;
	}

	bool hessian_detector::resize_descript()
	{
		if(!(m_orientation_1_texture && m_orientation_2_texture && m_descript_texture))
			return false;

		boost::uint32_t max_features_pow2=static_cast<boost::uint32_t>(std::pow(2.0f,
			std::ceil(
				static_cast<float>(std::log(static_cast<float>(m_max_features)))/
				static_cast<float>(std::log(2.0f))
			)
		));

		m_desc_vtx_pos.reset(new float[2*max_features_pow2*4]);
		m_desc_vtx_tex_coords.reset(new float[2*max_features_pow2*3]);
						

		GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
		m_orientation_1_texture.bind();
		m_orientation_1_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_orientation_1_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_orientation_1_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_orientation_1_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_orientation_1_texture.resize(size2ui(128, max_features_pow2));

		m_orientation_2_texture.bind();
		m_orientation_2_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_orientation_2_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_orientation_2_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_orientation_2_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_orientation_2_texture.resize(size2ui(1, max_features_pow2));

		m_descript_texture.bind();
		m_descript_texture.set_parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_descript_texture.set_parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_descript_texture.set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_descript_texture.set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_descript_texture.resize(size2ui(16, max_features_pow2));
		ucv::gles::rgba8_texture_2d::bind(old_tex);
		return true;
	}

	bool hessian_detector::set_integral_img(ucv::gil::rgba8_view_t iv)
	{
		if(!m_integral_texture)
			return false;

		if(iv.width()!=m_integral_img_size.width() || iv.height()!=m_integral_img_size.height())
			return false;

		GLuint old_tex=ucv::gles::rgba8_texture_2d::current_binding();
		m_integral_texture.bind();
		m_integral_texture.upload(
			0,
			ucv::point2ui(0,0),
			iv
		);
		ucv::gles::rgba8_texture_2d::bind(old_tex);
		return true;
	}

	bool hessian_detector::build_response_layers()
	{
		if(!(m_hessian_response_program && m_integral_texture && m_fbo))
			return false;

		if(m_response_layers.empty())
			return false;

		static float const vtx_pos[] =
		{
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f
		};

		float const int_tex_u_span=float(m_integral_img_size.width()-1)/float(m_integral_texture.get_size().width());
		float const int_tex_v_span=float(m_integral_img_size.height()-1)/float(m_integral_texture.get_size().height());

		float const vtx_tex_coord[]=
		{
			0.0f,				int_tex_v_span,
			0.0f,				0.0f,
			int_tex_u_span, 	int_tex_v_span,
			int_tex_u_span,		0.0f
		};
		
		GLuint old_fbuf=frame_buffer::current_binding();
		m_fbo.bind();

		m_fbo.attach_buffer(m_response_texture);
		if(!frame_buffer::is_complete())
			return false;

		glViewport(
			0,0,
			m_integral_texture.get_size().width(),m_integral_texture.get_size().height()
		);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT);

		GLuint old_tex=m_integral_texture.bind();

		if(!m_hessian_response_program.use())
			return false;
		
		//bind attributes
		vertex_shader::attribute a_vtx_pos=m_hessian_response_program.get_attribute("a_vtx_pos");
		BOOST_ASSERT(a_vtx_pos);
		glVertexAttribPointer(
			a_vtx_pos.get_location(),
			4,
			GL_FLOAT,
			0, 0,
			vtx_pos
		);
		glEnableVertexAttribArray(a_vtx_pos.get_location());

		vertex_shader::attribute a_vtx_tex_coord=m_hessian_response_program.get_attribute("a_vtx_tex_coord");
		BOOST_ASSERT(a_vtx_tex_coord);
		glVertexAttribPointer(
			a_vtx_tex_coord.get_location(),
			2,
			GL_FLOAT,
			0, 0,
			vtx_tex_coord
		);
		glEnableVertexAttribArray(a_vtx_tex_coord.get_location());
		
		program::uniform u_integral_img_span=m_hessian_response_program.get_uniform("u_integral_img_span");
		BOOST_ASSERT(u_integral_img_span);
		glUniform2f(u_integral_img_span.get_location(), int_tex_u_span, int_tex_v_span);

		program::uniform u_integral_tex_size=m_hessian_response_program.get_uniform("u_integral_tex_size");
		BOOST_ASSERT(u_integral_tex_size);
		glUniform2i(u_integral_tex_size.get_location(), m_integral_texture.get_size().width(), m_integral_texture.get_size().height());
		
		program::uniform u_layer_filter_size=m_hessian_response_program.get_uniform("u_layer_filter_size");
		BOOST_ASSERT(u_layer_filter_size);

		for(std::size_t rl=0;rl<m_response_layers.size();++rl)
		{
			
			response_layer r_layer=m_response_layers[rl];
			BOOST_ASSERT(r_layer);
			glViewport(
				r_layer.m_response_offset.x,
				r_layer.m_response_offset.y,
				r_layer.m_response_size.width(),
				r_layer.m_response_size.height()
				);
			glUniform1i(u_layer_filter_size.get_location(), r_layer.m_filter_size);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		}
		
		glDisableVertexAttribArray(a_vtx_pos.get_location());
		glDisableVertexAttribArray(a_vtx_tex_coord.get_location());

		rgba8_texture_2d::bind(old_tex);
		frame_buffer::bind(old_fbuf);
		return true;
	}

	bool hessian_detector::build_detection_layers()
	{
		if(!(m_feature_detection_program && m_response_texture && m_feature_detection_texture && m_fbo))
			return false;
		
		if(m_detection_layers.empty())
			return false;

		static float const vtx_pos[] =
		{
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 1.0f,
			1.0f,  1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f
		};

		GLuint old_fbuf=frame_buffer::current_binding();
		m_fbo.bind();

		m_fbo.attach_buffer(m_feature_detection_texture);
		if(!frame_buffer::is_complete())
		{
			frame_buffer::bind(old_fbuf);
			return false;
		}

		glViewport(
			0,0,
			m_feature_detection_texture.get_size().width(), m_feature_detection_texture.get_size().height()
		);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glClear(GL_COLOR_BUFFER_BIT);

		GLuint old_tex=rgba8_texture_2d::current_binding();
		m_response_texture.bind();

		if(!m_feature_detection_program.use())
		{
			rgba8_texture_2d::bind(old_tex);
			frame_buffer::bind(old_fbuf);
			return false;
		}

		vertex_shader::attribute a_vtx_pos=m_feature_detection_program.get_attribute("a_vtx_pos");
		BOOST_ASSERT(a_vtx_pos);
		glEnableVertexAttribArray(a_vtx_pos.get_location());

		vertex_shader::attribute a_vtx_tex_coord=m_feature_detection_program.get_attribute("a_vtx_tex_coord");
		BOOST_ASSERT(a_vtx_tex_coord);
		glEnableVertexAttribArray(a_vtx_tex_coord.get_location());


		program::uniform u_resp_trshld=m_feature_detection_program.get_uniform("u_resp_trshld");
		BOOST_ASSERT(u_resp_trshld);
		glUniform1f(u_resp_trshld.get_location(), m_treshold);

		program::uniform u_resp_tex_sz=m_feature_detection_program.get_uniform("u_resp_tex_sz");
		BOOST_ASSERT(u_resp_tex_sz);
		glUniform2i(u_resp_tex_sz.get_location(), m_response_texture.get_size().width(), m_response_texture.get_size().height());
				

		program::uniform u_top_resp_off=m_feature_detection_program.get_uniform("u_top_resp_off");
		BOOST_ASSERT(u_top_resp_off);
		program::uniform u_top_resp_sz=m_feature_detection_program.get_uniform("u_top_resp_sz");
		BOOST_ASSERT(u_top_resp_sz);

		program::uniform u_mid_resp_off=m_feature_detection_program.get_uniform("u_mid_resp_off");
		BOOST_ASSERT(u_mid_resp_off);
		program::uniform u_mid_resp_sz=m_feature_detection_program.get_uniform("u_mid_resp_sz");
		BOOST_ASSERT(u_mid_resp_sz);

		program::uniform u_btm_resp_off=m_feature_detection_program.get_uniform("u_btm_resp_off");
		BOOST_ASSERT(u_btm_resp_off);
		program::uniform u_btm_resp_sz=m_feature_detection_program.get_uniform("u_btm_resp_sz");
		BOOST_ASSERT(u_btm_resp_sz);


		program::uniform u_top_step=m_feature_detection_program.get_uniform("u_top_step");
		BOOST_ASSERT(u_top_step);
		program::uniform u_top_filter_size=m_feature_detection_program.get_uniform("u_top_filter_size");
		BOOST_ASSERT(u_top_filter_size);
		program::uniform u_top_det_sz=m_feature_detection_program.get_uniform("u_top_det_sz");
		BOOST_ASSERT(u_top_det_sz);
		
		
		glVertexAttribPointer(
			a_vtx_pos.get_location(),
			4,
			GL_FLOAT,
			0, 0,
			vtx_pos
		);

		for(std::size_t octave=0;octave<m_octaves;++octave)
		{
			for(std::size_t i=0;i<2;++i)
			{
				response_layer const &bottom_layer=m_response_layers[filter_map[octave][i]];
				response_layer const &middle_layer=m_response_layers[filter_map[octave][i+1]];
				response_layer const &top_layer=m_response_layers[filter_map[octave][i+2]];
				detection_layer const &top_det_layer=m_detection_layers[2*octave+i];

				point2f tl_offset(
					float(top_layer.get_offset().x)/float(m_response_texture.get_size().width()),
					float(top_layer.get_offset().y)/float(m_response_texture.get_size().height())
				);
				size2f tl_size(
					float(top_layer.get_size().width())/float(m_response_texture.get_size().width()),
					float(top_layer.get_size().height())/float(m_response_texture.get_size().height())
				);
				glUniform2f(u_top_resp_off.get_location(), tl_offset.x, tl_offset.y);
				glUniform2f(u_top_resp_sz.get_location(), tl_size.width(), tl_size.height());

				point2f ml_offset(
					float(middle_layer.get_offset().x)/float(m_response_texture.get_size().width()),
					float(middle_layer.get_offset().y)/float(m_response_texture.get_size().height())
				);
				size2f ml_size(
					float(middle_layer.get_size().width())/float(m_response_texture.get_size().width()),
					float(middle_layer.get_size().height())/float(m_response_texture.get_size().height())
				);
				glUniform2f(u_mid_resp_off.get_location(), ml_offset.x, ml_offset.y);
				glUniform2f(u_mid_resp_sz.get_location(), ml_size.width(), ml_size.height());

				point2f bl_offset(
					float(bottom_layer.get_offset().x)/float(m_response_texture.get_size().width()),
					float(bottom_layer.get_offset().y)/float(middle_layer.get_size().height())
				);
				size2f bl_size(
					float(bottom_layer.get_size().width())/float(m_response_texture.get_size().width()),
					float(bottom_layer.get_size().height())/float(m_response_texture.get_size().height())
				);
				glUniform2f(u_btm_resp_off.get_location(), bl_offset.x, bl_offset.y);
				glUniform2f(u_btm_resp_sz.get_location(), bl_size.width(), bl_size.height());

				glUniform1i(u_top_step.get_location(), top_layer.m_sample_step );
				glUniform1i(u_top_filter_size.get_location(), top_layer.m_filter_size );
				glUniform2i(u_top_det_sz.get_location(), top_det_layer.m_detection_size.width(), top_det_layer.m_detection_size.height());

				float const vtx_tex_coord[]=
				{
					tl_offset.x,					tl_offset.y+tl_size.height(),
					tl_offset.x,					tl_offset.y,
					tl_offset.x+tl_size.width(), 	tl_offset.y+tl_size.height(),
					tl_offset.x+tl_size.width(),	tl_offset.y
				};
				
				glViewport(
					top_det_layer.m_detection_offset.x,
					top_det_layer.m_detection_offset.y,
					top_det_layer.m_detection_size.width(),
					top_det_layer.m_detection_size.height()
				);
				
				glVertexAttribPointer(
					a_vtx_tex_coord.get_location(),
					2,
					GL_FLOAT,
					0, 0,
					vtx_tex_coord
				);

				glDrawArrays(GL_TRIANGLE_STRIP,0,4);
			}
		}
		
		glDisableVertexAttribArray(a_vtx_tex_coord.get_location());
		glDisableVertexAttribArray(a_vtx_pos.get_location());

		glReadPixels(
			0,0,
			m_feature_detection_img.width(), m_feature_detection_img.height(),
			GL_RGBA, GL_UNSIGNED_BYTE,
			&(gil::view(m_feature_detection_img)[0][0])
		);

		rgba8_texture_2d::bind(old_tex);
		frame_buffer::bind(old_fbuf);
		return true;
	}

	bool hessian_detector::collect_detected_points(std::vector<feature_point> &fps)
	{
		typedef gil::channel_type<gil::rgba8_image_t>::type channel_t;
		fps.clear();
		gil::rgba8_view_t d_view=gil::view(m_feature_detection_img);

		for(std::size_t octave=0;octave<m_octaves;++octave)
		{
			for(std::size_t i=0;i<2;++i)
			{
				response_layer const &bottom_layer=m_response_layers[filter_map[octave][i]];
				response_layer const &middle_layer=m_response_layers[filter_map[octave][i+1]];
				response_layer const &top_layer=m_response_layers[filter_map[octave][i+2]];
				detection_layer const &top_det_layer=m_detection_layers[2*octave+i];

				int filter_step=(middle_layer.m_filter_size-bottom_layer.m_filter_size);
				
				for(std::size_t y=0;y<top_det_layer.get_size().height();++y)
				{
					channel_t *p_row=reinterpret_cast<channel_t*>(d_view.row_begin(top_det_layer.get_offset().y+y)+4*top_det_layer.get_offset().x);
					for(std::size_t x=0;x<top_det_layer.get_size().width();++x)
					{
						if(p_row[3] || p_row[2] || p_row[1] || p_row[0])
						{
							//float fpt_x=((float(p_row[0])/255.0f-0.5f)+x)*float(top_layer.m_sample_step);
							//float fpt_y=((float(p_row[1])/255.0f-0.5f)+y)*float(top_layer.m_sample_step);
							float fpt_x=((float(p_row[0]+((p_row[2]&0x0f)<<8))/float(0x0fff)-0.5f)+x)*float(top_layer.m_sample_step);
							float fpt_y=((float(p_row[1]+((p_row[2]&0xf0)<<4))/float(0x0fff)-0.5f)+y)*float(top_layer.m_sample_step);
							float fpt_scale=0.1333f*(middle_layer.m_filter_size+(float(p_row[3])/255.0f-0.5f)*filter_step);
							fps.push_back(feature_point(point2f(fpt_x,fpt_y),fpt_scale));
						}
						p_row+=4;
					}
				}
			}
		}
		return true;
	}

	bool hessian_detector::compute_orientations(std::vector<feature_point> &fps)
	{
		if(!(m_orientation_1_program && m_orientation_1_texture && m_desc_vtx_pos && m_desc_vtx_tex_coords && m_fbo))
			return false;

		GLuint old_fbuf=frame_buffer::current_binding();
		m_fbo.bind();

		m_fbo.attach_buffer(m_orientation_1_texture);
		if(!frame_buffer::is_complete())
		{
			frame_buffer::bind(old_fbuf);
			return false;
		}

		boost::uint32_t n_features=std::min<boost::uint32_t>(fps.size(), m_max_features);
		
		glViewport(
			0,0,
			109, n_features
		);
		
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		//glClear(GL_COLOR_BUFFER_BIT);
		
		GLuint old_tex=rgba8_texture_2d::current_binding();
		m_integral_texture.bind();
		

		if(!m_orientation_1_program.use())
		{
			rgba8_texture_2d::bind(old_tex);
			frame_buffer::bind(old_fbuf);
			return false;
		}
				
		float const int_tex_u_span=float(m_integral_img_size.width()-1)/float(m_integral_texture.get_size().width());
		float const int_tex_v_span=float(m_integral_img_size.height()-1)/float(m_integral_texture.get_size().height());

		float const vtx_step=2.0f/float(n_features);
		float *p_vtx_pos=m_desc_vtx_pos.get();
		float *p_vtx_tex_coord=m_desc_vtx_tex_coords.get();

		for(boost::uint32_t ifp=0;ifp<n_features;++ifp)
		{
			p_vtx_pos[0]=-1.0f; p_vtx_pos[4]=1.0f;
			p_vtx_pos[1]=p_vtx_pos[5]=-1.0f+2.0f*float(ifp+1)/float(n_features);
			
			p_vtx_pos[2]=p_vtx_pos[6]=0.0f;
			p_vtx_pos[3]=p_vtx_pos[7]=1.0f;

			p_vtx_tex_coord[0]=p_vtx_tex_coord[3]=int_tex_u_span*fps[ifp].x/float(m_integral_img_size.width());
			p_vtx_tex_coord[1]=p_vtx_tex_coord[4]=int_tex_v_span*fps[ifp].y/float(m_integral_img_size.height());
			p_vtx_tex_coord[2]=p_vtx_tex_coord[5]=fps[ifp].m_scale;

			p_vtx_pos+=8;
			p_vtx_tex_coord+=6;
		}

		//bind attributes
		vertex_shader::attribute a_vtx_pos=m_orientation_1_program.get_attribute("a_vtx_pos");
		BOOST_ASSERT(a_vtx_pos);
		glVertexAttribPointer(
			a_vtx_pos.get_location(),
			4,
			GL_FLOAT,
			0, 0,
			m_desc_vtx_pos.get()
		);
		glEnableVertexAttribArray(a_vtx_pos.get_location());


		vertex_shader::attribute a_vtx_fpt=m_orientation_1_program.get_attribute("a_vtx_fpt");
		BOOST_ASSERT(a_vtx_fpt);
		glVertexAttribPointer(
			a_vtx_fpt.get_location(),
			3,
			GL_FLOAT,
			0, 0,
			m_desc_vtx_tex_coords.get()
		);
		glEnableVertexAttribArray(a_vtx_fpt.get_location());

		program::uniform u_integral_img_span=m_orientation_1_program.get_uniform("u_integral_img_span");
		BOOST_ASSERT(u_integral_img_span);
		glUniform2f(u_integral_img_span.get_location(), int_tex_u_span, int_tex_v_span);
				
		program::uniform u_integral_tex_size=m_orientation_1_program.get_uniform("u_integral_tex_size");
		BOOST_ASSERT(u_integral_tex_size);
		glUniform2i(u_integral_tex_size.get_location(), m_integral_texture.get_size().width(), m_integral_texture.get_size().height());
		
		program::uniform u_gauss_25=m_orientation_1_program.get_uniform("u_gauss_25");
		BOOST_ASSERT(u_gauss_25);
		glUniform1fv(u_gauss_25.get_location(), ORI_GAUSS25_SIZE, ori_gauss_25);

		program::uniform u_indices=m_orientation_1_program.get_uniform("u_indices");
		BOOST_ASSERT(u_indices);
		glUniform2iv(u_indices.get_location(), ORI_INDICES_SIZE, ori_indices::get().m_values);
				
		glDrawArrays(GL_LINES, 0, 2*n_features);
		glDisableVertexAttribArray(a_vtx_fpt.get_location());
		glDisableVertexAttribArray(a_vtx_pos.get_location());
		
		
		
#if 0
		gil::rgba8_image_t os(109,n_features);
		gil::rgba8_view_t os_view=gil::view(os);
		glReadPixels(
			0,0,
			109, n_features,
			GL_RGBA, GL_UNSIGNED_BYTE,
			&(os_view[0][0])
			);

		float min_res_x=std::numeric_limits<float>::max();
		float max_res_x=std::numeric_limits<float>::min();

		typedef gil::channel_type<gil::rgba8_image_t>::type channel_t;
		for(boost::uint32_t ifp=0;ifp<n_features;++ifp)
		{
			channel_t *p_row=reinterpret_cast<channel_t*>(os_view.row_begin(ifp));
			for(int x=0;x<109;++x)
			{
				float resx=10.0f*(float(p_row[x*4+0])/(255.0f*256.0f)+float(p_row[x*4+1])/255.0f-0.5f);
				float resy=10.0f*(float(p_row[x*4+2])/(255.0f*256.0f)+float(p_row[x*4+3])/255.0f-0.5f);

				if(!x)
				{
				
					min_res_x=std::min(min_res_x,resx);
					max_res_x=std::max(max_res_x,resx);
				}

				if(!p_row[x*4+2] && !p_row[x*4+3])
				{
					int c=0;
				}
			}
		}
#endif
		m_fbo.attach_buffer(m_orientation_2_texture);
		if(!frame_buffer::is_complete())
		{
			frame_buffer::bind(old_fbuf);
			return false;
		}

		glViewport(
			0,0,
			1, n_features
		);

		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glDisable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		//glClear(GL_COLOR_BUFFER_BIT);

		m_orientation_1_texture.bind();

		if(!m_orientation_2_program.use())
		{
			rgba8_texture_2d::bind(old_tex);
			frame_buffer::bind(old_fbuf);
			return false;
		}

		p_vtx_pos=m_desc_vtx_pos.get();
		p_vtx_tex_coord=m_desc_vtx_tex_coords.get();

		for(boost::uint32_t ifp=0;ifp<n_features;++ifp)
		{
			p_vtx_pos[0]=-1.0; p_vtx_pos[4]=1.0f;
			p_vtx_pos[1]=p_vtx_pos[5]=-1.0f+2.0f*float(ifp+1)/float(n_features);
			p_vtx_pos[2]=p_vtx_pos[6]=0.0f;
			p_vtx_pos[3]=p_vtx_pos[7]=1.0f;

			p_vtx_tex_coord[0]=p_vtx_tex_coord[2]=0.0f;
			p_vtx_tex_coord[1]=p_vtx_tex_coord[3]=float(ifp)/float(m_orientation_1_texture.get_size().height());
			
			p_vtx_pos+=8;
			p_vtx_tex_coord+=4;
		}

		//bind attributes
		vertex_shader::attribute a_vtx_pos_2=m_orientation_2_program.get_attribute("a_vtx_pos");
		BOOST_ASSERT(a_vtx_pos_2);
		glVertexAttribPointer(
			a_vtx_pos_2.get_location(),
			4,
			GL_FLOAT,
			0, 0,
			m_desc_vtx_pos.get()
		);
		glEnableVertexAttribArray(a_vtx_pos_2.get_location());

		vertex_shader::attribute a_vtx_fpt_2=m_orientation_2_program.get_attribute("a_vtx_fpt");
		BOOST_ASSERT(a_vtx_fpt_2);
		glVertexAttribPointer(
			a_vtx_fpt_2.get_location(),
			2,
			GL_FLOAT,
			0, 0,
			m_desc_vtx_tex_coords.get()
		);
		glEnableVertexAttribArray(a_vtx_fpt_2.get_location());

		glDrawArrays(GL_LINES, 0, 2*n_features);
		glDisableVertexAttribArray(a_vtx_pos_2.get_location());
		glDisableVertexAttribArray(a_vtx_fpt_2.get_location());

#if 0
		gil::rgba8_image_t os2(1,n_features);
		gil::rgba8_view_t os2_view=gil::view(os2);
		glReadPixels(
			0,0,
			1, n_features,
			GL_RGBA, GL_UNSIGNED_BYTE,
			&(os2_view[0][0])
		);

		float min_res_x2=std::numeric_limits<float>::max();
		float max_res_x2=std::numeric_limits<float>::min();

		for(boost::uint32_t ifp=0;ifp<n_features;++ifp)
		{
			typedef gil::channel_type<gil::rgba8_image_t>::type channel_t;
			channel_t *p_row=reinterpret_cast<channel_t*>(os2_view.row_begin(ifp));
#if 0
			float resx=10.0f*(float(p_row[0])/(255.0f*256.0f)+float(p_row[1])/255.0f-0.5f);
			float resy=10.0f*(float(p_row[2])/(255.0f*256.0f)+float(p_row[3])/255.0f-0.5f);

			min_res_x2=std::min(min_res_x2,resx);
			max_res_x2=std::max(max_res_x2,resx);
#else
			float angle=	float(p_row[0])/(255.0f*256.0f*256.0f)+
							float(p_row[1])/(255.0f*256.0f)+
							float(p_row[2])/(255.0f);

			angle=angle*2.0f*3.14f;

#endif

			int c=0;
		}
#endif

		rgba8_texture_2d::bind(old_tex);
		frame_buffer::bind(old_fbuf);

		return true;
	}


	rgba8_texture_2d const& hessian_detector::get_response_texture() const
	{
		return m_response_texture;
	}

	rgba8_texture_2d const& hessian_detector::get_detection_texture() const
	{
		return m_feature_detection_texture;
	}

	rgba8_texture_2d const& hessian_detector::get_integral_texture() const
	{
		return m_integral_texture;
	}

	hessian_detector::response_layers_t const&	hessian_detector::get_response_layers() const
	{
		return m_response_layers;
	}

	hessian_detector::detection_layers_t const&	hessian_detector::get_detection_layers() const
	{
		return m_detection_layers;
	}

} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2
