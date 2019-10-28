#pragma once

#include "common.h"
#include <vector>

#include"Shader.h"
#include"VertexArray.h"

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include"pnpchange.h"
//Only for triangles
class Model {
public:
	std::vector<Eigen::Vector3f> v_p;  //頂点座標
	std::vector<Eigen::Vector3f> v_pp;  //頂点座標
	std::vector<Eigen::Vector3f> v_n;  //頂点法線
	std::vector<Eigen::Vector3f> v_nn;  //頂点法線
	std::vector<Eigen::Vector2f> v_t;  //頂点tex座標
	std::vector<Eigen::Vector2f> v_tt;  //頂点tex座標
	std::vector<unsigned int> f_i;  //面頂点座標インデックス
	std::vector<unsigned int> f_n;
	std::vector<unsigned int> f_t;

	std::vector<float> varray;
	


	Model() {};
	~Model() {};

	void loadObj(char* filename = NULL) {
		v_p.clear();
		v_n.clear();
		v_t.clear();
		f_i.clear();
		

		if (filename != NULL) {
			FILE* fp = fopen(filename, "r");
			if (fp == NULL) {
				printf("File was NOT found.\n");
			}

			char str[1024];
			int tmpi[4];
			int tmpt[4];
			int tmpn[4];
			float tmpf[3];
			std::vector<Eigen::Vector2f> tmpvt;

			while ((fgets(str, 1024, fp)) != NULL) {
				if (str[0] == 'v' && str[1] == ' ') {
					sscanf(str, "v %f %f %f\n", &tmpf[0], &tmpf[1], &tmpf[2]);
					v_p.push_back(Eigen::Vector3f(tmpf[0], tmpf[1], tmpf[2]));
				}
				else if (str[0] == 'v' && str[1] == 'n') {
					sscanf(str, "vn %f %f %f\n", &tmpf[0], &tmpf[1], &tmpf[2]);
					v_n.push_back(Eigen::Vector3f(tmpf[0], tmpf[1], tmpf[2]));
				}
				else if (str[0] == 'v' && str[1] == 't') {
					sscanf(str, "vt %f %f\n", &tmpf[0], &tmpf[1]);
					v_t.push_back(Eigen::Vector2f(tmpf[0], tmpf[1]));
				}
				else if (str[0] == 'f' && str[1] == ' ') {
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					f_i.push_back(vertexIndex[0]-1);
					f_i.push_back(vertexIndex[1]-1);
					f_i.push_back(vertexIndex[2]-1);
					f_n.push_back(normalIndex[0] - 1);
					f_n.push_back(normalIndex[1] - 1);
					f_n.push_back(normalIndex[2] - 1);
					f_t.push_back(uvIndex[0] - 1);
					f_t.push_back(uvIndex[1] - 1);
					f_t.push_back(uvIndex[2] - 1);

				}
			}
			for(int i=0;i<f_n.size();i++){
				v_nn.push_back(v_n[f_n[i]]);
				v_pp.push_back(v_p[f_i[i]]);
				v_tt.push_back(v_t[f_t[i]]);

				varray.push_back(v_p[f_i[i]][0]);
				varray.push_back(v_p[f_i[i]][1]);
				varray.push_back(v_p[f_i[i]][2]);
				varray.push_back(v_n[f_n[i]][0]);
				varray.push_back(v_n[f_n[i]][1]);
				varray.push_back(v_n[f_n[i]][2]);
				varray.push_back(v_t[f_t[i]][0]);
				varray.push_back(v_t[f_t[i]][1]);
			
			}
			
		}
		else {
			v_p.push_back(Eigen::Vector3f(-1.0, -1.0, 0.0));
			v_p.push_back(Eigen::Vector3f(+1.0, -1.0, 0.0));
			v_p.push_back(Eigen::Vector3f(+1.0, +1.0, 0.0));
			v_p.push_back(Eigen::Vector3f(-1.0, +1.0, 0.0));

			f_i.push_back(0);
			f_i.push_back(1);
			f_i.push_back(2);
			f_i.push_back(3);

			v_t.push_back(Eigen::Vector2f(1.0, 0.0));
			v_t.push_back(Eigen::Vector2f(0.0, 0.0));
			v_t.push_back(Eigen::Vector2f(0.0, 1.0));
			v_t.push_back(Eigen::Vector2f(1.0, 1.0));
		}

		//頂点法線
		if (v_n.size() == 0 && f_i.size() != 0) {
			Eigen::Vector3f a, b;
			Eigen::Vector3f fn;
			std::vector<Eigen::Vector3f> tmpvnv(v_p.size(), Eigen::Vector3f(0, 0, 0));

			v_n = tmpvnv;

			for (int i = 0; i < f_i.size() / 3; i++) {
				a = v_p[f_i[i * 3 + 1]] - v_p[f_i[i * 3 + 0]];
				b = v_p[f_i[i * 3 + 2]] - v_p[f_i[i * 3 + 0]];
				fn = a.cross(b);
				fn.normalize();
				for (int j = 0; j < 3; j++) {
					v_n[f_i[i * 3 + j]] += fn;
				}
			}

			for (int i = 0; i < v_n.size(); i++) {
				v_n[i].normalize();
			}
		}
	}
};

class GLkit {
public:
	//Projection
	int render_size[2];
	float render_aspect;

	//ModelView
	float view_fov;
	Eigen::Quaternion<GLfloat> view_rot;
	Eigen::Transform< GLfloat, 3, Eigen::Affine> view_rotmat;

	Model model;
	GLuint texture_id[40];

	bool modelFlag = false;

	Shader shader;
	VertexArray vertexArray;
	glm::mat4 projection;
	glm::mat4 M;
	glm::mat4 viewMat;
	cv::Mat H;

	glm::mat4 homography;

	
	float objpoints[12] = { 0.0865f,0.071f,0 ,
							-0.0805f,0.071f,0,
							-0.0805f,-0.0945f,0,
							
							0.054f,-0.0649f,-0.019f
	};
	
	/*
	float objpoints[12] = { -25.0f / 2.0f,-28.0f / 2.0f,0,
							25.0f / 2.0f,-28.0f / 2.0f,0,
							25.0f / 2.0f,28.0f / 2.0f,0,
							-25.0f / 2.0f,28.0f / 2.0f,0
	};*/


	MVmatrix mvMatrix;
public:
	//Constructor
	GLkit(int width, int height,cv::Mat m_H) :
		mvMatrix("intrinsics.xml"),
		//Projection
		render_size{ width, height },
		render_aspect(width * 1.0 / height),

		//ModelView
		view_fov(0.5),
		view_rot(Eigen::Quaternion<GLfloat>::Identity()),
		H(m_H)
	{
		//ModelView
		projection = glm::mat4(1.0);
		M = glm::mat4(1.0);
		

	}

	//Destructor
	~GLkit() {
	}

	void loadObj(char* fileName = NULL) {
		model.loadObj(fileName);
		if (fileName != NULL) {
			modelFlag = true;
		}
		vertexArray.load(model.varray.data(), static_cast<unsigned>(model.varray.size())/8);
		vertexArray.SetActive();
		
		
	}

	virtual void setup() {
		cv::Mat texture[40];
		texture[0] = cv::imread("white.png");


		// The directions of axes are different between CV and GL
		//cv::flip(texture[1], texture[1], 0);// The directions of axes are different between CV and GL
		//cv::flip(texture[2], texture[2], 0);// The directions of axes are different between CV and GL
		glGenTextures(40, texture_id);
		/*
		for (int i = 0; i < 40; i++) {
			glBindTexture(GL_TEXTURE_2D, texture_id[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[i].cols, texture[i].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[i].data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		*/
		shader.Load("Shader/simple.vert", "Shader/uvmap.frag");

		//shader.SetTextureUniform("image", tex);
		

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		
		//glEnable(GL_LIGHT0);
		//glEnable(GL_TEXTURE_2D);
		//projection = glm::ortho(-view_fov * render_aspect, view_fov * render_aspect, -view_fov, view_fov, -50.0f, 50.0f);
		
		
		//H.at<double>(1, 1) *= -1;
		//H.at<double>(0, 1) = 0;
		//H.at<double>(1, 0) = 0;
		//mvMatrix.intrinsics_matrix = H * mvMatrix.intrinsics_matrix;
		
		cameraFrustumRH(mvMatrix.intrinsics_matrix, cv::Size(648,474), projection, 0.1, 100.0);
		//cameraFrustumRH(mvMatrix.intrinsics_matrix, cv::Size(render_size[0],render_size[1]), projection, 0.1, 100.0);
		//cout << to_string(projection) << endl;
		//projection = glm::perspective((float)glm::radians(31.68), render_aspect, 0.01f, 100.0f);
		//cout << to_string(projection) << endl;
		changeHomography(H, homography);
		//cout << to_string(homography) << endl;
		//cout << mvMatrix.intrinsics_matrix << endl;
		
		viewMat=glm::mat4(1.0)
			* glm::lookAt(
				glm::vec3(0, 0, 0), // カメラの原点
				glm::vec3(0, 0, 1), // 見ている点
				glm::vec3(0, 1, 0)  // カメラの上方向
			);
		//cout <<glm::to_string( projection)<< endl;
		M = glm::translate(M, glm::vec3(0.0, 0.0, 1.0f));
		shader.SetActive();
		//shader.printProgram();
		shader.SetMatrixUniform("MVP", projection*M );
		//shader.SetFloatUniform("U", 0.0f);
	
		//Projection
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		glViewport(0, 0, render_size[0], render_size[1]);
		//glOrtho(-view_fov * render_aspect, view_fov * render_aspect, -view_fov, view_fov, 0.1f, 10.0f);
		//glFrustum(-view_fov * render_aspect, view_fov * render_aspect, -view_fov, view_fov, 0.1f, 10.0f);
		//gluPerspective(22.4, render_aspect, 0.1f, 10.0f);
		
		//glTranslated(0.0, 0.0, -1.0);
		//glScalef(0.1f, 0.1f, 0.1f);
  
		//ModelView
		//glMatrixMode(GL_MODELVIEW);
		
		//glLoadIdentity();
		//glScalef(0.1f, 0.1f, 0.1f);
		//glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

		//Lighting
		/*
		GLfloat l4Position[] = { 0.0f, 0.0f, 5.0f, 0.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, l4Position);

		//Material
		GLfloat f4Ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
		GLfloat f4Diffuse[] = { 0.70f, 0.70f, 0.70f, 1.0f };
		GLfloat f4Specular[] = { 0.30f, 0.30f, 0.30f, 1.0f };
		GLfloat fShininess = 0.8f;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, f4Ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, f4Diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, f4Specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, fShininess);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		*/
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		printf("GL Renderer  :%s\n", renderer);
		printf("GLSL Version :%s\n", glslVersion);
	}

	//Display
	virtual void render(float* input_xyz, cv::Mat* img = NULL, int texid = 0) {

		if (modelFlag) {
			//cout << "Y" << endl;
			glClearColor(0, 0.0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			//Rendering
			vertexArray.SetActive();

			mvMatrix.change3Dpoint(objpoints, input_xyz, M, 4);
			glm::mat4 RotX = glm::mat4(1, 0, 0, 0,
				0, -1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
			shader.SetMatrixUniform("MVP", homography *RotX* projection*viewMat*M);
			//shader.SetMatrixUniform("MVP", RotX * projection * viewMat * M);
			//shader.SetMatrixUniform("MVP",projection*viewMat*M);
			//shader.SetMatrixUniform("MVP", M*viewMat*projection);
			//glm::vec4 a = homography*projection * viewMat * M * glm::vec4(0.0, 0.0, 0.0, 1.0);
			//a /= a.a;
			//cout << glm::to_string(a) << endl;

			//glDrawElements(GL_TRIANGLES, model.varray.size()/8, GL_UNSIGNED_INT, nullptr);
			glDrawArrays(GL_TRIANGLES, 0, model.varray.size()/8);
		}
		else {
			//Initialize
			glClearColor(0, 0.0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Rendering
			glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)input_xyz);
			glEnableClientState(GL_VERTEX_ARRAY);

			glNormalPointer(GL_FLOAT, 0, (GLfloat*) & (model.v_n[0][0]));
			glEnableClientState(GL_NORMAL_ARRAY);

			glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*) & (model.v_t[0][0]));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);


			glBindTexture(GL_TEXTURE_2D, texture_id[texid]);
			glDrawElements(GL_QUADS, model.f_i.size(), GL_UNSIGNED_INT, &model.f_i[0]);


		}
		if (img != NULL && img->cols == render_size[0] && img->rows == render_size[1]) {
			//glReadPixels(0, 0, render_size[0], render_size[1], GL_RGB, GL_UNSIGNED_BYTE, img->data);
			glReadPixels(0, 0, render_size[0], render_size[1], GL_RED, GL_UNSIGNED_BYTE, img->data);
		}
	}

	virtual void render(float* input_xyz, cv::Mat* img, cv::Mat renderimg) {

		glClearColor(0, 0.0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Rendering
		glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)input_xyz);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, 0, (GLfloat*) & (model.v_n[0][0]));
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*) & (model.v_t[0][0]));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		

		//glBindTexture(GL_TEXTURE_2D, tex);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, renderimg.cols, renderimg.rows, 0, GL_RED, GL_UNSIGNED_BYTE, renderimg.data);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, renderimg.cols, renderimg.rows, GL_RED, GL_UNSIGNED_BYTE, renderimg.data);
		glDrawElements(GL_QUADS, model.f_i.size(), GL_UNSIGNED_INT, &model.f_i[0]);

		if (img != NULL && img->cols == render_size[0] && img->rows == render_size[1]) {
			//glReadPixels(0, 0, render_size[0], render_size[1], GL_RGB, GL_UNSIGNED_BYTE, img->data);
			glReadPixels(0, 0, render_size[0], render_size[1], GL_RED, GL_UNSIGNED_BYTE, img->data);
		}
	}

	//Mouse
	void setFov(float y_offset) {
		view_fov -= y_offset / 10.0;
		if (view_fov <= 0.1) {
			view_fov = 0.1;
		}
		else if (view_fov >= 1.0) {
			view_fov = 1.0;
		}
	}

	void touchStart(const Eigen::Vector2f& pos) {
	}

	void touchMove(const Eigen::Vector2f& pos, const Eigen::Vector2f& last_pos) {
		Eigen::Vector2f d = pos - last_pos;

		float len = d.norm();
		if (len <= 0.0f) {
			return;
		}

		Eigen::Vector3f vec = Eigen::Vector3f(d.y(), d.x(), 0.0f).normalized();
		Eigen::Quaternion<GLfloat> q(Eigen::AngleAxisf(len * 0.01f, vec));
		view_rot = q * view_rot;
	}

	void touchEnd(const Eigen::Vector2f& pos) {
	}

	void cameraFrustumRH(cv::Mat camMat, cv::Size camSz, glm::mat4& projMat, double znear, double zfar)
	{
		// Load camera parameters
		double fx = camMat.at<double>(0, 0);
		double fy = camMat.at<double>(1, 1);
		double cx = camMat.at<double>(0, 2);
		double cy = camMat.at<double>(1, 2);
		double w = camSz.width, h = camSz.height;

	

		// 参考:https://strawlab.org/2011/11/05/augmented-reality-with-OpenGL
		// With window_coords=="y_down", we have:
		// [2 * K00 / width,   -2 * K01 / width,   (width - 2 * K02 + 2 * x0) / width,     0]
		// [0,                 2 * K11 / height,   (-height + 2 * K12 + 2 * y0) / height,  0]
		// [0,                 0,                  (-zfar - znear) / (zfar - znear),       -2 * zfar*znear / (zfar - znear)]
		// [0,                 0,                  -1,                                     0]

		
		glm::mat4 projection(
			2.0 * fx / w, 0, 0, 0,
			0, 2.0 * fy / h, 0, 0,
			1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear), -1.0,
			0, 0, -2.0 * zfar * znear / (zfar - znear), 0);
			
		
		/*
		glm::mat4 projection(
			4.71976, 0, 0, 0,
			0, 6.45233, 0, 0,
			1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear), -1.0,
			0, 0, -2.0 * zfar * znear / (zfar - znear), 0);
		*/
		
		projMat = projection;
		
	}
	
	void changeHomography(cv::Mat m_H, glm::mat4& newmat) {
		glm::mat4 projection(
			m_H.at<double>(0,0), m_H.at<double>(1, 0), 0, m_H.at<double>(2, 0),
			m_H.at<double>(0, 1), m_H.at<double>(1, 1), 0, m_H.at<double>(2, 1),
			0, 0, 1.0, 0,
			m_H.at<double>(0, 2), m_H.at<double>(1, 2), 0,1);
		projection = glm::transpose(projection);
		newmat = projection;
		
	}
	

private:
};
