#pragma once
#include"GLKit.h"

extern void launchVertexProcess(unsigned int NUM_POINTS, float(*d_virtualpoints)[3], float dt, glm::mat4& M);
extern void initField(unsigned int num_points,vector<float>& position);
class GLkitD :public GLkit {
private:
	Shader oshader;

	GLuint FramebufferName;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

	VertexArray virtualVertexArray;


	float(*d_realpoints)[3];
	float(*d_virtualpoints)[3];

public:

	GLkitD(int window_w, int window_h, cv::Mat H) :
		GLkit(window_w, window_h, H) {

	}

	void loadObj(char* fileName = NULL) {
		model.loadObj(fileName);
		if (fileName != NULL) {
			modelFlag = true;
		}
		vertexArray.load(model.position, model.normal, model.texcoord, static_cast<unsigned>(model.position.size()) / 3);
		//vertexArray.SetActive();

		virtualVertexArray.load(model.position, model.normal, model.texcoord, static_cast<unsigned>(model.position.size()) / 3);
		//vertexArray.SetActive();
		initField(model.position.size() / 3,model.position);
	}

	void setup() {
		cv::Mat texture[40];
		texture[0] = cv::imread("white.png");


		// The directions of axes are different between CV and GL
		//cv::flip(texture[1], texture[1], 0);// The directions of axes are different between CV and GL
		//cv::flip(texture[2], texture[2], 0);// The directions of axes are different between CV and GL
		//ccccccccccglGenTextures(40, texture_id);
		/*
		for (int i = 0; i < 40; i++) {
			glBindTexture(GL_TEXTURE_2D, texture_id[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[i].cols, texture[i].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[i].data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		*/
		shader.Load("Shader/simple.vert", "Shader/uvmap.frag");
		oshader.Load("Shader/override.vert", "Shader/override.frag");

		//shader.printProgram();
		//oshader.printProgram();
		//shader.SetTextureUniform("image", tex);


		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);

		//glEnable(GL_LIGHT0);
		//glEnable(GL_TEXTURE_2D);
		//projection = glm::ortho(-view_fov * render_aspect, view_fov * render_aspect, -view_fov, view_fov, -50.0f, 50.0f);


		cameraFrustumRH(mvMatrix.intrinsics_matrix, cv::Size(648, 474), projection, 0.1, 100.0);
		//cameraFrustumRH(mvMatrix.intrinsics_matrix, cv::Size(render_size[0],render_size[1]), projection, 0.1, 100.0);
		//cout << to_string(projection) << endl;
		//projection = glm::perspective((float)glm::radians(31.68), render_aspect, 0.01f, 100.0f);
		//cout << to_string(projection) << endl;
		changeHomography(H, homography);
		//cout << to_string(homography) << endl;
		//cout << mvMatrix.intrinsics_matrix << endl;

		viewMat = glm::mat4(1.0)
			* glm::lookAt(
				glm::vec3(0, 0, 0), // カメラの原点
				glm::vec3(0, 0, 1), // 見ている点
				glm::vec3(0, 1, 0)  // カメラの上方向
			);
		//cout <<glm::to_string( projection)<< endl;
		M = glm::translate(M, glm::vec3(0.0, 0.0, 1.0f));
		//shader.SetActive();
		//shader.printProgram();
		//shader.SetMatrixUniform("MVP", projection * M);
		//shader.SetFloatUniform("U", 0.0f);

		//Projection
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		
		FramebufferName = 0;
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		glGenTextures(1, &renderedTexture);
		glBindTexture(GL_TEXTURE_2D, renderedTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, render_size[0], render_size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
		glDrawBuffers(1, DrawBuffers);
		glViewport(0, 0, render_size[0], render_size[1]);
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

	void render(float* input_xyz, cv::Mat* img = NULL, int texid = 0) {
		//cout << "r" << endl;
		glClearColor(0.2, 0.2, 0.2,0.0 );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		cudaGraphicsMapResources(1, &(virtualVertexArray.vbo_res), 0);
		cudaGraphicsResourceGetMappedPointer((void**)&d_virtualpoints, NULL, virtualVertexArray.vbo_res);

		launchVertexProcess(model.position.size() / 3, d_virtualpoints, 0.033f, M);

		cudaGraphicsUnmapResources(1, &(virtualVertexArray.vbo_res), 0);
		//Rendering
		vertexArray.SetActive();

		mvMatrix.change3Dpoint(objpoints, input_xyz, M, 4);
		glm::mat4 RotX = glm::mat4(1, 0, 0, 0,
			0, -1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
		shader.SetActive();
		shader.SetMatrixUniform("MVP",  RotX * projection * viewMat * M);

		//glm::vec4 a = homography*projection * viewMat * M * glm::vec4(0.0, 0.0, 0.0, 1.0);
		//a /= a.a;
		//cout << glm::to_string(a) << endl;
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, render_size[0], render_size[1]);
		
		glDrawArrays(GL_TRIANGLES, 0, model.position.size() / 3);
		


		virtualVertexArray.SetActive();
		oshader.SetActive();

		oshader.SetMatrixUniform("VP",RotX* projection * viewMat);
		oshader.SetMatrixUniform("MVP",RotX* projection * viewMat*M);
		//oshader.SetMatrixUniform("MVP", RotX * projection * viewMat * M);
		oshader.SetTextureUniform("renderedTexture", renderedTexture);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, model.position.size() / 3);

	
		if (img!= NULL && img->cols == render_size[0] && img->rows == render_size[1]) {
			//glReadPixels(0, 0, render_size[0], render_size[1], GL_RGB, GL_UNSIGNED_BYTE, img->data);
			glReadPixels(0, 0, render_size[0], render_size[1], GL_RED, GL_UNSIGNED_BYTE, img->data);
		}
	}
	
};